//
// Copyright 2009-present MongoDB, Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <mongocxx/v1/database.hh>

//

#include <bsoncxx/v1/array/view.hpp>

#include <mongocxx/v1/aggregate_options.hpp>
#include <mongocxx/v1/pipeline.hpp>
#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/write_concern.hh>

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

struct database_mocks_type {
    using client_destroy_type = decltype(libmongoc::client_destroy.create_instance());
    using database_destroy_type = decltype(libmongoc::database_destroy.create_instance());
    using database_get_name_type = decltype(libmongoc::database_get_name.create_instance());

    identity_type client_identity;
    identity_type database_identity;

    mongoc_client_t* client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);
    mongoc_database_t* database_id = reinterpret_cast<mongoc_database_t*>(&database_identity);

    client_destroy_type client_destroy = libmongoc::client_destroy.create_instance();
    database_destroy_type database_destroy = libmongoc::database_destroy.create_instance();
    database_get_name_type database_get_name = libmongoc::database_get_name.create_instance();

    v1::client client = v1::client::internal::make(client_id);

    ~database_mocks_type() = default;
    database_mocks_type(database_mocks_type&& other) noexcept = delete;
    database_mocks_type& operator=(database_mocks_type&& other) noexcept = delete;
    database_mocks_type(database_mocks_type const& other) = delete;
    database_mocks_type& operator=(database_mocks_type const& other) = delete;

    database_mocks_type() {
        client_destroy
            ->interpose([&](mongoc_client_t* ptr) -> void {
                if (ptr) {
                    CHECK(ptr == client_id);
                }
            })
            .forever();

        database_destroy
            ->interpose([&](mongoc_database_t* ptr) -> void {
                if (ptr) {
                    CHECK(ptr == database_id);
                }
            })
            .forever();

        database_get_name
            ->interpose([&](mongoc_database_t* ptr) -> char const* {
                CHECK(ptr == database_id);
                FAIL("should not reach this point");
                return nullptr;
            })
            .forever();
    }

    v1::database make() {
        return v1::database::internal::make(database_id, client_id);
    }
};

struct session_mocks_type {
    using destroy_type = decltype(libmongoc::client_session_destroy.create_instance());
    using append_type = decltype(libmongoc::client_session_append.create_instance());

    identity_type session_identity;

    mongoc_client_session_t* session_id = reinterpret_cast<mongoc_client_session_t*>(&session_identity);

    destroy_type destroy = libmongoc::client_session_destroy.create_instance();
    append_type append = libmongoc::client_session_append.create_instance();

    scoped_bson doc{R"({"sessionId": 123})"};

    session_mocks_type() {
        destroy
            ->interpose([&](mongoc_client_session_t* ptr) -> void {
                if (ptr) {
                    CHECK(ptr == session_id);
                }
            })
            .forever();

        append
            ->interpose([&](mongoc_client_session_t const* ptr, bson_t* out, bson_error_t* error) -> bool {
                CHECK(ptr == session_id);
                REQUIRE(out != nullptr);
                CHECK(error != nullptr);

                bson_copy_to(doc.bson(), out);

                return true;
            })
            .forever();
    }

    client_session make(v1::client& client) {
        return v1::client_session::internal::make(session_id, client);
    }
};

} // namespace

