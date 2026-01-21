// Copyright 2009-present MongoDB, Inc.
//
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

#include <mongocxx/v1/client_session.hh>

//

#include <mongocxx/v1/client.hpp>
#include <mongocxx/v1/read_concern.hpp>    // IWYU pragma: keep: v1::transaction_options
#include <mongocxx/v1/read_preference.hpp> // IWYU pragma: keep: v1::transaction_options
#include <mongocxx/v1/write_concern.hpp>   // IWYU pragma: keep: v1::transaction_options

#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/server_error.hh>
#include <mongocxx/v1/transaction_options.hh>

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

struct session_mocks_type {
    using client_destroy_type = decltype(libmongoc::client_destroy.create_instance());
    using destroy_type = decltype(libmongoc::client_session_destroy.create_instance());

    identity_type client_identity;
    identity_type session_identity;

    mongoc_client_t* client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);
    mongoc_client_session_t* session_id = reinterpret_cast<mongoc_client_session_t*>(&session_identity);

    client_destroy_type client_destroy = libmongoc::client_destroy.create_instance();
    destroy_type destroy = libmongoc::client_session_destroy.create_instance();

    v1::client client = v1::client::internal::make(client_id);

    ~session_mocks_type() = default;
    session_mocks_type(session_mocks_type&& other) noexcept = delete;
    session_mocks_type& operator=(session_mocks_type&& other) noexcept = delete;
    session_mocks_type(session_mocks_type const& other) = delete;
    session_mocks_type& operator=(session_mocks_type const& other) = delete;

    session_mocks_type() {
        client_destroy->interpose([&](mongoc_client_t* ptr) -> void { CHECK(ptr == client_id); }).forever();
        destroy->interpose([&](mongoc_client_session_t* ptr) -> void { CHECK(ptr == session_id); }).forever();
    }

    client_session make() {
        return v1::client_session::internal::make(session_id, client);
    }
};

} // namespace