TEST_CASE("exceptions", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    auto const v = GENERATE(as<int>(), 1, 2, 3);
    auto const msg = GENERATE("one", "two", "three");

    CAPTURE(v);
    CAPTURE(msg);

    auto const set_client_error = [&](bson_error_t* error) {
        REQUIRE(error != nullptr);
        bson_set_error(error, MONGOC_ERROR_CLIENT, static_cast<std::uint32_t>(v), "%s", msg);
        error->reserved = 2; // MONGOC_ERROR_CATEGORY
    };

    auto const set_server_error = [&](bson_error_t* error) {
        REQUIRE(error != nullptr);
        bson_set_error(error, MONGOC_ERROR_SERVER, static_cast<std::uint32_t>(v), "%s", msg);
        error->reserved = 3; // MONGOC_ERROR_CATEGORY_SERVER
    };

    SECTION("mongoc") {
        SECTION("run_command") {
            scoped_bson const command_input{R"({"x": 1})"};

            SECTION("v1::exception") {
                auto const raw = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

                SECTION("basic") {
                    auto with_opts = libmongoc::database_command_with_opts.create_instance();
                    with_opts->interpose(
                        [&](mongoc_database_t* ptr,
                            bson_t const* command,
                            mongoc_read_prefs_t const* read_prefs,
                            bson_t const* opts,
                            bson_t* reply,
                            bson_error_t* error) -> bool {
                            CHECK(ptr == mocks.database_id);
                            CHECK(scoped_bson_view{command}.data() == command_input.data());
                            CHECK(read_prefs == nullptr);
                            CHECK(opts == nullptr);
                            REQUIRE(reply != nullptr);
                            REQUIRE(error != nullptr);

                            set_client_error(error);
                            bson_copy_to(raw.bson(), reply);

                            return false;
                        });

                    try {
                        (void)db.run_command(command_input.view());
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::mongoc);
                        CHECK(ex.code().value() == v);
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                        CHECK(v1::exception::internal::get_reply(ex) == raw.view());
                    }
                }

                SECTION("server_id") {
                    auto const input = GENERATE(as<std::uint32_t>(), 1, 2, 3);

                    char const name = '\0';
                    mocks.database_get_name->interpose([&](mongoc_database_t* ptr) {
                        CHECK(ptr == mocks.database_id);
                        return &name;
                    });

                    identity_type read_prefs_identity;
                    auto const read_prefs_id = reinterpret_cast<mongoc_read_prefs_t const*>(&read_prefs_identity);
                    auto database_get_read_prefs = libmongoc::database_get_read_prefs.create_instance();
                    database_get_read_prefs->interpose([&](mongoc_database_t const* ptr) -> mongoc_read_prefs_t const* {
                        CHECK(ptr == mocks.database_id);
                        return read_prefs_id;
                    });

                    auto client_command_simple_with_server_id =
                        libmongoc::client_command_simple_with_server_id.create_instance();
                    client_command_simple_with_server_id->interpose(
                        [&](mongoc_client_t* ptr,
                            char const* db_name,
                            bson_t const* command,
                            mongoc_read_prefs_t const* read_prefs,
                            std::uint32_t server_id,
                            bson_t* reply,
                            bson_error_t* error) -> bool {
                            CHECK(ptr == mocks.client_id);
                            CHECK(static_cast<void const*>(db_name) == static_cast<void const*>(&name));
                            CHECK(scoped_bson_view{command}.data() == command_input.data());
                            CHECK(read_prefs == read_prefs_id);
                            CHECK(server_id == input);
                            REQUIRE(reply != nullptr);
                            REQUIRE(error != nullptr);

                            set_client_error(error);
                            bson_copy_to(raw.bson(), reply);

                            return false;
                        });

                    try {
                        (void)db.run_command(command_input.view(), input);
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::mongoc);
                        CHECK(ex.code().value() == v);
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                        CHECK(v1::exception::internal::get_reply(ex) == raw.view());
                    }
                }
            }

            SECTION("v1::server_error") {
                scoped_bson const raw{BCON_NEW("code", BCON_INT32(std::int32_t{v}))};

                SECTION("basic") {
                    auto with_opts = libmongoc::database_command_with_opts.create_instance();
                    with_opts->interpose(
                        [&](mongoc_database_t* ptr,
                            bson_t const* command,
                            mongoc_read_prefs_t const* read_prefs,
                            bson_t const* opts,
                            bson_t* reply,
                            bson_error_t* error) -> bool {
                            CHECK(ptr == mocks.database_id);
                            CHECK(scoped_bson_view{command}.data() == command_input.data());
                            CHECK(read_prefs == nullptr);
                            CHECK(opts == nullptr);
                            REQUIRE(reply != nullptr);
                            REQUIRE(error != nullptr);

                            set_server_error(error);
                            bson_copy_to(raw.bson(), reply);

                            return false;
                        });

                    try {
                        (void)db.run_command(command_input.view());
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::server);
                        CHECK(ex.code().value() == v);
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));

                        auto const ptr = dynamic_cast<v1::server_error const*>(&ex);
                        REQUIRE(ptr);
                        CHECK(ptr->raw() == raw.view());
                    }
                }

                SECTION("server_id") {
                    auto const input = GENERATE(as<std::uint32_t>(), 1, 2, 3);

                    char const name = '\0';
                    mocks.database_get_name->interpose([&](mongoc_database_t* ptr) {
                        CHECK(ptr == mocks.database_id);
                        return &name;
                    });

                    identity_type read_prefs_identity;
                    auto const read_prefs_id = reinterpret_cast<mongoc_read_prefs_t const*>(&read_prefs_identity);
                    auto database_get_read_prefs = libmongoc::database_get_read_prefs.create_instance();
                    database_get_read_prefs->interpose([&](mongoc_database_t const* ptr) -> mongoc_read_prefs_t const* {
                        CHECK(ptr == mocks.database_id);
                        return read_prefs_id;
                    });

                    auto client_command_simple_with_server_id =
                        libmongoc::client_command_simple_with_server_id.create_instance();
                    client_command_simple_with_server_id->interpose(
                        [&](mongoc_client_t* ptr,
                            char const* db_name,
                            bson_t const* command,
                            mongoc_read_prefs_t const* read_prefs,
                            std::uint32_t server_id,
                            bson_t* reply,
                            bson_error_t* error) -> bool {
                            CHECK(ptr == mocks.client_id);
                            CHECK(static_cast<void const*>(db_name) == static_cast<void const*>(&name));
                            CHECK(scoped_bson_view{command}.data() == command_input.data());
                            CHECK(read_prefs == read_prefs_id);
                            CHECK(server_id == input);
                            REQUIRE(reply != nullptr);
                            REQUIRE(error != nullptr);

                            set_server_error(error);
                            bson_copy_to(raw.bson(), reply);

                            return false;
                        });

                    try {
                        (void)db.run_command(command_input.view(), input);
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::server);
                        CHECK(ex.code().value() == v);
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));

                        auto const ptr = dynamic_cast<v1::server_error const*>(&ex);
                        REQUIRE(ptr);
                        CHECK(ptr->raw() == raw.view());
                    }
                }
            }
        }

        SECTION("drop") {
            auto const raw = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

            auto drop_with_opts = libmongoc::database_drop_with_opts.create_instance();
            drop_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> bool {
                CHECK(ptr == mocks.database_id);
                CHECK(opts == nullptr);
                REQUIRE(error != nullptr);

                set_client_error(error);

                return false;
            });

            try {
                db.drop();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("has_collection") {
            auto const input = GENERATE(values<bsoncxx::v1::stdx::string_view>({{}, "x", "abc"}));

            auto has_collection = libmongoc::database_has_collection.create_instance();
            has_collection->interpose([&](mongoc_database_t* ptr, char const* name, bson_error_t* error) -> bool {
                CHECK(ptr == mocks.database_id);
                CHECK(name == input);
                REQUIRE(error != nullptr);

                set_client_error(error);

                return false;
            });

            try {
                (void)db.has_collection(input);
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("list_collection_names") {
            auto get_collection_names_with_opts = libmongoc::database_get_collection_names_with_opts.create_instance();
            get_collection_names_with_opts->interpose(
                [&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                    CHECK(ptr == mocks.database_id);
                    CHECK(opts != nullptr);
                    REQUIRE(error != nullptr);

                    set_client_error(error);

                    return nullptr;
                });

            try {
                (void)db.list_collection_names();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("session") {
            session_mocks_type session_mocks;
            auto const session = session_mocks.make(mocks.client);

            session_mocks.append
                ->interpose([&](mongoc_client_session_t const* ptr, bson_t* out, bson_error_t* error) -> bool {
                    CHECK(ptr == session_mocks.session_id);
                    CHECK(out != nullptr);
                    REQUIRE(error != nullptr);

                    set_client_error(error);

                    return false;
                })
                .forever();

            auto const op = [&](std::function<void()> fn) {
                try {
                    fn();
                    FAIL("should not reach this point");
                } catch (v1::exception const& ex) {
                    CHECK(ex.code() == v1::source_errc::mongoc);
                    CHECK(ex.code().value() == static_cast<int>(v));
                    CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                }
            };

            SECTION("aggregate") {
                op([&] { (void)db.aggregate(session, v1::pipeline{}); });
            }

            SECTION("run_command") {
                op([&] { (void)db.run_command(session, bsoncxx::v1::document::view{}); });
            }

            SECTION("drop") {
                op([&] { (void)db.drop(session); });
            }

            SECTION("list_collections") {
                op([&] { (void)db.list_collections(session); });
            }

            SECTION("list_collection_names") {
                op([&] { (void)db.list_collection_names(session); });
            }

            SECTION("watch") {
                SECTION("with options") {
                    op([&] { (void)db.watch(session); });
                }

                SECTION("with pipeline") {
                    op([&] { (void)db.watch(session, v1::pipeline{}); });
                }
            }
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][database]") {
    identity_type client_identity;
    auto const client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    identity_type id1;
    identity_type id2;
    identity_type id3;

    auto const db1 = reinterpret_cast<mongoc_database_t*>(&id1);
    auto const db2 = reinterpret_cast<mongoc_database_t*>(&id2);
    auto const db3 = reinterpret_cast<mongoc_database_t*>(&id3);

    auto destroy = libmongoc::database_destroy.create_instance();
    auto copy = libmongoc::database_copy.create_instance();

    int destroy_count = 0;
    int copy_count = 0;

    destroy
        ->interpose([&](mongoc_database_t* ptr) -> void {
            if (ptr) {
                if (ptr != db1 && ptr != db2 && ptr != db3) {
                    FAIL("unexpected mongoc_database_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    copy->interpose([&](mongoc_database_t* ptr) -> mongoc_database_t* {
            if (ptr == db1) {
                ++copy_count;
                return db2;
            } else if (ptr == db2) {
                ++copy_count;
                return db3;
            } else {
                FAIL("unexpected mongoc_database_t");
                return nullptr;
            }
        })
        .forever();

    auto source = database::internal::make(db1, client_id);
    auto target = database::internal::make(db2, client_id);

    REQUIRE(database::internal::as_mongoc(source) == db1);
    REQUIRE(database::internal::as_mongoc(target) == db2);

    SECTION("move") {
        {
            auto move = std::move(source);

            CHECK_FALSE(source);
            REQUIRE(move);
            CHECK(database::internal::as_mongoc(move) == db1);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 0);

            target = std::move(move);

            CHECK_FALSE(move);
            REQUIRE(target);
            CHECK(database::internal::as_mongoc(target) == db1);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 0);
        }

        CHECK(destroy_count == 1);
    }

    SECTION("copy") {
        {
            auto copy = source;

            REQUIRE(source);
            REQUIRE(copy);
            CHECK(database::internal::as_mongoc(source) == db1);
            CHECK(database::internal::as_mongoc(copy) == db2);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 1);

            target = copy;

            REQUIRE(copy);
            REQUIRE(target);
            CHECK(database::internal::as_mongoc(copy) == db2);
            CHECK(database::internal::as_mongoc(target) == db3);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 2);
        }

        CHECK(destroy_count == 2);
    }
}

TEST_CASE("default", "[mongocxx][v1][database]") {
    database const db;

    CHECK_FALSE(db);
}

TEST_CASE("aggregate", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type cursor_identity;
    auto const cursor_id = reinterpret_cast<mongoc_cursor_t*>(&cursor_identity);
    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    cursor_destroy->interpose([&](mongoc_cursor_t* ptr) -> void { CHECK(ptr == cursor_id); });

    auto const pipeline = v1::pipeline{}.append_stage(scoped_bson{R"({"x": 1})"}.view());
    auto const pipeline_data = pipeline.view_array().data();

    auto aggregate = libmongoc::database_aggregate.create_instance();

    SECTION("no options") {
        aggregate->interpose(
            [&](mongoc_database_t* ptr,
                bson_t const* pipeline,
                bson_t const* opts,
                mongoc_read_prefs_t const* read_prefs) -> mongoc_cursor_t* {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{pipeline}.data() == pipeline_data);
                CHECK(scoped_bson_view{opts}.view().empty());
                CHECK(read_prefs == nullptr);

                return cursor_id;
            });

        auto const res = db.aggregate(pipeline);

        CHECK(v1::cursor::internal::as_mongoc(res) == cursor_id);
    }

    SECTION("with options") {
        auto const bypass = GENERATE(false, true);
        auto const staleness = GENERATE(as<std::chrono::seconds>(), 1, 2, 3);

        CAPTURE(bypass);
        CAPTURE(staleness);

        scoped_bson const expected_opts{BCON_NEW("bypassDocumentValidation", BCON_BOOL(bypass))};

        v1::aggregate_options opts;

        opts.bypass_document_validation(bypass);
        opts.read_preference(std::move(v1::read_preference{}.max_staleness(staleness)));

        aggregate->interpose(
            [&](mongoc_database_t* ptr,
                bson_t const* pipeline,
                bson_t const* opts,
                mongoc_read_prefs_t const* read_prefs) -> mongoc_cursor_t* {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{pipeline}.data() == pipeline_data);
                CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                REQUIRE(read_prefs != nullptr);

                CHECK(libmongoc::read_prefs_get_max_staleness_seconds(read_prefs) == staleness.count());

                return cursor_id;
            });

        auto const res = db.aggregate(pipeline, opts);

        CHECK(v1::cursor::internal::as_mongoc(res) == cursor_id);
    }

    SECTION("session") {
        session_mocks_type session_mocks;

        auto const session = session_mocks.make(mocks.client);

        scoped_bson const expected_opts{R"({"sessionId": 123})"};

        SECTION("no options") {
            aggregate->interpose(
                [&](mongoc_database_t* ptr,
                    bson_t const* pipeline,
                    bson_t const* opts,
                    mongoc_read_prefs_t const* read_prefs) -> mongoc_cursor_t* {
                    CHECK(ptr == mocks.database_id);
                    CHECK(scoped_bson_view{pipeline}.data() == pipeline_data);
                    CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                    CHECK(read_prefs == nullptr);

                    return cursor_id;
                });

            auto const res = db.aggregate(session, pipeline);

            CHECK(v1::cursor::internal::as_mongoc(res) == cursor_id);
        }

        SECTION("with options") {
            auto const bypass = GENERATE(false, true);
            auto const staleness = GENERATE(as<std::chrono::seconds>(), 1, 2, 3);

            CAPTURE(bypass);
            CAPTURE(staleness);

            scoped_bson const expected_opts{
                BCON_NEW("bypassDocumentValidation", BCON_BOOL(bypass), "sessionId", BCON_INT32(123))};

            v1::aggregate_options opts;

            opts.bypass_document_validation(bypass);
            opts.read_preference(std::move(v1::read_preference{}.max_staleness(staleness)));

            aggregate->interpose(
                [&](mongoc_database_t* ptr,
                    bson_t const* pipeline,
                    bson_t const* opts,
                    mongoc_read_prefs_t const* read_prefs) -> mongoc_cursor_t* {
                    CHECK(ptr == mocks.database_id);
                    CHECK(scoped_bson_view{pipeline}.data() == pipeline_data);
                    CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                    REQUIRE(read_prefs != nullptr);

                    CHECK(libmongoc::read_prefs_get_max_staleness_seconds(read_prefs) == staleness.count());

                    return cursor_id;
                });

            auto const res = db.aggregate(session, pipeline, opts);

            CHECK(v1::cursor::internal::as_mongoc(res) == cursor_id);
        }
    }
}

TEST_CASE("run_command", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    auto const command = GENERATE(as<scoped_bson>(), R"({"x": 1})");
    auto const output = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

    auto command_with_opts = libmongoc::database_command_with_opts.create_instance();
    auto client_command_simple_with_server_id = libmongoc::client_command_simple_with_server_id.create_instance();

    SECTION("basic") {
        command_with_opts->interpose(
            [&](mongoc_database_t* ptr,
                bson_t const* cmd,
                mongoc_read_prefs_t const* read_prefs,
                bson_t const* opts,
                bson_t* reply,
                bson_error_t* error) -> bool {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{cmd}.data() == command.data());
                CHECK(read_prefs == nullptr);
                CHECK(opts == nullptr);
                REQUIRE(reply != nullptr);
                CHECK(error != nullptr);

                bson_copy_to(output.bson(), reply);

                return true;
            });

        auto const res = db.run_command(command.view());

        CHECK(res == output.view());
    }

    SECTION("session") {
        session_mocks_type session_mocks;

        auto const session = session_mocks.make(mocks.client);

        scoped_bson const expected_opts{R"({"sessionId": 123})"};

        command_with_opts->interpose(
            [&](mongoc_database_t* ptr,
                bson_t const* cmd,
                mongoc_read_prefs_t const* read_prefs,
                bson_t const* opts,
                bson_t* reply,
                bson_error_t* error) -> bool {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{cmd}.data() == command.data());
                CHECK(read_prefs == nullptr);
                CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                REQUIRE(reply != nullptr);
                CHECK(error != nullptr);

                bson_copy_to(output.bson(), reply);

                return true;
            });

        auto const res = db.run_command(session, command.view());

        CHECK(res == output.view());
    }

    SECTION("server_id") {
        identity_type read_prefs_identity;
        auto const read_prefs_id = reinterpret_cast<mongoc_read_prefs_t const*>(&read_prefs_identity);
        auto get_read_prefs = libmongoc::database_get_read_prefs.create_instance();
        get_read_prefs->interpose([&](mongoc_database_t const* ptr) -> mongoc_read_prefs_t const* {
            CHECK(ptr == mocks.database_id);
            return read_prefs_id;
        });

        char const name = '\0';

        mocks.database_get_name->interpose([&](mongoc_database_t* ptr) -> char const* {
            CHECK(ptr == mocks.database_id);
            return &name;
        });

        auto const v = GENERATE(as<std::uint32_t>(), 0u, 1u, UINT32_MAX - 1u, UINT32_MAX);

        client_command_simple_with_server_id->interpose(
            [&](mongoc_client_t* ptr,
                char const* db_name,
                bson_t const* cmd,
                mongoc_read_prefs_t const* read_prefs,
                std::uint32_t server_id,
                bson_t* reply,
                bson_error_t* error) -> bool {
                CHECK(ptr == mocks.client_id);
                CHECK(static_cast<void const*>(db_name) == static_cast<void const*>(&name));
                CHECK(scoped_bson_view{cmd}.data() == command.data());
                CHECK(read_prefs == read_prefs_id);
                CHECK(server_id == v);
                REQUIRE(reply != nullptr);
                CHECK(error != nullptr);

                bson_copy_to(output.bson(), reply);

                return true;
            });

        auto const res = db.run_command(command.view(), v);

        CHECK(res == output.view());
    }
}