TEST_CASE("exceptions", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

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

    SECTION("start_transaction") {
        identity_type txn_identity;
        auto const txn_id = reinterpret_cast<mongoc_transaction_opt_t*>(&txn_identity);

        auto txn_destroy = libmongoc::transaction_opts_destroy.create_instance();
        txn_destroy->interpose([&](mongoc_transaction_opt_t* ptr) -> void { CHECK(ptr == txn_id); }).forever();
        auto const txn_opts = v1::transaction_options::internal::make(txn_id);

        auto const op = [&](mongoc_transaction_opt_t* txn_opts_ptr) {
            auto start_transaction = libmongoc::client_session_start_transaction.create_instance();
            start_transaction->interpose(
                [&](mongoc_client_session_t* ptr, mongoc_transaction_opt_t const* opts, bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.session_id);
                    CHECK(opts == txn_opts_ptr);
                    CHECK(error != nullptr);

                    set_client_error(error);

                    return false;
                });

            try {
                txn_opts_ptr ? session.start_transaction(txn_opts) : session.start_transaction();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        };

        SECTION("no options") {
            op(nullptr);
        }

        SECTION("with options") {
            op(txn_id);
        }
    }

    SECTION("commit_transaction") {
        SECTION("client") {
            auto commit_transaction = libmongoc::client_session_commit_transaction.create_instance();
            commit_transaction->interpose(
                [&](mongoc_client_session_t* ptr, bson_t* reply, bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.session_id);
                    REQUIRE(reply != nullptr);
                    REQUIRE(error != nullptr);

                    set_client_error(error);
                    *reply = BSON_INITIALIZER;

                    return false;
                });

            try {
                session.commit_transaction();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("server") {
            scoped_bson const raw{BCON_NEW("code", BCON_INT32(static_cast<std::int32_t>(v)))};

            auto commit_transaction = libmongoc::client_session_commit_transaction.create_instance();
            commit_transaction->interpose(
                [&](mongoc_client_session_t* ptr, bson_t* reply, bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.session_id);
                    REQUIRE(reply != nullptr);
                    REQUIRE(error != nullptr);

                    set_server_error(error);
                    bson_copy_to(raw.bson(), reply);

                    return false;
                });

            try {
                session.commit_transaction();
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

    SECTION("abort_transaction") {
        auto abort_transaction = libmongoc::client_session_abort_transaction.create_instance();
        abort_transaction->interpose([&](mongoc_client_session_t* ptr, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.session_id);
            REQUIRE(error != nullptr);

            set_client_error(error);

            return false;
        });

        try {
            session.abort_transaction();
            FAIL("should not reach this point");
        } catch (v1::exception const& ex) {
            CHECK(ex.code() == v1::source_errc::mongoc);
            CHECK(ex.code().value() == v);
            CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
        }
    }

    SECTION("with_transaction") {
        auto with = libmongoc::client_session_with_transaction.create_instance();

        SECTION("no exception") {
            std::string what_arg;
            what_arg += "{v: ";
            what_arg += std::to_string(v);
            what_arg += ", msg: ";
            what_arg += msg;
            what_arg += "}";
            CAPTURE(what_arg);

            auto const fn = [&](v1::client_session& s) -> void {
                CHECK(&s == &session);
                FAIL_CHECK("should not reach this point");
            };

            SECTION("client") {
                scoped_bson const raw = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");
                CAPTURE(raw.view());

                with->interpose(
                    [&](mongoc_client_session_t* ptr,
                        mongoc_client_session_with_transaction_cb_t cb,
                        mongoc_transaction_opt_t const* opts,
                        void* ctx,
                        bson_t* reply,
                        bson_error_t* error) -> bool {
                        CHECK(ptr == mocks.session_id);
                        REQUIRE(cb != nullptr);
                        CHECK(opts == nullptr);
                        CHECK(ctx != nullptr);
                        REQUIRE(reply != nullptr);
                        REQUIRE(error != nullptr);

                        set_client_error(error);

                        if (!raw.view().empty()) {
                            bson_copy_to(raw.bson(), reply);
                        }

                        return false;
                    });

                try {
                    session.with_transaction(std::ref(fn));
                    FAIL("should not reach this point");
                } catch (v1::exception const& ex) {
                    CHECK(ex.code() == v1::source_errc::mongoc);
                    CHECK(ex.code().value() == v);
                    CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                    CHECK(dynamic_cast<v1::server_error const*>(&ex) == nullptr);
                }
            }

            SECTION("server") {
                scoped_bson const raw = GENERATE(as<scoped_bson>(), R"({})", R"({"code": 123})");
                CAPTURE(raw.view());

                with->interpose(
                    [&](mongoc_client_session_t* ptr,
                        mongoc_client_session_with_transaction_cb_t cb,
                        mongoc_transaction_opt_t const* opts,
                        void* ctx,
                        bson_t* reply,
                        bson_error_t* error) -> bool {
                        CHECK(ptr == mocks.session_id);
                        REQUIRE(cb != nullptr);
                        CHECK(opts == nullptr);
                        CHECK(ctx != nullptr);
                        REQUIRE(reply != nullptr);
                        REQUIRE(error != nullptr);

                        set_server_error(error);

                        if (!raw.view().empty()) {
                            bson_copy_to(raw.bson(), reply);
                        }

                        return false;
                    });

                try {
                    session.with_transaction(std::ref(fn));
                    FAIL("should not reach this point");
                } catch (v1::exception const& ex) {
                    CHECK(ex.code() == v1::source_errc::server);
                    CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));

                    CHECKED_IF(raw.view().empty()) {
                        CHECK(ex.code().value() == v);
                        CHECK_FALSE(v1::exception::internal::get_reply(ex).has_value());
                        CHECK(dynamic_cast<v1::server_error const*>(&ex) == nullptr);
                    }
                    else {
                        CHECK(ex.code().value() == 123);
                        auto const ptr = dynamic_cast<v1::server_error const*>(&ex);
                        REQUIRE(ptr);
                        CHECK_FALSE(v1::exception::internal::get_reply(ex).has_value());
                        CHECK(ptr->raw() == raw.view());
                        CHECK(ptr->client_code().value() == v);
                    }
                }
            }
        }

        SECTION("client") {
            scoped_bson const raw = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");
            CAPTURE(raw.view());

            auto const fn = [&](v1::client_session& s) -> void {
                CHECK(&s == &session);

                bson_error_t error = {};
                set_client_error(&error);

                if (raw.view().empty()) {
                    v1::throw_exception(error);
                }

                v1::throw_exception(error, raw.value());
            };

            with->interpose(
                [&](mongoc_client_session_t* ptr,
                    mongoc_client_session_with_transaction_cb_t cb,
                    mongoc_transaction_opt_t const* opts,
                    void* ctx,
                    bson_t* reply,
                    bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.session_id);
                    REQUIRE(cb != nullptr);
                    CHECK(opts == nullptr);
                    CHECK(ctx != nullptr);
                    REQUIRE(reply != nullptr);
                    REQUIRE(error != nullptr);

                    CHECK(error->code == 0);

                    bson_t* bson = nullptr;
                    CHECK_FALSE(cb(ptr, ctx, &bson, error));
                    CHECKED_IF(raw.view().empty()) {
                        CHECK(bson == nullptr);
                    }
                    else {
                        REQUIRE(bson != nullptr);
                        CHECK(bson_steal(reply, bson));
                    }
                    CHECK(error->code == static_cast<std::uint32_t>(v));

                    return false;
                });

            try {
                session.with_transaction(std::ref(fn));
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));

                CHECKED_IF(raw.view().empty()) {
                    CHECK_FALSE(v1::exception::internal::get_reply(ex).has_value());
                }
                else {
                    CHECK(v1::exception::internal::get_reply(ex) == raw.view());
                }

                CHECK(dynamic_cast<v1::server_error const*>(&ex) == nullptr);
            }
        }

        SECTION("server") {
            scoped_bson const raw = GENERATE(as<scoped_bson>(), R"({})", R"({"code": 123})");
            CAPTURE(raw.view());

            auto const fn = [&](v1::client_session& s) -> void {
                CHECK(&s == &session);

                bson_error_t error = {};
                set_server_error(&error);

                if (raw.view().empty()) {
                    v1::throw_exception(error);
                }

                v1::throw_exception(error, raw.value());
            };

            with->interpose(
                [&](mongoc_client_session_t* ptr,
                    mongoc_client_session_with_transaction_cb_t cb,
                    mongoc_transaction_opt_t const* opts,
                    void* ctx,
                    bson_t* reply,
                    bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.session_id);
                    REQUIRE(cb != nullptr);
                    CHECK(opts == nullptr);
                    CHECK(ctx != nullptr);
                    REQUIRE(reply != nullptr);
                    REQUIRE(error != nullptr);

                    CHECK(error->code == 0);

                    bson_t* bson = nullptr;
                    CHECK_FALSE(cb(ptr, ctx, &bson, error));
                    CHECKED_IF(raw.view().empty()) {
                        CHECK(bson == nullptr);
                        CHECK(error->code == static_cast<std::uint32_t>(v));
                    }
                    else {
                        REQUIRE(bson != nullptr);
                        CHECK(bson_steal(reply, bson));
                        CHECK(error->code == 123);
                    }

                    return false;
                });

            try {
                session.with_transaction(std::ref(fn));
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::server);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));

                CHECKED_IF(raw.view().empty()) {
                    CHECK(ex.code().value() == v);
                    CHECK_FALSE(v1::exception::internal::get_reply(ex).has_value());
                    CHECK(dynamic_cast<v1::server_error const*>(&ex) == nullptr);
                }
                else {
                    CHECK(ex.code().value() == 123);
                    auto const ptr = dynamic_cast<v1::server_error const*>(&ex);
                    REQUIRE(ptr);
                    CHECK_FALSE(v1::exception::internal::get_reply(ex).has_value());
                    CHECK(ptr->raw() == raw.view());
                    CHECK(ptr->client_code().value() == v);
                }
            }
        }

        SECTION("unknown") {
            std::string what_arg;
            what_arg += "{v: ";
            what_arg += std::to_string(v);
            what_arg += ", msg: ";
            what_arg += msg;
            what_arg += "}";
            CAPTURE(what_arg);

            auto const fn = [&](v1::client_session& s) -> void {
                CHECK(&s == &session);
                throw std::runtime_error{what_arg};
            };

            with->interpose(
                [&](mongoc_client_session_t* ptr,
                    mongoc_client_session_with_transaction_cb_t cb,
                    mongoc_transaction_opt_t const* opts,
                    void* ctx,
                    bson_t* reply,
                    bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.session_id);
                    REQUIRE(cb != nullptr);
                    CHECK(opts == nullptr);
                    CHECK(ctx != nullptr);
                    REQUIRE(reply != nullptr);
                    REQUIRE(error != nullptr);

                    CHECK(error->code == 0);

                    bson_t* bson = nullptr;
                    CHECK_FALSE(cb(ptr, ctx, &bson, error));
                    CHECK(bson == nullptr);
                    CHECK(error->code == 0);

                    return false;
                });

            CHECK_THROWS_MATCHES(
                session.with_transaction(std::ref(fn)), std::runtime_error, Catch::Matchers::Message(what_arg));
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][client_session]") {
    identity_type id1;
    identity_type id2;

    auto const session1 = reinterpret_cast<mongoc_client_session_t*>(&id1);
    auto const session2 = reinterpret_cast<mongoc_client_session_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::client_session_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_client_session_t* ptr) -> void {
            if (ptr != session1 && ptr != session2) {
                FAIL_CHECK("unexpected mongoc_client_t");
            }
            ++destroy_count;
        })
        .forever();

    v1::client client;

    auto source = client_session::internal::make(session1, client);
    auto target = client_session::internal::make(session2, client);

    REQUIRE(client_session::internal::as_mongoc(source) == session1);
    REQUIRE(client_session::internal::as_mongoc(target) == session2);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.

            CHECK(client_session::internal::as_mongoc(move) == session1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.

            CHECK(client_session::internal::as_mongoc(target) == session1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][client_session][options]") {
    identity_type id1;
    identity_type id2;
    identity_type id3;

    auto const opts1 = reinterpret_cast<mongoc_session_opt_t*>(&id1);
    auto const opts2 = reinterpret_cast<mongoc_session_opt_t*>(&id2);
    auto const opts3 = reinterpret_cast<mongoc_session_opt_t*>(&id3);

    int destroy_count = 0;

    auto destroy = libmongoc::session_opts_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_session_opt_t* ptr) -> void {
            if (ptr) {
                if (ptr != opts1 && ptr != opts2 && ptr != opts3) {
                    FAIL("unexpected mongoc_session_opt_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    auto source = client_session::options::internal::make(opts1);
    auto target = client_session::options::internal::make(opts2);

    REQUIRE(client_session::options::internal::as_mongoc(source) == opts1);
    REQUIRE(client_session::options::internal::as_mongoc(target) == opts2);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.

            CHECK(client_session::options::internal::as_mongoc(move) == opts1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.

            CHECK(client_session::options::internal::as_mongoc(target) == opts1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }

    SECTION("copy") {
        int clone_count = 0;

        auto clone = libmongoc::session_opts_clone.create_instance();
        clone
            ->interpose([&](mongoc_session_opt_t const* ptr) -> mongoc_session_opt_t* {
                CHECKED_IF(ptr == opts1) {
                    ++clone_count;
                    return opts2;
                }

                CHECKED_IF(ptr == opts2) {
                    ++clone_count;
                    return opts3;
                }

                FAIL("unexpected mongoc_session_opt_t");
                return nullptr;
            })
            .forever();

        {
            auto copy = source;

            CHECK(client_session::options::internal::as_mongoc(source) == opts1);
            CHECK(client_session::options::internal::as_mongoc(copy) == opts2);
            CHECK(destroy_count == 0);
            CHECK(clone_count == 1);

            target = copy;

            CHECK(client_session::options::internal::as_mongoc(copy) == opts2);
            CHECK(client_session::options::internal::as_mongoc(target) == opts3);
            CHECK(destroy_count == 1);
            CHECK(clone_count == 2);
        }

        CHECK(destroy_count == 2);
    }
}

TEST_CASE("client", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto const session = mocks.make();

    CHECK(&session.client() == &mocks.client);
}

TEST_CASE("opts", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    identity_type session_identity;
    auto const opts_id = reinterpret_cast<mongoc_session_opt_t*>(&session_identity);

    auto opts_destroy = libmongoc::session_opts_destroy.create_instance();
    auto get = libmongoc::client_session_get_opts.create_instance();
    auto clone = libmongoc::session_opts_clone.create_instance();

    int destroy_counter = 0;
    opts_destroy->interpose([&](mongoc_session_opt_t* ptr) -> void {
        CHECK(ptr == opts_id);
        ++destroy_counter;
    });

    int get_counter = 0;
    get->interpose([&](mongoc_client_session_t const* ptr) -> mongoc_session_opt_t const* {
        CHECK(ptr == mocks.session_id);
        ++get_counter;
        return opts_id;
    });

    int clone_counter = 0;
    clone->interpose([&](mongoc_session_opt_t const* ptr) -> mongoc_session_opt_t* {
        CHECK(ptr == opts_id);
        ++clone_counter;
        return opts_id;
    });

    CHECK(v1::client_session::options::internal::as_mongoc(session.opts()) == opts_id);
    CHECK(destroy_counter == 1);
    CHECK(get_counter == 1);
    CHECK(clone_counter == 1);
}

TEST_CASE("id", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    scoped_bson const doc{R"({"x": 1})"};

    auto get = libmongoc::client_session_get_lsid.create_instance();
    get->interpose([&](mongoc_client_session_t const* ptr) -> bson_t const* {
        CHECK(ptr == mocks.session_id);
        return doc.bson();
    });

    CHECK(session.id().data() == doc.data());
}

TEST_CASE("cluster_time", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    auto get = libmongoc::client_session_get_cluster_time.create_instance();

    SECTION("null") {
        get->interpose([&](mongoc_client_session_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.session_id);
            return nullptr;
        });

        CHECK(session.cluster_time().data() == bsoncxx::v1::document::view{}.data());
    }

    SECTION("value") {
        scoped_bson const doc{R"({"x": 1})"};

        get->interpose([&](mongoc_client_session_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.session_id);
            return doc.bson();
        });

        CHECK(session.cluster_time().data() == doc.data());
    }
}

TEST_CASE("operation_time", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    auto const timestamp = GENERATE(as<std::uint32_t>(), 0u, 1u, UINT32_MAX - 1u, UINT32_MAX);
    auto const increment = GENERATE(as<std::uint32_t>(), 0u, 1u, UINT32_MAX - 1u, UINT32_MAX);

    auto get = libmongoc::client_session_get_operation_time.create_instance();
    get->interpose([&](mongoc_client_session_t const* ptr, std::uint32_t* tp, std::uint32_t* ip) -> void {
        CHECK(ptr == mocks.session_id);
        REQUIRE(tp != nullptr);
        REQUIRE(ip != nullptr);
        *tp = timestamp;
        *ip = increment;
    });

    CHECK(session.operation_time() == bsoncxx::v1::types::b_timestamp{increment, timestamp});
}

TEST_CASE("server_id", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    auto const v = GENERATE(as<std::uint32_t>(), 0u, 1u, UINT32_MAX - 1u, UINT32_MAX);

    auto get = libmongoc::client_session_get_server_id.create_instance();
    get->interpose([&](mongoc_client_session_t const* ptr) -> std::uint32_t {
        CHECK(ptr == mocks.session_id);
        return v;
    });

    CHECK(session.server_id() == v);
}

TEST_CASE("get_transaction_state", "[mongocxx][v1][client_session]") {
    using state = client_session::transaction_state;

    session_mocks_type mocks;

    auto session = mocks.make();

    mongoc_transaction_state_t input = {};
    client_session::transaction_state expected = {};
    std::tie(input, expected) = GENERATE(
        table<mongoc_transaction_state_t, client_session::transaction_state>({
            {MONGOC_TRANSACTION_NONE, state::k_transaction_none},
            {MONGOC_TRANSACTION_STARTING, state::k_transaction_starting},
            {MONGOC_TRANSACTION_IN_PROGRESS, state::k_transaction_in_progress},
            {MONGOC_TRANSACTION_COMMITTED, state::k_transaction_committed},
            {MONGOC_TRANSACTION_ABORTED, state::k_transaction_aborted},
        }));

    auto get = libmongoc::client_session_get_transaction_state.create_instance();
    get->interpose([&](mongoc_client_session_t const* ptr) -> mongoc_transaction_state_t {
        CHECK(ptr == mocks.session_id);
        return input;
    });

    CHECK(session.get_transaction_state() == expected);
}

TEST_CASE("get_dirty", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    auto const v = GENERATE(false, true);

    auto get = libmongoc::client_session_get_dirty.create_instance();
    get->interpose([&](mongoc_client_session_t const* ptr) -> std::uint32_t {
        CHECK(ptr == mocks.session_id);
        return v;
    });

    CHECK(session.get_dirty() == v);
}

TEST_CASE("advance_cluster_time", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    auto advance = libmongoc::client_session_advance_cluster_time.create_instance();

    scoped_bson const doc{R"({"x": 1})"};
    int advance_count = 0;
    advance->interpose([&](mongoc_client_session_t* ptr, bson_t const* cluster_time) -> void {
        CHECK(ptr == mocks.session_id);
        REQUIRE(cluster_time != nullptr);

        CHECK(bson_get_data(cluster_time) == doc.data());

        ++advance_count;
    });

    session.advance_cluster_time(doc.view());

    CHECK(advance_count == 1);
}

TEST_CASE("advance_operation_time", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    auto advance = libmongoc::client_session_advance_operation_time.create_instance();

    bsoncxx::v1::types::b_timestamp const input{
        GENERATE(as<std::uint32_t>(), 0u, 1u, UINT32_MAX - 1u, UINT32_MAX),
        GENERATE(as<std::uint32_t>(), 0u, 1u, UINT32_MAX - 1u, UINT32_MAX),
    };

    int advance_count = 0;
    advance->interpose([&](mongoc_client_session_t* ptr, std::uint32_t timestamp, std::uint32_t increment) -> void {
        CHECK(ptr == mocks.session_id);
        CHECK(timestamp == input.timestamp);
        CHECK(increment == input.increment);

        ++advance_count;
    });

    session.advance_operation_time(input);

    CHECK(advance_count == 1);
}

TEST_CASE("start_transaction", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    SECTION("no options") {
        auto start_transaction = libmongoc::client_session_start_transaction.create_instance();
        int start_counter = 0;
        start_transaction->interpose(
            [&](mongoc_client_session_t* ptr, mongoc_transaction_opt_t const* opts, bson_error_t* error) -> bool {
                CHECK(ptr == mocks.session_id);
                CHECK(opts == nullptr);
                CHECK(error != nullptr);

                ++start_counter;

                return true;
            });

        session.start_transaction();

        CHECK(start_counter == 1);
    }

    SECTION("with options") {
        identity_type txn_identity;
        auto const txn_id = reinterpret_cast<mongoc_transaction_opt_t*>(&txn_identity);

        auto txn_destroy = libmongoc::transaction_opts_destroy.create_instance();
        txn_destroy->interpose([&](mongoc_transaction_opt_t* ptr) -> void { CHECK(ptr == txn_id); }).forever();
        auto const txn_opts = v1::transaction_options::internal::make(txn_id);

        auto start_transaction = libmongoc::client_session_start_transaction.create_instance();
        int start_counter = 0;
        start_transaction->interpose(
            [&](mongoc_client_session_t* ptr, mongoc_transaction_opt_t const* opts, bson_error_t* error) -> bool {
                CHECK(ptr == mocks.session_id);
                CHECK(opts == txn_id);
                CHECK(error != nullptr);

                ++start_counter;

                return true;
            });

        session.start_transaction(txn_opts);

        CHECK(start_counter == 1);
    }
}

TEST_CASE("commit_transaction", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    int commit_counter = 0;
    auto commit_transaction = libmongoc::client_session_commit_transaction.create_instance();
    commit_transaction->interpose([&](mongoc_client_session_t* ptr, bson_t* reply, bson_error_t* error) -> bool {
        CHECK(ptr == mocks.session_id);
        REQUIRE(reply != nullptr);
        CHECK(error != nullptr);

        *reply = BSON_INITIALIZER;

        ++commit_counter;

        return true;
    });

    session.commit_transaction();

    CHECK(commit_counter == 1);
}

TEST_CASE("abort_transaction", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    int abort_counter = 0;
    auto abort_transaction = libmongoc::client_session_abort_transaction.create_instance();
    abort_transaction->interpose([&](mongoc_client_session_t* ptr, bson_error_t* error) -> bool {
        CHECK(ptr == mocks.session_id);
        CHECK(error != nullptr);

        ++abort_counter;

        return true;
    });

    session.abort_transaction();

    CHECK(abort_counter == 1);
}

TEST_CASE("with_transaction", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;

    auto session = mocks.make();

    int fn_counter = 0;
    auto const fn = [&](v1::client_session& s) -> void {
        CHECK(&s == &session);
        ++fn_counter;
    };

    auto with = libmongoc::client_session_with_transaction.create_instance();

    SECTION("no options") {
        int with_counter = 0;
        with->interpose(
            [&](mongoc_client_session_t* ptr,
                mongoc_client_session_with_transaction_cb_t cb,
                mongoc_transaction_opt_t const* opts,
                void* ctx,
                bson_t* reply,
                bson_error_t* error) -> bool {
                CHECK(ptr == mocks.session_id);
                REQUIRE(cb != nullptr);
                CHECK(opts == nullptr);
                CHECK(ctx != nullptr);
                REQUIRE(reply != nullptr);
                REQUIRE(error != nullptr);

                *reply = BSON_INITIALIZER;
                CHECK(error->code == 0);

                bson_t* bson = nullptr;
                CHECK(cb(ptr, ctx, &bson, error));
                CHECK(bson == nullptr);
                CHECK(error->code == 0);

                ++with_counter;

                return true;
            });

        session.with_transaction(std::ref(fn));

        CHECK(with_counter == 1);
        CHECK(fn_counter == 1);
    }

    SECTION("with options") {
        identity_type txn_identity;
        auto const txn_id = reinterpret_cast<mongoc_transaction_opt_t*>(&txn_identity);

        auto txn_destroy = libmongoc::transaction_opts_destroy.create_instance();
        txn_destroy
            ->interpose([&](mongoc_transaction_opt_t* ptr) -> void {
                if (ptr) {
                    CHECK(ptr == txn_id);
                }
            })
            .forever();
        auto const txn_opts = v1::transaction_options::internal::make(txn_id);

        int with_counter = 0;
        with->interpose(
            [&](mongoc_client_session_t* ptr,
                mongoc_client_session_with_transaction_cb_t cb,
                mongoc_transaction_opt_t const* opts,
                void* ctx,
                bson_t* reply,
                bson_error_t* error) -> bool {
                CHECK(ptr == mocks.session_id);
                REQUIRE(cb != nullptr);
                CHECK(opts == txn_id);
                CHECK(ctx != nullptr);
                REQUIRE(reply != nullptr);
                REQUIRE(error != nullptr);

                *reply = BSON_INITIALIZER;
                CHECK(error->code == 0);

                bson_t* bson = nullptr;
                CHECK(cb(ptr, ctx, &bson, error));
                CHECK(bson == nullptr);
                CHECK(error->code == 0);

                ++with_counter;

                return true;
            });

        session.with_transaction(std::ref(fn), txn_opts);

        CHECK(with_counter == 1);
        CHECK(fn_counter == 1);
    }
}

TEST_CASE("append_to", "[mongocxx][v1][client_session]") {
    session_mocks_type mocks;
    auto session = mocks.make();

    auto append = libmongoc::client_session_append.create_instance();

    scoped_bson const before = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");
    scoped_bson const input = GENERATE(as<scoped_bson>(), R"({})", R"({"y": 2})");

    CAPTURE(before.view());
    CAPTURE(input.view());

    SECTION("failure") {
        auto const v = GENERATE(as<std::uint32_t>(), 1, 2, 3);
        auto const msg = GENERATE("one", "two", "three");

        append->interpose([&](mongoc_client_session_t const* ptr, bson_t* out, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.session_id);
            REQUIRE(out != nullptr);
            REQUIRE(error != nullptr);

            bson_concat(out, input.bson());
            bson_set_error(error, 0u, v, "%s", msg);

            return false;
        });

        scoped_bson doc = before;
        bson_error_t error = {};
        CHECK_FALSE(v1::client_session::internal::append_to(session, doc, error));
        CHECK(doc.view() == before.view());
        CHECK(error.code == v);
        CHECK_THAT(error.message, Catch::Matchers::Equals(msg));
    }

    SECTION("success") {
        scoped_bson expected;
        expected += before;
        expected += input;

        append->interpose([&](mongoc_client_session_t const* ptr, bson_t* out, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.session_id);
            REQUIRE(out != nullptr);
            REQUIRE(error != nullptr);

            bson_copy_to(input.bson(), out);

            return true;
        });

        scoped_bson doc = before;
        bson_error_t error = {};
        CHECK(v1::client_session::internal::append_to(session, doc, error));
        CHECK(doc.view() == expected.view());
        CHECK(error.code == 0);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_session][options]") {
    client_session::options const opts;

    CHECK(opts.causal_consistency());
    CHECK_FALSE(opts.snapshot());

    {
        auto const txn_opts = opts.default_transaction_opts();
        REQUIRE(txn_opts.has_value()); // Never empty?

        v1::transaction_options const default_txn_opts;

        CHECK(txn_opts->max_commit_time_ms() == default_txn_opts.max_commit_time_ms());
        CHECK(txn_opts->read_concern() == default_txn_opts.read_concern());
        CHECK(txn_opts->read_preference() == default_txn_opts.read_preference());
        CHECK(txn_opts->write_concern() == default_txn_opts.write_concern());
    }
}