TEST_CASE("create_collection", "[mongocxx][v1][database]") {
    // TODO: v1::collection (CXX-3237)
}

TEST_CASE("drop", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    SECTION("no write_concern") {
        auto drop_with_opts = libmongoc::database_drop_with_opts.create_instance();
        drop_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.database_id);
            CHECK(opts == nullptr);
            CHECK(error != nullptr);

            return true;
        });

        CHECK_NOTHROW(db.drop());
    }

    SECTION("with write_concern") {
        auto const nodes = GENERATE(as<std::int32_t>(), 1, 2, 3);

        scoped_bson const expected_opts{BCON_NEW("writeConcern", "{", "w", BCON_INT32(nodes), "}")};

        auto drop_with_opts = libmongoc::database_drop_with_opts.create_instance();
        drop_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.database_id);
            CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
            CHECK(error != nullptr);

            return true;
        });

        CHECK_NOTHROW(db.drop(std::move(v1::write_concern{}.nodes(nodes))));
    }

    SECTION("session") {
        session_mocks_type session_mocks;

        auto const session = session_mocks.make(mocks.client);

        SECTION("no write_concern") {
            scoped_bson const expected_opts{R"({"sessionId": 123})"};

            auto drop_with_opts = libmongoc::database_drop_with_opts.create_instance();
            drop_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> bool {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                CHECK(error != nullptr);

                return true;
            });

            CHECK_NOTHROW(db.drop(session));
        }

        SECTION("with write_concern") {
            auto const nodes = GENERATE(as<std::int32_t>(), 1, 2, 3);

            scoped_bson const expected_opts{
                BCON_NEW("writeConcern", "{", "w", BCON_INT32(nodes), "}", "sessionId", BCON_INT32(123))};

            auto drop_with_opts = libmongoc::database_drop_with_opts.create_instance();
            drop_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> bool {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                CHECK(error != nullptr);

                return true;
            });

            CHECK_NOTHROW(db.drop(session, std::move(v1::write_concern{}.nodes(nodes))));
        }
    }
}

TEST_CASE("has_collection", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    auto const output = GENERATE(false, true);
    auto const input = GENERATE("a", "b", "c");

    auto has_collection = libmongoc::database_has_collection.create_instance();
    has_collection->interpose([&](mongoc_database_t* ptr, char const* name, bson_error_t* error) -> bool {
        CHECK(ptr == mocks.database_id);
        CHECK_THAT(name, Catch::Matchers::Equals(input));
        CHECK(error != nullptr);

        return output;
    });

    CHECK(db.has_collection(input) == output);
}

TEST_CASE("list_collections", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type cursor_identity;
    auto const cursor_id = reinterpret_cast<mongoc_cursor_t*>(&cursor_identity);
    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    cursor_destroy->interpose([&](mongoc_cursor_t* ptr) -> void { CHECK(ptr == cursor_id); });

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

    auto find_collections_with_opts = libmongoc::database_find_collections_with_opts.create_instance();

    SECTION("basic") {
        auto const expected_opts = scoped_bson{BCON_NEW("filter", BCON_DOCUMENT(filter.bson()))};

        find_collections_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts) -> mongoc_cursor_t* {
            CHECK(ptr == mocks.database_id);
            CHECK(scoped_bson_view{opts}.view() == expected_opts.view());

            return cursor_id;
        });

        auto const res = db.list_collections(filter.view());

        CHECK(v1::cursor::internal::as_mongoc(res) == cursor_id);
    }

    SECTION("session") {
        session_mocks_type session_mocks;

        auto const session = session_mocks.make(mocks.client);

        auto const expected_opts =
            scoped_bson{BCON_NEW("filter", BCON_DOCUMENT(filter.bson()), "sessionId", BCON_INT32(123))};

        find_collections_with_opts->interpose([&](mongoc_database_t* ptr, bson_t const* opts) -> mongoc_cursor_t* {
            CHECK(ptr == mocks.database_id);
            CHECK(scoped_bson_view{opts}.view() == expected_opts.view());

            return cursor_id;
        });

        auto const res = db.list_collections(session, filter.view());

        CHECK(v1::cursor::internal::as_mongoc(res) == cursor_id);
    }
}