TEST_CASE("causal_consistency", "[mongocxx][v1][client_session][options]") {
    identity_type identity;
    auto const opts_id = reinterpret_cast<mongoc_session_opt_t*>(&identity);

    int set_count = 0;
    int get_count = 0;

    auto destroy = libmongoc::session_opts_destroy.create_instance();
    auto set_causal_consistency = libmongoc::session_opts_set_causal_consistency.create_instance();
    auto get_causal_consistency = libmongoc::session_opts_get_causal_consistency.create_instance();

    auto const input = GENERATE(false, true);

    destroy->interpose([&](mongoc_session_opt_t* ptr) -> void { CHECK(ptr == opts_id); }).forever();
    set_causal_consistency
        ->interpose([&](mongoc_session_opt_t* ptr, bool v) -> void {
            CHECK(ptr == opts_id);
            CHECK(v == input);
            ++set_count;
        })
        .forever();
    get_causal_consistency
        ->interpose([&](mongoc_session_opt_t const* ptr) -> bool {
            CHECK(ptr == opts_id);
            ++get_count;
            return input;
        })
        .forever();

    auto opts = v1::client_session::options::internal::make(opts_id);

    CHECK(opts.causal_consistency(input).causal_consistency() == input);
    CHECK(set_count == 1);
    CHECK(get_count == 1);
}