TEST_CASE("list_collection_names", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type cursor_identity;
    auto const cursor_id = reinterpret_cast<mongoc_cursor_t*>(&cursor_identity);

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    auto get_collection_names_with_opts = libmongoc::database_get_collection_names_with_opts.create_instance();

    cursor_destroy->interpose([&](mongoc_cursor_t* ptr) -> void { CHECK(ptr == cursor_id); });

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

    auto const names = GENERATE(
        values<std::vector<std::string>>({
            {},
            {"x"},
            {"a", "b", "c"},
        }));

    struct names_deleter {
        void operator()(char** ptr) const noexcept {
            bson_strfreev(ptr);
        }
    };

    SECTION("basic") {
        scoped_bson const expected_opts{BCON_NEW("filter", BCON_DOCUMENT(filter.bson()))};

        get_collection_names_with_opts->interpose(
            [&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                CHECK(ptr == mocks.database_id);
                CHECK(scoped_bson_view{opts}.view() == expected_opts.view());
                CHECK(error != nullptr);

                if (names.empty()) {
                    return static_cast<char**>(bson_malloc0(sizeof(char*))); // Null terminator.
                }

                // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): bson compatibility.
                auto owner = std::unique_ptr<char*[], names_deleter>(
                    static_cast<char**>(bson_malloc0((names.size() + 1u) * sizeof(char*))));

                for (std::size_t idx = 0u; idx < names.size(); ++idx) {
                    auto const& name = names[idx];
                    owner[idx] = bson_strndup(name.c_str(), name.size());
                }

                return owner.release();
            });

        auto const res = db.list_collection_names(filter.view());

        CHECK(res == names);
    }

    SECTION("session") {
        session_mocks_type session_mocks;
        auto const session = session_mocks.make(mocks.client);

        scoped_bson const expected_opts{BCON_NEW("filter", BCON_DOCUMENT(filter.bson()), "sessionId", BCON_INT32(123))};

        get_collection_names_with_opts->interpose(
            [&](mongoc_database_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                CHECK(ptr == mocks.database_id);
                REQUIRE(opts != nullptr);
                CHECK(error != nullptr);

                CHECK(scoped_bson_view{opts}.view() == expected_opts.view());

                // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): bson compatibility.
                auto owner = std::unique_ptr<char*[], names_deleter>(
                    static_cast<char**>(bson_malloc0((names.size() + 1u) * sizeof(char*))));

                for (std::size_t idx = 0u; idx < names.size(); ++idx) {
                    auto const& name = names[idx];
                    owner[idx] = bson_strndup(name.c_str(), name.size());
                }

                return owner.release();
            });

        auto const result = db.list_collection_names(session, filter.view());

        CHECK(result == names);
    }
}

TEST_CASE("name", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    char const name = '\0';

    mocks.database_get_name->interpose([&](mongoc_database_t const* ptr) -> char const* {
        CHECK(ptr == mocks.database_id);
        return &name;
    });

    CHECK(static_cast<void const*>(db.name().data()) == static_cast<void const*>(&name));
}

TEST_CASE("read_concern", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type identity0;
    identity_type identity1;

    auto const id0 = reinterpret_cast<mongoc_read_concern_t const*>(&identity0);
    auto const id1 = reinterpret_cast<mongoc_read_concern_t*>(&identity1);

    auto destroy = libmongoc::read_concern_destroy.create_instance();
    auto copy = libmongoc::read_concern_copy.create_instance();
    auto set = libmongoc::database_set_read_concern.create_instance();
    auto get = libmongoc::database_get_read_concern.create_instance();

    destroy
        ->interpose([&](mongoc_read_concern_t* ptr) -> void {
            if (ptr != id0 && ptr != id1) {
                FAIL_CHECK("unexpected mongoc_read_concern_t");
            }
        })
        .forever();

    copy->interpose([&](mongoc_read_concern_t const* ptr) -> mongoc_read_concern_t* {
        CHECK(ptr == id0);
        return id1;
    });

    SECTION("get") {
        get->interpose([&](mongoc_database_t const* ptr) -> mongoc_read_concern_t const* {
            CHECK(ptr == mocks.database_id);
            return id0;
        });

        auto const res = db.read_concern();

        CHECK(v1::read_concern::internal::as_mongoc(res) == id1);
    }

    SECTION("set") {
        set->interpose([&](mongoc_database_t* ptr, mongoc_read_concern_t const* v) -> void {
            CHECK(ptr == mocks.database_id);
            CHECK(v == id1);
        });

        db.read_concern(v1::read_concern::internal::make(id1));
    }
}