TEST_CASE("snapshot", "[mongocxx][v1][client_session][options]") {
    identity_type identity;
    auto const opts_id = reinterpret_cast<mongoc_session_opt_t*>(&identity);

    int set_count = 0;
    int get_count = 0;

    auto destroy = libmongoc::session_opts_destroy.create_instance();
    auto set_snapshot = libmongoc::session_opts_set_snapshot.create_instance();
    auto get_snapshot = libmongoc::session_opts_get_snapshot.create_instance();

    auto const input = GENERATE(false, true);

    destroy->interpose([&](mongoc_session_opt_t* ptr) -> void { CHECK(ptr == opts_id); }).forever();
    set_snapshot
        ->interpose([&](mongoc_session_opt_t* ptr, bool v) -> void {
            CHECK(ptr == opts_id);
            CHECK(v == input);
            ++set_count;
        })
        .forever();
    get_snapshot
        ->interpose([&](mongoc_session_opt_t const* ptr) -> bool {
            CHECK(ptr == opts_id);
            ++get_count;
            return input;
        })
        .forever();

    auto opts = v1::client_session::options::internal::make(opts_id);

    CHECK(opts.snapshot(input).snapshot() == input);
    CHECK(set_count == 1);
    CHECK(get_count == 1);
}

TEST_CASE("default_transaction_opts", "[mongocxx][v1][client_session][options]") {
    identity_type identity;
    identity_type txn_identity;

    auto const opts_id = reinterpret_cast<mongoc_session_opt_t*>(&identity);
    auto const txn_opts_id = reinterpret_cast<mongoc_transaction_opt_t*>(&txn_identity);

    int set_count = 0;
    int get_count = 0;
    int clone_count = 0;

    auto destroy = libmongoc::session_opts_destroy.create_instance();
    auto set_default_transaction_opts = libmongoc::session_opts_set_default_transaction_opts.create_instance();
    auto get_default_transaction_opts = libmongoc::session_opts_get_default_transaction_opts.create_instance();
    auto txn_destroy = libmongoc::transaction_opts_destroy.create_instance();
    auto txn_clone = libmongoc::transaction_opts_clone.create_instance();

    destroy->interpose([&](mongoc_session_opt_t* ptr) -> void { CHECK(ptr == opts_id); }).forever();
    set_default_transaction_opts
        ->interpose([&](mongoc_session_opt_t* ptr, mongoc_transaction_opt_t const* txn_opts) -> void {
            CHECK(ptr == opts_id);
            CHECK(txn_opts == txn_opts_id);
            ++set_count;
        })
        .forever();
    get_default_transaction_opts
        ->interpose([&](mongoc_session_opt_t const* ptr) -> mongoc_transaction_opt_t const* {
            CHECK(ptr == opts_id);
            ++get_count;
            return txn_opts_id;
        })
        .forever();
    txn_destroy
        ->interpose([&](mongoc_transaction_opt_t* ptr) -> void {
            if (ptr) {
                CHECK(ptr == txn_opts_id);
            }
        })
        .forever();
    txn_clone
        ->interpose([&](mongoc_transaction_opt_t const* ptr) -> mongoc_transaction_opt_t* {
            CHECK(ptr == txn_opts_id);
            ++clone_count;
            return txn_opts_id;
        })
        .forever();

    auto opts = v1::client_session::options::internal::make(opts_id);

    CHECK_NOTHROW(opts.default_transaction_opts(v1::transaction_options::internal::make(txn_opts_id)));
    {
        auto const res = opts.default_transaction_opts();
        REQUIRE(res.has_value());
        CHECK(v1::transaction_options::internal::as_mongoc(*res) == txn_opts_id);
    }
    CHECK(set_count == 1);
    CHECK(get_count == 1);
    CHECK(clone_count == 1);
}

} // namespace v1
} // namespace mongocxx