TEST_CASE("read_preference", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type identity0;
    identity_type identity1;

    auto const id0 = reinterpret_cast<mongoc_read_prefs_t const*>(&identity0);
    auto const id1 = reinterpret_cast<mongoc_read_prefs_t*>(&identity1);

    auto destroy = libmongoc::read_prefs_destroy.create_instance();
    auto copy = libmongoc::read_prefs_copy.create_instance();
    auto set = libmongoc::database_set_read_prefs.create_instance();
    auto get = libmongoc::database_get_read_prefs.create_instance();

    destroy
        ->interpose([&](mongoc_read_prefs_t* ptr) -> void {
            if (ptr != id0 && ptr != id1) {
                FAIL_CHECK("unexpected mongoc_read_prefs_t");
            }
        })
        .forever();

    copy->interpose([&](mongoc_read_prefs_t const* ptr) -> mongoc_read_prefs_t* {
        CHECK(ptr == id0);
        return id1;
    });

    SECTION("get") {
        get->interpose([&](mongoc_database_t const* ptr) -> mongoc_read_prefs_t const* {
            CHECK(ptr == mocks.database_id);
            return id0;
        });

        auto const res = db.read_preference();

        CHECK(v1::read_preference::internal::as_mongoc(res) == id1);
    }

    SECTION("set") {
        set->interpose([&](mongoc_database_t* ptr, mongoc_read_prefs_t const* v) -> void {
            CHECK(ptr == mocks.database_id);
            CHECK(v == id1);
        });

        db.read_preference(v1::read_preference::internal::make(id1));
    }
}

TEST_CASE("write_concern", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type identity0;
    identity_type identity1;

    auto const id0 = reinterpret_cast<mongoc_write_concern_t const*>(&identity0);
    auto const id1 = reinterpret_cast<mongoc_write_concern_t*>(&identity1);

    auto destroy = libmongoc::write_concern_destroy.create_instance();
    auto copy = libmongoc::write_concern_copy.create_instance();
    auto set = libmongoc::database_set_write_concern.create_instance();
    auto get = libmongoc::database_get_write_concern.create_instance();

    destroy
        ->interpose([&](mongoc_write_concern_t* ptr) -> void {
            if (ptr != id0 && ptr != id1) {
                FAIL_CHECK("unexpected mongoc_write_concern_t");
            }
        })
        .forever();

    copy->interpose([&](mongoc_write_concern_t const* ptr) -> mongoc_write_concern_t* {
        CHECK(ptr == id0);
        return id1;
    });

    SECTION("get") {
        get->interpose([&](mongoc_database_t const* ptr) -> mongoc_write_concern_t const* {
            CHECK(ptr == mocks.database_id);
            return id0;
        });

        auto const res = db.write_concern();

        CHECK(v1::write_concern::internal::as_mongoc(res) == id1);
    }

    SECTION("set") {
        set->interpose([&](mongoc_database_t* ptr, mongoc_write_concern_t const* v) -> void {
            CHECK(ptr == mocks.database_id);
            CHECK(v == id1);
        });

        db.write_concern(v1::write_concern::internal::make(id1));
    }
}

TEST_CASE("collection", "[mongocxx][v1][database]") {
    // TODO: v1::collection (CXX-3237)
}

TEST_CASE("gridfs_bucket", "[mongocxx][v1][database]") {
    // TODO: v1::gridfs (CXX-3237)
}

TEST_CASE("watch", "[mongocxx][v1][database]") {
    database_mocks_type mocks;

    auto db = mocks.make();

    identity_type stream_identity;
    auto const stream_id = reinterpret_cast<mongoc_change_stream_t*>(&stream_identity);

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    auto watch = libmongoc::database_watch.create_instance();

    change_stream_destroy
        ->interpose([&](mongoc_change_stream_t* ptr) {
            if (ptr) {
                CHECK(ptr == stream_id);
            }
        })
        .forever();

    SECTION("with pipeline") {
        v1::pipeline pipeline;
        pipeline.append_stage(scoped_bson{R"({"x": 1})"}.value());
        scoped_bson const expected_pipeline{R"([{"x": 1}])"};

        int count = 0;

        SECTION("no options") {
            watch->interpose(
                [&](mongoc_database_t const* ptr,
                    bson_t const* pipeline,
                    bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.database_id);
                    CHECK(scoped_bson_view{pipeline}.view() == expected_pipeline.view());
                    REQUIRE(opts != nullptr);

                    CHECK(scoped_bson_view{opts}.view().empty());

                    ++count;

                    return stream_id;
                });

            auto database = mocks.make();
            auto const stream = database.watch(pipeline);
            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
        }

        SECTION("with options") {
            watch->interpose(
                [&](mongoc_database_t const* ptr,
                    bson_t const* pipeline,
                    bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.database_id);
                    CHECK(scoped_bson_view{pipeline}.view() == expected_pipeline.view());
                    REQUIRE(opts != nullptr);

                    CHECK(scoped_bson_view{opts}.view().empty());

                    ++count;

                    return stream_id;
                });

            auto database = mocks.make();
            auto const stream = database.watch(pipeline, v1::change_stream::options{});
            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
        }

        CHECK(count == 1);
    }

    SECTION("with session") {
        auto database = mocks.make();

        session_mocks_type session_mocks;
        auto const session = session_mocks.make(mocks.client);

        int count = 0;

        auto const op = [&](v1::pipeline const* pipeline_ptr, v1::change_stream::options const* opts_ptr) -> void {
            auto const stream = pipeline_ptr
                                    ? (opts_ptr ? database.watch(session, *pipeline_ptr, *opts_ptr)
                                                : database.watch(session, *pipeline_ptr))
                                    : (opts_ptr ? database.watch(session, *opts_ptr) : database.watch(session));

            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
            CHECK(count == 1);
        };

        SECTION("no options") {
            watch->interpose(
                [&](mongoc_database_t const* ptr,
                    bson_t const* pipeline,
                    bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.database_id);
                    REQUIRE(pipeline != nullptr);
                    REQUIRE(opts != nullptr);

                    CHECK(scoped_bson_view{pipeline}.view() == bsoncxx::v1::array::view{});
                    CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                    ++count;

                    return stream_id;
                });

            op(nullptr, nullptr);
        }

        SECTION("with options") {
            watch->interpose(
                [&](mongoc_database_t const* ptr,
                    bson_t const* pipeline,
                    bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.database_id);
                    REQUIRE(pipeline != nullptr);
                    REQUIRE(opts != nullptr);

                    CHECK(scoped_bson_view{pipeline}.view() == bsoncxx::v1::array::view{});
                    CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                    ++count;

                    return stream_id;
                });

            v1::change_stream::options const opts;
            op(nullptr, &opts);
        }

        SECTION("with pipeline") {
            v1::pipeline pipeline;
            pipeline.append_stage(scoped_bson{R"({"x": 1})"}.value());
            scoped_bson const expected_pipeline{R"([{"x": 1}])"};

            SECTION("no options") {
                watch->interpose(
                    [&](mongoc_database_t const* ptr,
                        bson_t const* pipeline,
                        bson_t const* opts) -> mongoc_change_stream_t* {
                        CHECK(ptr == mocks.database_id);
                        REQUIRE(pipeline != nullptr);
                        REQUIRE(opts != nullptr);

                        CHECK(scoped_bson_view{pipeline}.view() == expected_pipeline.view());
                        CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                        ++count;

                        return stream_id;
                    });

                op(&pipeline, nullptr);
            }

            SECTION("with options") {
                watch->interpose(
                    [&](mongoc_database_t const* ptr,
                        bson_t const* pipeline,
                        bson_t const* opts) -> mongoc_change_stream_t* {
                        CHECK(ptr == mocks.database_id);
                        REQUIRE(pipeline != nullptr);
                        REQUIRE(opts != nullptr);

                        CHECK(scoped_bson_view{pipeline}.view() == expected_pipeline.view());
                        CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                        ++count;

                        return stream_id;
                    });

                v1::change_stream::options const opts;
                op(&pipeline, &opts);
            }
        }
    }
}

} // namespace v1
} // namespace mongocxx
