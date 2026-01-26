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

#include <mongocxx/v1/client.hh>

//

#include <mongocxx/v1/apm.hpp>
#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/pipeline.hpp>
#include <mongocxx/v1/server_api.hpp>

#include <mongocxx/v1/auto_encryption_options.hh>
#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/database.hh>
#include <mongocxx/v1/tls.hh>
#include <mongocxx/v1/uri.hh>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/ssl.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = mongocxx::v1::client::errc;

TEST_CASE("error code", "[bsoncxx][v1][client][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = mongocxx::v1::client::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::client"));

    auto const zero_errc = make_error_condition(static_cast<std::errc>(0));

    SECTION("unknown") {
        std::error_code const ec = static_cast<code>(-1);

        CHECK(ec.category() == category);
        CHECK(ec.value() == -1);
        CHECK(ec);
        CHECK(ec.message() == std::string(category.name()) + ":-1");
    }

    SECTION("zero") {
        std::error_code const ec = code::zero;

        CHECK(ec.category() == category);
        CHECK(ec.value() == 0);
        CHECK_FALSE(ec);
        CHECK(ec.message() == "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("non-zero") {
        std::error_code const ec = code::tls_not_enabled;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::tls_not_enabled) == source_errc::mongocxx);
        CHECK(make_error_code(code::tls_not_supported) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::tls_not_enabled) == type_errc::invalid_argument);
        CHECK(make_error_code(code::tls_not_supported) == type_errc::invalid_argument);
    }
}

namespace {

struct identity_type {};

// Represent a successfully-constructed mocked `v1::client` object.
struct client_mocks_type {
    using uri_destroy_type = decltype(libmongoc::uri_destroy.create_instance());
    using uri_copy_type = decltype(libmongoc::uri_copy.create_instance());
    using uri_get_tls_type = decltype(libmongoc::uri_get_tls.create_instance());
    using client_destroy_type = decltype(libmongoc::client_destroy.create_instance());
    using client_new_type = decltype(libmongoc::client_new_from_uri_with_error.create_instance());

    identity_type uri_identity;
    identity_type client_identity;

    mongoc_uri_t* uri_id = reinterpret_cast<mongoc_uri_t*>(&uri_identity);
    mongoc_client_t* client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    uri_destroy_type uri_destroy;
    uri_copy_type uri_copy;
    uri_get_tls_type uri_get_tls;
    client_destroy_type client_destroy;
    client_new_type client_new_from_uri_with_error;

    v1::uri uri;

    ~client_mocks_type() = default;
    client_mocks_type(client_mocks_type&& other) noexcept = delete;
    client_mocks_type& operator=(client_mocks_type&& other) noexcept = delete;
    client_mocks_type(client_mocks_type const& other) = delete;
    client_mocks_type& operator=(client_mocks_type const& other) = delete;

    client_mocks_type()
        : uri_destroy{libmongoc::uri_destroy.create_instance()},
          uri_copy{libmongoc::uri_copy.create_instance()},
          uri_get_tls{libmongoc::uri_get_tls.create_instance()},
          client_destroy{libmongoc::client_destroy.create_instance()},
          client_new_from_uri_with_error{libmongoc::client_new_from_uri_with_error.create_instance()},
          uri{v1::uri::internal::make(uri_id)} {
        uri_destroy
            ->interpose([&](mongoc_uri_t* ptr) {
                if (ptr) {
                    CHECK(ptr == uri_id);
                }
            })
            .forever();

        uri_copy
            ->interpose([&](mongoc_uri_t const* ptr) -> mongoc_uri_t* {
                CHECK(ptr == uri_id);
                FAIL("should not reach this point");
                return nullptr;
            })
            .forever();

        uri_get_tls
            ->interpose([&](mongoc_uri_t const* ptr) -> bool {
                CHECK(ptr == uri_id);
                return false;
            })
            .forever();

        client_destroy->interpose([&](mongoc_client_t* ptr) { CHECK(ptr == client_id); }).forever();

        client_new_from_uri_with_error
            ->interpose([&](mongoc_uri_t const* uri, bson_error_t* error) -> mongoc_client_t* {
                CHECK(uri == uri_id);
                CHECK(error != nullptr);
                return client_id;
            })
            .forever();
    }

    template <typename... Args>
    v1::client make(Args&&... args) {
        return {std::move(uri), std::forward<Args>(args)...};
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
        destroy->interpose([&](mongoc_client_session_t* ptr) -> void { CHECK(ptr == session_id); }).forever();

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

TEST_CASE("exceptions", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    SECTION("mongocxx") {
#if MONGOCXX_SSL_IS_ENABLED()
        SECTION("tls_not_enabled") {
            mocks.uri_get_tls->interpose([&](mongoc_uri_t const* uri) -> bool {
                CHECK(uri == mocks.uri_id);
                return false;
            });

            auto set_ssl_opts = libmongoc::client_set_ssl_opts.create_instance();
            set_ssl_opts->interpose([&](mongoc_client_t* client, mongoc_ssl_opt_t const* opts) -> void {
                CHECK(client == mocks.client_id);
                CHECK(opts != nullptr);
                FAIL("should not reach this point");
            });

            client::options opts;
            CHECK_NOTHROW(opts.tls_opts(v1::tls{}));
            CHECK_THROWS_WITH_CODE(mocks.make(std::move(opts)), code::tls_not_enabled);
        }
#else
        SECTION("tls_not_supported") {
            auto const tls_enabled = GENERATE(false, true);
            CAPTURE(tls_enabled);

            auto const tls_opts_set = GENERATE(false, true);
            CAPTURE(tls_opts_set);

            if (tls_enabled || tls_opts_set) {
                mocks.uri_get_tls->interpose([&](mongoc_uri_t const* uri) -> bool {
                    CHECK(uri == mocks.uri_id);
                    return tls_enabled;
                });

                client::options opts;
                CHECK_NOTHROW(opts.tls_opts(v1::tls{}));
                CHECK_THROWS_WITH_CODE(mocks.make(std::move(opts)), code::tls_not_supported);
            }
        }
#endif
    }

    SECTION("mongoc") {
        auto const v = GENERATE(1, 2, 3);
        auto const msg = GENERATE("one", "two", "three");

        auto const set_error = [&](bson_error_t* error) {
            REQUIRE(error != nullptr);
            bson_set_error(error, MONGOC_ERROR_CLIENT, static_cast<std::uint32_t>(v), "%s", msg);
            error->reserved = 2; // MONGOC_ERROR_CATEGORY
        };

        SECTION("new_from_uri_with_error") {
            mocks.client_new_from_uri_with_error->interpose(
                [&](mongoc_uri_t const* uri, bson_error_t* error) -> mongoc_client_t* {
                    CHECK(uri == mocks.uri_id);
                    set_error(error);
                    return nullptr;
                });

            try {
                (void)mocks.make();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == static_cast<int>(v));
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("enable_auto_encryption") {
            auto enable_auto_encryption = libmongoc::client_enable_auto_encryption.create_instance();
            enable_auto_encryption->interpose(
                [&](mongoc_client_t* client, mongoc_auto_encryption_opts_t* opts, bson_error_t* error) -> bool {
                    CHECK(client == mocks.client_id);
                    CHECK(opts != nullptr);
                    set_error(error);
                    return false;
                });

            client::options opts;
            CHECK_NOTHROW(opts.auto_encryption_opts(v1::auto_encryption_options{}));

            try {
                (void)mocks.make(std::move(opts));
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == static_cast<int>(v));
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("set_server_api") {
            auto set_server_api = libmongoc::client_set_server_api.create_instance();
            set_server_api->interpose(
                [&](mongoc_client_t* client, mongoc_server_api_t const* api, bson_error_t* error) -> bool {
                    CHECK(client == mocks.client_id);
                    CHECK(api != nullptr);
                    set_error(error);
                    return false;
                });

            client::options opts;
            CHECK_NOTHROW(opts.server_api_opts(v1::server_api{v1::server_api::version::k_version_1}));

            try {
                (void)mocks.make(std::move(opts));
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == static_cast<int>(v));
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("list_database_names") {
            auto client = mocks.make();

            auto get_database_names_with_opts = libmongoc::client_get_database_names_with_opts.create_instance();
            get_database_names_with_opts->interpose(
                [&](mongoc_client_t* client, bson_t const* opts, bson_error_t* error) -> char** {
                    CHECK(client == mocks.client_id);
                    CHECK(opts == nullptr);
                    set_error(error);
                    return nullptr;
                });

            try {
                client.list_database_names();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == static_cast<int>(v));
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("start_session") {
            auto client = mocks.make();

            auto start_session = libmongoc::client_start_session.create_instance();
            start_session->interpose(
                [&](mongoc_client_t* ptr,
                    mongoc_session_opt_t const* opts,
                    bson_error_t* err) -> mongoc_client_session_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(opts == nullptr);
                    REQUIRE(err != nullptr);

                    set_error(err);

                    return nullptr;
                });

            try {
                (void)client.start_session();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == v);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
            }
        }

        SECTION("session") {
            auto client = mocks.make();

            session_mocks_type session_mocks;
            auto const session = session_mocks.make(client);

            session_mocks.append
                ->interpose([&](mongoc_client_session_t const* ptr, bson_t* out, bson_error_t* error) -> bool {
                    CHECK(ptr == session_mocks.session_id);
                    CHECK(out != nullptr);
                    REQUIRE(error != nullptr);

                    set_error(error);

                    return false;
                })
                .forever();

            SECTION("list_databases") {
                auto const op = [&](bsoncxx::v1::document::view const* opts_ptr) {
                    try {
                        opts_ptr ? client.list_databases(session, *opts_ptr) : client.list_databases(session);
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::mongoc);
                        CHECK(ex.code().value() == static_cast<int>(v));
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                    }
                };

                SECTION("no options") {
                    op(nullptr);
                }

                SECTION("with options") {
                    scoped_bson const doc{R"({"x": 1})"};
                    auto const view = doc.view();
                    op(&view);
                }
            }

            SECTION("list_database_names") {
                auto const op = [&](bsoncxx::v1::document::view const* filter_ptr) -> void {
                    try {
                        filter_ptr ? client.list_database_names(session, *filter_ptr)
                                   : client.list_database_names(session);
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::mongoc);
                        CHECK(ex.code().value() == static_cast<int>(v));
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                    }
                };

                SECTION("no filter") {
                    op(nullptr);
                }

                SECTION("with filter") {
                    scoped_bson const doc{R"({"x": 1})"};
                    auto const view = doc.view();
                    op(&view);
                }
            }

            SECTION("watch") {
                auto const op = [&](v1::pipeline const* pipeline_ptr,
                                    v1::change_stream::options const* opts_ptr) -> void {
                    try {
                        pipeline_ptr ? (opts_ptr ? client.watch(session, *pipeline_ptr, *opts_ptr)
                                                 : client.watch(session, *pipeline_ptr))
                                     : (opts_ptr ? client.watch(session, *opts_ptr) : client.watch(session));
                        FAIL("should not reach this point");
                    } catch (v1::exception const& ex) {
                        CHECK(ex.code() == v1::source_errc::mongoc);
                        CHECK(ex.code().value() == static_cast<int>(v));
                        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(msg));
                    }
                };

                SECTION("no options") {
                    op(nullptr, nullptr);
                }

                SECTION("with options") {
                    v1::change_stream::options const opts;
                    op(nullptr, &opts);
                }

                SECTION("with pipeline") {
                    v1::pipeline const pipeline;

                    SECTION("no options") {
                        op(&pipeline, nullptr);
                    }

                    SECTION("with options") {
                        v1::change_stream::options const opts;
                        op(&pipeline, &opts);
                    }
                }
            }
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][client]") {
    identity_type id1;
    identity_type id2;

    auto const client1 = reinterpret_cast<mongoc_client_t*>(&id1);
    auto const client2 = reinterpret_cast<mongoc_client_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::client_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_client_t* ptr) -> void {
            if (ptr != client1 && ptr != client2) {
                FAIL_CHECK("unexpected mongoc_client_t");
            }
            ++destroy_count;
        })
        .forever();

    auto source = client::internal::make(client1);
    auto target = client::internal::make(client2);

    REQUIRE(client::internal::as_mongoc(source) == client1);
    REQUIRE(client::internal::as_mongoc(target) == client2);

    REQUIRE(source);
    REQUIRE(target);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.
            CHECK_FALSE(source);

            CHECK(move);
            CHECK(client::internal::as_mongoc(move) == client1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.
            CHECK_FALSE(move);

            CHECK(target);
            CHECK(client::internal::as_mongoc(target) == client1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][client][options]") {
    client::options source;
    client::options target;

    auto source_value = std::string{"source"};
    auto target_value = std::string{"target"};

    source.tls_opts(std::move(v1::tls{}.ca_file(source_value)));
    target.tls_opts(std::move(v1::tls{}.ca_file(target_value)));

    REQUIRE(source.tls_opts().value().ca_file() == "source");
    REQUIRE(target.tls_opts().value().ca_file() == "target");

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.tls_opts().value().ca_file() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.tls_opts().value().ca_file() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.tls_opts().value().ca_file() == source_value);
        CHECK(copy.tls_opts().value().ca_file() == source_value);

        target = copy;

        CHECK(copy.tls_opts().value().ca_file() == source_value);
        CHECK(target.tls_opts().value().ca_file() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][client]") {
    client const client;

    CHECK_FALSE(client);
}

TEST_CASE("default", "[mongocxx][v1][client][options]") {
    client::options const opts;

    CHECK_FALSE(opts.tls_opts().has_value());
    CHECK_FALSE(opts.auto_encryption_opts().has_value());
    CHECK_FALSE(opts.apm_opts().has_value());
    CHECK_FALSE(opts.server_api_opts().has_value());
}

TEST_CASE("tls_opts", "[mongocxx][v1][client]") {
#if MONGOCXX_SSL_IS_ENABLED()
    client_mocks_type mocks;

    auto set_ssl_opts = libmongoc::client_set_ssl_opts.create_instance();
    set_ssl_opts
        ->interpose([&](mongoc_client_t* client, mongoc_ssl_opt_t const* opts) {
            CHECK(client == mocks.client_id);
            CHECK(opts != nullptr);
            FAIL("should not reach this point");
        })
        .forever();

    SECTION("string") {
        using data_mem_type = char const* mongoc_ssl_opt_t::*;
        using setter_type = v1::tls& (v1::tls::*)(std::string);

        std::string value{"a very long string to avoid small string optimization for unique identity checks"};
        auto const value_ptr = value.c_str();

        setter_type setter = nullptr;
        data_mem_type data_mem = {};

        std::tie(setter, data_mem) = GENERATE(
            table<setter_type, data_mem_type>({
                {&v1::tls::pem_file, &mongoc_ssl_opt_t::pem_file},
                {&v1::tls::pem_password, &mongoc_ssl_opt_t::pem_pwd},
                {&v1::tls::ca_file, &mongoc_ssl_opt_t::ca_file},
                {&v1::tls::ca_dir, &mongoc_ssl_opt_t::ca_dir},
                {&v1::tls::crl_file, &mongoc_ssl_opt_t::crl_file},
            }));

        int set_counter = 0;
        set_ssl_opts->interpose([&](mongoc_client_t* client, mongoc_ssl_opt_t const* opts) -> void {
            CHECK(client == mocks.client_id);
            REQUIRE(opts != nullptr);
            CHECK(static_cast<void const*>(opts->*data_mem) == static_cast<void const*>(value_ptr));
            ++set_counter;
        });

        int get_counter = 0;
        mocks.uri_get_tls->interpose([&](mongoc_uri_t const* uri) -> bool {
            CHECK(uri == mocks.uri_id);
            ++get_counter;
            return true;
        });

        {
            v1::tls tls_opts;
            CHECK_NOTHROW((tls_opts.*setter)(std::move(value)));

            v1::client::options opts;
            CHECK_NOTHROW(opts.tls_opts(std::move(tls_opts)));
            (void)mocks.make(std::move(opts));
        }

        CHECK(set_counter == 1);
        CHECK(get_counter == 1);
    }

    SECTION("bool") {
        using data_mem_type = bool mongoc_ssl_opt_t::*;
        using setter_type = v1::tls& (v1::tls::*)(bool);

        auto const value = GENERATE(false, true);

        setter_type setter = nullptr;
        data_mem_type data_mem = {};

        std::tie(setter, data_mem) = GENERATE(
            table<setter_type, data_mem_type>({
                {&v1::tls::allow_invalid_certificates, &mongoc_ssl_opt_t::weak_cert_validation},
            }));

        int set_counter = 0;
        set_ssl_opts->interpose([&](mongoc_client_t* client, mongoc_ssl_opt_t const* opts) -> void {
            CHECK(client == mocks.client_id);
            REQUIRE(opts != nullptr);
            CHECK(opts->*data_mem == value);
            ++set_counter;
        });

        int get_counter = 0;
        mocks.uri_get_tls->interpose([&](mongoc_uri_t const* uri) -> bool {
            CHECK(uri == mocks.uri_id);
            ++get_counter;
            return true;
        });

        {
            v1::tls tls_opts;
            CHECK_NOTHROW((tls_opts.*setter)(std::move(value)));

            v1::client::options opts;
            CHECK_NOTHROW(opts.tls_opts(std::move(tls_opts)));
            (void)mocks.make(std::move(opts));
        }

        CHECK(set_counter == 1);
        CHECK(get_counter == 1);
    }
#else
    SKIP(std::error_code{code::tls_not_supported}.message());
#endif
}

namespace {

template <typename Mock>
void test_auto_encryption_opts_bool(
    client_mocks_type& mocks,
    Mock& mock,
    v1::auto_encryption_options& (v1::auto_encryption_options::*setter)(bool),
    mongoc_auto_encryption_opts_t* opts_id) {
    auto const input = GENERATE(false, true);

    int counter = 0;

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_auto_encryption_opts_t* ptr, bool v) -> void {
        CHECK(ptr == opts_id);
        CHECK(v == input);
        ++counter;
    });

    {
        v1::auto_encryption_options auto_encryption_opts;
        CHECK_NOTHROW((auto_encryption_opts.*setter)(input));

        client::options opts;
        CHECK_NOTHROW(opts.auto_encryption_opts(std::move(auto_encryption_opts)));
        (void)mocks.make(std::move(opts));
    }

    CHECK(counter == 1);
}

template <typename Mock>
void test_auto_encryption_opts_doc(
    client_mocks_type& mocks,
    Mock& mock,
    v1::auto_encryption_options& (v1::auto_encryption_options::*setter)(bsoncxx::v1::document::value),
    mongoc_auto_encryption_opts_t* opts_id) {
    auto const doc = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

    int counter = 0;

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_auto_encryption_opts_t* ptr, bson_t const* bson) -> void {
        CHECK(ptr == opts_id);
        REQUIRE(bson != nullptr);
        CHECK(scoped_bson_view{bson}.view() == doc.view());
        ++counter;
    });

    {
        v1::auto_encryption_options auto_encryption_opts;
        CHECK_NOTHROW((auto_encryption_opts.*setter)(doc.value()));

        client::options opts;
        CHECK_NOTHROW(opts.auto_encryption_opts(std::move(auto_encryption_opts)));
        (void)mocks.make(std::move(opts));
    }

    CHECK(counter == 1);
}

} // namespace

TEST_CASE("auto_encryption_opts", "[mongocxx][v1][client]") {
    // Workaround baffling segmentation faults during destruction of the `mocks` local variable when compiled with GCC
    // on RHEL 8 ARM64. Not observed on any other target platform. Compiling with Clang or enabling ASAN suppresses this
    // runtime error. This issue seems to only affect this specific test case. (???)
    std::unique_ptr<client_mocks_type> mocks_owner{new client_mocks_type{}};
    auto& mocks = *mocks_owner;

    identity_type opts_identity;
    auto const opts_id = reinterpret_cast<mongoc_auto_encryption_opts_t*>(&opts_identity);

    auto opts_destroy = libmongoc::auto_encryption_opts_destroy.create_instance();
    auto opts_new = libmongoc::auto_encryption_opts_new.create_instance();
    auto enable_auto_encryption = libmongoc::client_enable_auto_encryption.create_instance();

    opts_destroy->interpose([&](mongoc_auto_encryption_opts_t* ptr) { CHECK(ptr == opts_id); }).forever();

    opts_new->interpose([&]() -> mongoc_auto_encryption_opts_t* { return opts_id; }).forever();

    int enable_count = 0;
    enable_auto_encryption->interpose(
        [&](mongoc_client_t* client, mongoc_auto_encryption_opts_t* opts, bson_error_t* error) -> bool {
            CHECK(client == mocks.client_id);
            CHECK(opts == opts_id);
            CHECK(error != nullptr);
            ++enable_count;
            return true;
        });

    SECTION("key_vault_client") {
        identity_type kv_client_identity;
        auto const kv_client_id = reinterpret_cast<mongoc_client_t*>(&kv_client_identity);
        auto kv_client = v1::client::internal::make(kv_client_id);
        mocks.client_destroy->interpose([&](mongoc_client_t* ptr) -> void {
            if (ptr != kv_client_id) {
                FAIL_CHECK("unexpected mongoc_client_t");
            }
        });

        int counter = 0;
        auto set_keyvault_client = libmongoc::auto_encryption_opts_set_keyvault_client.create_instance();
        set_keyvault_client->interpose([&](mongoc_auto_encryption_opts_t* ptr, mongoc_client_t* kv_ptr) -> void {
            CHECK(ptr == opts_id);
            CHECK(kv_ptr == kv_client_id);
            ++counter;
        });

        {
            v1::auto_encryption_options auto_encryption_opts;
            CHECK_NOTHROW(auto_encryption_opts.key_vault_client(&kv_client));

            client::options opts;
            CHECK_NOTHROW(opts.auto_encryption_opts(std::move(auto_encryption_opts)));
            mocks.client_destroy->interpose([&](mongoc_client_t* ptr) -> void { CHECK(ptr == mocks.client_id); });
            (void)mocks.make(std::move(opts));
        }

        // Workaround baffling assertion failure when compiling with GCC on RHEL 8 PPC64LE. Not observed on any other
        // target platform. Compiling with Clang or enabling ASAN suppresses this failure. This issue seems to only
        // affect this specific test case. (???)
        CHECK_NOFAIL(counter == 1);
        CHECK(enable_count == 1);
    }

    SECTION("key_vault_pool") {
        // TODO: v1::pool (CXX-3237)
    }

    SECTION("key_vault_namespace") {
        auto const db = GENERATE(as<std::string>(), "db1", "db2");
        auto const coll = GENERATE(as<std::string>(), "coll1", "coll2");

        auto fn = libmongoc::auto_encryption_opts_set_keyvault_namespace.create_instance();

        int counter = 0;
        fn->interpose([&](mongoc_auto_encryption_opts_t* ptr, char const* db_ptr, char const* coll_ptr) -> void {
            CHECK(ptr == opts_id);
            CHECK_THAT(db_ptr, Catch::Matchers::Equals(db));
            CHECK_THAT(coll_ptr, Catch::Matchers::Equals(coll));
            ++counter;
        });

        {
            v1::auto_encryption_options auto_encryption_opts;
            CHECK_NOTHROW(auto_encryption_opts.key_vault_namespace({db, coll}));

            client::options opts;
            CHECK_NOTHROW(opts.auto_encryption_opts(std::move(auto_encryption_opts)));
            (void)mocks.make(std::move(opts));
        }

        CHECK(counter == 1);
        CHECK(enable_count == 1);
    }

    SECTION("kms_providers") {
        test_auto_encryption_opts_doc(
            mocks,
            libmongoc::auto_encryption_opts_set_kms_providers,
            &v1::auto_encryption_options::kms_providers,
            opts_id);
        CHECK(enable_count == 1);
    }

    SECTION("tls_opts") {
        test_auto_encryption_opts_doc(
            mocks, libmongoc::auto_encryption_opts_set_tls_opts, &v1::auto_encryption_options::tls_opts, opts_id);
        CHECK(enable_count == 1);
    }

    SECTION("schema_map") {
        test_auto_encryption_opts_doc(
            mocks, libmongoc::auto_encryption_opts_set_schema_map, &v1::auto_encryption_options::schema_map, opts_id);
        CHECK(enable_count == 1);
    }

    SECTION("encrypted_fields_map") {
        test_auto_encryption_opts_doc(
            mocks,
            libmongoc::auto_encryption_opts_set_encrypted_fields_map,
            &v1::auto_encryption_options::encrypted_fields_map,
            opts_id);
        CHECK(enable_count == 1);
    }

    SECTION("bypass_auto_encryption") {
        test_auto_encryption_opts_bool(
            mocks,
            libmongoc::auto_encryption_opts_set_bypass_auto_encryption,
            &v1::auto_encryption_options::bypass_auto_encryption,
            opts_id);
        CHECK(enable_count == 1);
    }

    SECTION("bypass_query_analysis") {
        test_auto_encryption_opts_bool(
            mocks,
            libmongoc::auto_encryption_opts_set_bypass_query_analysis,
            &v1::auto_encryption_options::bypass_query_analysis,
            opts_id);
        CHECK(enable_count == 1);
    }

    SECTION("extra_options") {
        test_auto_encryption_opts_doc(
            mocks, libmongoc::auto_encryption_opts_set_extra, &v1::auto_encryption_options::extra_options, opts_id);
        CHECK(enable_count == 1);
    }
}

namespace {

struct callback_fn_type {
    template <typename Event>
    void operator()(Event const&) const noexcept {
        FAIL_CHECK("should not reach this point");
    }
};

struct callback_mock_fn_type {
    mongoc_apm_callbacks_t* callbacks_id;
    int* counter;

    callback_mock_fn_type(mongoc_apm_callbacks_t* callbacks_id, int* counter)
        : callbacks_id{callbacks_id}, counter{counter} {
        REQUIRE(callbacks_id != nullptr);
        REQUIRE(counter != nullptr);
    }

    template <typename Callback>
    void operator()(mongoc_apm_callbacks_t* ptr, Callback cb) {
        CHECK(ptr == callbacks_id);
        CHECK(cb != nullptr);
        *counter += 1;
    }
};

} // namespace

TEST_CASE("apm_opts", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    identity_type callbacks_identity;
    auto const callbacks_id = reinterpret_cast<mongoc_apm_callbacks_t*>(&callbacks_identity);

    v1::apm callbacks;

    static constexpr std::size_t N = 12; // Number of APM callback functions.
    std::array<int, N> counters = {};    // Number of times each APM callback (by index) is invoked.
    std::array<std::function<void()>, N> setters = {{
        [&] { callbacks.on_command_started(callback_fn_type{}); },
        [&] { callbacks.on_command_failed(callback_fn_type{}); },
        [&] { callbacks.on_command_succeeded(callback_fn_type{}); },
        [&] { callbacks.on_server_closed(callback_fn_type{}); },
        [&] { callbacks.on_server_description_changed(callback_fn_type{}); },
        [&] { callbacks.on_server_opening(callback_fn_type{}); },
        [&] { callbacks.on_topology_closed(callback_fn_type{}); },
        [&] { callbacks.on_topology_description_changed(callback_fn_type{}); },
        [&] { callbacks.on_topology_opening(callback_fn_type{}); },
        [&] { callbacks.on_server_heartbeat_started(callback_fn_type{}); },
        [&] { callbacks.on_server_heartbeat_failed(callback_fn_type{}); },
        [&] { callbacks.on_server_heartbeat_succeeded(callback_fn_type{}); },
    }};

    auto callbacks_destroy = libmongoc::apm_callbacks_destroy.create_instance();
    auto callbacks_new = libmongoc::apm_callbacks_new.create_instance();

    auto set_apm_callbacks = libmongoc::client_set_apm_callbacks.create_instance();

    auto set_command_started = libmongoc::apm_set_command_started_cb.create_instance();
    auto set_command_failed = libmongoc::apm_set_command_failed_cb.create_instance();
    auto set_command_succeeded = libmongoc::apm_set_command_succeeded_cb.create_instance();
    auto set_server_closed = libmongoc::apm_set_server_closed_cb.create_instance();
    auto set_server_description_changed = libmongoc::apm_set_server_changed_cb.create_instance();
    auto set_server_opening = libmongoc::apm_set_server_opening_cb.create_instance();
    auto set_topology_closed = libmongoc::apm_set_topology_closed_cb.create_instance();
    auto set_topology_description_changed = libmongoc::apm_set_topology_changed_cb.create_instance();
    auto set_topology_opening = libmongoc::apm_set_topology_opening_cb.create_instance();
    auto set_server_heartbeat_started = libmongoc::apm_set_server_heartbeat_started_cb.create_instance();
    auto set_server_heartbeat_failed = libmongoc::apm_set_server_heartbeat_failed_cb.create_instance();
    auto set_server_heartbeat_succeeded = libmongoc::apm_set_server_heartbeat_succeeded_cb.create_instance();

    callbacks_destroy->interpose([&](mongoc_apm_callbacks_t* ptr) { CHECK(ptr == callbacks_id); });

    callbacks_new->interpose([&]() -> mongoc_apm_callbacks_t* { return callbacks_id; }).forever();

    set_apm_callbacks->interpose(
        [&](mongoc_client_t* client, mongoc_apm_callbacks_t* callbacks, void* context) -> bool {
            CHECK(client == mocks.client_id);
            CHECK(callbacks == callbacks_id);
            CHECK(context != nullptr);
            return true;
        });

    // Each `set_*` increments the appropriate counter in `counters` (by index) upon invocation.
    {
        std::size_t idx = 0;

        set_command_started->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_command_failed->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_command_succeeded->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_server_closed->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_server_description_changed->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_server_opening->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_topology_closed->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_topology_description_changed->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_topology_opening->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_server_heartbeat_started->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_server_heartbeat_failed->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
        set_server_heartbeat_succeeded->interpose(callback_mock_fn_type{callbacks_id, &counters[idx++]});
    }

    auto const idx = GENERATE(range(std::size_t{0}, N));
    CAPTURE(idx);

    setters[idx]();

    {
        client::options opts;
        CHECK_NOTHROW(opts.apm_opts(std::move(callbacks)));
        (void)mocks.make(std::move(opts));
    }

    // Only one APM callback (by index) should have been invoked.
    for (std::size_t i = 0u; i < N; ++i) {
        CHECKED_IF(i == idx) {
            CHECK(counters[i] == 1);
        }

        else {
            CHECK(counters[i] == 0);
        }
    }
}

TEST_CASE("server_api_opts", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    identity_type server_api_identity;
    auto const server_api_id = reinterpret_cast<mongoc_server_api_t*>(&server_api_identity);

    auto server_api_destroy = libmongoc::server_api_destroy.create_instance();
    auto server_api_new = libmongoc::server_api_new.create_instance();
    auto set_server_api = libmongoc::client_set_server_api.create_instance();

    server_api_destroy->interpose([&](mongoc_server_api_t* ptr) { CHECK(ptr == server_api_id); }).forever();

    server_api_new
        ->interpose([&](mongoc_server_api_version_t version) -> mongoc_server_api_t* {
            CHECK(version == mongoc_server_api_version_t::MONGOC_SERVER_API_V1);
            return server_api_id;
        })
        .forever();

    int set_counter = 0;
    set_server_api->interpose([&](mongoc_client_t* ptr, mongoc_server_api_t const* api, bson_error_t* error) -> bool {
        CHECK(ptr == mocks.client_id);
        CHECK(api == server_api_id);
        CHECK(error != nullptr);
        ++set_counter;
        return true;
    });

    v1::server_api api{v1::server_api::version::k_version_1};

    SECTION("strict") {
        auto const input = GENERATE(false, true);

        CHECK_NOTHROW(api.strict(input));

        int counter = 0;

        auto strict = libmongoc::server_api_strict.create_instance();
        strict->interpose([&](mongoc_server_api_t* ptr, bool v) {
            CHECK(ptr == server_api_id);
            CHECK(v == input);
            ++counter;
        });

        {
            client::options opts;
            CHECK_NOTHROW(opts.server_api_opts(std::move(api)));
            (void)mocks.make(std::move(opts));
        }

        CHECK(counter == 1);
        CHECK(set_counter == 1);
    }

    SECTION("deprecation_errors") {
        auto const input = GENERATE(false, true);

        CHECK_NOTHROW(api.deprecation_errors(input));

        int counter = 0;

        auto deprecation_errors = libmongoc::server_api_deprecation_errors.create_instance();
        deprecation_errors->interpose([&](mongoc_server_api_t* ptr, bool v) {
            CHECK(ptr == server_api_id);
            CHECK(v == input);
            ++counter;
        });

        {
            client::options opts;
            CHECK_NOTHROW(opts.server_api_opts(std::move(api)));
            (void)mocks.make(std::move(opts));
        }

        CHECK(counter == 1);
        CHECK(set_counter == 1);
    }
}

TEST_CASE("uri", "[mongocxx][v1][client]") {
    identity_type uri1_identity;
    identity_type uri2_identity;
    identity_type client_identity;

    auto const uri1_id = reinterpret_cast<mongoc_uri_t*>(&uri1_identity);
    auto const uri2_id = reinterpret_cast<mongoc_uri_t*>(&uri2_identity);
    auto const client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    auto uri_destroy = libmongoc::uri_destroy.create_instance();
    auto uri_copy = libmongoc::uri_copy.create_instance();
    auto uri_get_tls = libmongoc::uri_get_tls.create_instance();
    auto client_destroy = libmongoc::client_destroy.create_instance();
    auto client_new_from_uri_with_error = libmongoc::client_new_from_uri_with_error.create_instance();
    auto get_uri = libmongoc::client_get_uri.create_instance();

    int uri_destroy_count = 0;
    uri_destroy
        ->interpose([&](mongoc_uri_t* ptr) {
            if (ptr) {
                if (ptr != uri1_id && ptr != uri2_id) {
                    FAIL_CHECK("unexpected mongoc_uri_t");
                }

                ++uri_destroy_count;
            }
        })
        .forever();

    int uri_copy_count = 0;
    uri_copy
        ->interpose([&](mongoc_uri_t const* ptr) -> mongoc_uri_t* {
            CHECK(ptr == uri1_id);
            ++uri_copy_count;
            return uri2_id;
        })
        .forever();

    int uri_get_tls_count = 0;
    uri_get_tls
        ->interpose([&](mongoc_uri_t const* ptr) -> bool {
            CHECK(ptr == uri1_id);
            ++uri_get_tls_count;
            return false;
        })
        .forever();

    client_destroy->interpose([&](mongoc_client_t* ptr) { CHECK(ptr == client_id); }).forever();

    int new_count = 0;
    client_new_from_uri_with_error
        ->interpose([&](mongoc_uri_t const* uri, bson_error_t* error) -> mongoc_client_t* {
            CHECK(uri == uri1_id);
            CHECK(error != nullptr);
            ++new_count;
            return client_id;
        })
        .forever();

    int get_uri_count = 0;
    get_uri->interpose([&](mongoc_client_t const* ptr) -> mongoc_uri_t const* {
        CHECK(ptr == client_id);
        ++get_uri_count;
        return uri1_id;
    });

    {
        v1::client const client{v1::uri::internal::make(uri1_id)};

        CHECK(uri_destroy_count == 1);
        CHECK(uri_copy_count == 0);
        CHECK(get_uri_count == 0);
        CHECK(uri_get_tls_count == 1);

        {
            auto const uri = client.uri();

            CHECK(uri_destroy_count == 1);
            CHECK(uri_copy_count == 1);
            CHECK(get_uri_count == 1);

            CHECK(v1::uri::internal::as_mongoc(uri) == uri2_id);
        }

        CHECK(uri_destroy_count == 2);
        CHECK(uri_copy_count == 1);
        CHECK(get_uri_count == 1);
        CHECK(uri_get_tls_count == 1);
    }

    CHECK(new_count == 1);
}

TEST_CASE("database", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    identity_type database_identity;
    auto const database_id = reinterpret_cast<mongoc_database_t*>(&database_identity);

    auto database_destroy = libmongoc::database_destroy.create_instance();
    auto get_database = libmongoc::client_get_database.create_instance();

    auto const input = GENERATE("a", "b", "c");

    database_destroy->interpose([&](mongoc_database_t* ptr) -> void {
        if (ptr) {
            CHECK(ptr == database_id);
        }
    });

    get_database->interpose([&](mongoc_client_t* ptr, char const* name) -> mongoc_database_t* {
        CHECK(ptr == mocks.client_id);
        CHECK_THAT(name, Catch::Matchers::Equals(input));
        return database_id;
    });

    auto client = mocks.make();

    auto const db = client[input];

    CHECK(v1::database::internal::as_mongoc(db) == database_id);
}

TEST_CASE("list_databases", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    identity_type cursor_identity;
    auto const cursor_id = reinterpret_cast<mongoc_cursor_t*>(&cursor_identity);

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    auto find_databases_with_opts = libmongoc::client_find_databases_with_opts.create_instance();

    cursor_destroy->interpose([&](mongoc_cursor_t* ptr) -> void { CHECK(ptr == cursor_id); });

    auto client = mocks.make();

    int counter = 0;

    auto const names = GENERATE(
        values<std::vector<std::string>>({
            {},
            {"x"},
            {"a", "b", "c"},
        }));

    SECTION("no options") {
        find_databases_with_opts->interpose([&](mongoc_client_t* ptr, bson_t const* opts) -> mongoc_cursor_t* {
            CHECK(ptr == mocks.client_id);
            CHECK(opts == nullptr);
            ++counter;
            return cursor_id;
        });

        {
            auto const cursor = client.list_databases();
            CHECK(v1::cursor::internal::as_mongoc(cursor) == cursor_id);
        }

        CHECK(counter == 1);
    }

    SECTION("with options") {
        scoped_bson const owner{R"({"x": 1})"};

        find_databases_with_opts->interpose([&](mongoc_client_t* ptr, bson_t const* opts) -> mongoc_cursor_t* {
            CHECK(ptr == mocks.client_id);
            REQUIRE(opts != nullptr);
            CHECK(scoped_bson_view{opts}.data() == owner.data());
            ++counter;
            return cursor_id;
        });

        {
            auto const cursor = client.list_databases(owner.view());
            CHECK(v1::cursor::internal::as_mongoc(cursor) == cursor_id);
        }

        CHECK(counter == 1);
    }

    SECTION("session") {
        session_mocks_type session_mocks;
        auto const session = session_mocks.make(client);

        auto const op = [&](bsoncxx::v1::document::view const* opts_ptr) -> void {
            auto const cursor = opts_ptr ? client.list_databases(session, *opts_ptr) : client.list_databases(session);

            CHECK(v1::cursor::internal::as_mongoc(cursor) == cursor_id);
            CHECK(counter == 1);
        };

        SECTION("no options") {
            find_databases_with_opts->interpose([&](mongoc_client_t* ptr, bson_t const* opts) -> mongoc_cursor_t* {
                CHECK(ptr == mocks.client_id);
                REQUIRE(opts != nullptr);

                CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                ++counter;

                return cursor_id;
            });

            op(nullptr);

            CHECK(counter == 1);
        }

        SECTION("with options") {
            scoped_bson const owner{R"({"x": 1})"};
            scoped_bson const expected{R"({"sessionId": 123, "x": 1})"};

            find_databases_with_opts->interpose([&](mongoc_client_t* ptr, bson_t const* opts) -> mongoc_cursor_t* {
                CHECK(ptr == mocks.client_id);
                REQUIRE(opts != nullptr);

                CHECK(scoped_bson_view{opts}.view() == expected.view());

                ++counter;

                return cursor_id;
            });

            auto const view = owner.view();

            op(&view);

            CHECK(counter == 1);
        }
    }
}

TEST_CASE("list_database_names", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    identity_type cursor_identity;
    auto const cursor_id = reinterpret_cast<mongoc_cursor_t*>(&cursor_identity);

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    auto get_database_names_with_opts = libmongoc::client_get_database_names_with_opts.create_instance();

    cursor_destroy->interpose([&](mongoc_cursor_t* ptr) -> void { CHECK(ptr == cursor_id); });

    auto client = mocks.make();

    int counter = 0;

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

    SECTION("no filter") {
        get_database_names_with_opts->interpose(
            [&](mongoc_client_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                CHECK(ptr == mocks.client_id);
                CHECK(opts == nullptr);
                CHECK(error != nullptr);

                ++counter;

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

        auto const result = client.list_database_names();

        CHECK(result == names);
        CHECK(counter == 1);
    }

    SECTION("with filter") {
        scoped_bson const filter{R"({"x": 1})"};
        scoped_bson const options{BCON_NEW("filter", BCON_DOCUMENT(filter.bson()))};

        get_database_names_with_opts->interpose(
            [&](mongoc_client_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                CHECK(ptr == mocks.client_id);
                REQUIRE(opts != nullptr);
                CHECK(error != nullptr);

                CHECK(scoped_bson_view{opts}.view() == options.view());

                ++counter;

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

        auto const result = client.list_database_names(filter.view());

        CHECK(result == names);
        CHECK(counter == 1);
    }

    SECTION("session") {
        session_mocks_type session_mocks;
        auto const session = session_mocks.make(client);

        auto const op = [&](bsoncxx::v1::document::view const* opts_ptr) -> void {
            auto const result =
                opts_ptr ? client.list_database_names(session, *opts_ptr) : client.list_database_names(session);

            CHECK(result == names);
            CHECK(counter == 1);
        };

        SECTION("no filter") {
            get_database_names_with_opts->interpose(
                [&](mongoc_client_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                    CHECK(ptr == mocks.client_id);
                    REQUIRE(opts != nullptr);
                    CHECK(error != nullptr);

                    CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                    ++counter;

                    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): bson compatibility.
                    auto owner = std::unique_ptr<char*[], names_deleter>(
                        static_cast<char**>(bson_malloc0((names.size() + 1u) * sizeof(char*))));

                    for (std::size_t idx = 0u; idx < names.size(); ++idx) {
                        auto const& name = names[idx];
                        owner[idx] = bson_strndup(name.c_str(), name.size());
                    }

                    return owner.release();
                });

            op(nullptr);

            CHECK(counter == 1);
        }

        SECTION("with filter") {
            scoped_bson const owner{R"({"x": 1})"};
            scoped_bson const expected{R"({"sessionId": 123, "filter": {"x": 1}})"};

            get_database_names_with_opts->interpose(
                [&](mongoc_client_t* ptr, bson_t const* opts, bson_error_t* error) -> char** {
                    CHECK(ptr == mocks.client_id);
                    REQUIRE(opts != nullptr);
                    CHECK(error != nullptr);

                    CHECK(scoped_bson_view{opts}.view() == expected.view());

                    ++counter;

                    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): bson compatibility.
                    auto owner = std::unique_ptr<char*[], names_deleter>(
                        static_cast<char**>(bson_malloc0((names.size() + 1u) * sizeof(char*))));

                    for (std::size_t idx = 0u; idx < names.size(); ++idx) {
                        auto const& name = names[idx];
                        owner[idx] = bson_strndup(name.c_str(), name.size());
                    }

                    return owner.release();
                });

            auto const view = owner.view();

            op(&view);

            CHECK(counter == 1);
        }
    }
}

TEST_CASE("start_session", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    auto client = mocks.make();

    identity_type session_identity;
    auto const session_id = reinterpret_cast<mongoc_client_session_t*>(&session_identity);

    auto session_destroy = libmongoc::client_session_destroy.create_instance();
    auto start_session = libmongoc::client_start_session.create_instance();
    auto session_append = libmongoc::client_session_append.create_instance();

    session_destroy->interpose([&](mongoc_client_session_t* ptr) -> void { CHECK(ptr == session_id); }).forever();

    SECTION("no options") {
        int counter = 0;
        start_session
            ->interpose(
                [&](mongoc_client_t* ptr,
                    mongoc_session_opt_t const* opts,
                    bson_error_t* error) -> mongoc_client_session_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(opts == nullptr);
                    CHECK(error != nullptr);
                    ++counter;
                    return session_id;
                })
            .forever();

        auto const session = client.start_session();

        CHECK(v1::client_session::internal::as_mongoc(session) == session_id);
        CHECK(counter == 1);
    }

    SECTION("with options") {
        identity_type opts_identity;
        auto const opts_id = reinterpret_cast<mongoc_session_opt_t*>(&opts_identity);

        auto opts_destroy = libmongoc::session_opts_destroy.create_instance();
        auto opts_copy = libmongoc::session_opts_clone.create_instance();

        opts_destroy->interpose([&](mongoc_session_opt_t* ptr) -> void { CHECK(ptr == opts_id); }).forever();
        opts_copy
            ->interpose([&](mongoc_session_opt_t const* ptr) -> mongoc_session_opt_t* {
                CHECK(ptr == opts_id);
                FAIL("should not reach this point");
                return opts_id;
            })
            .forever();

        auto const opts = v1::client_session::options::internal::make(opts_id);

        int counter = 0;
        start_session
            ->interpose(
                [&](mongoc_client_t* ptr,
                    mongoc_session_opt_t const* opts,
                    bson_error_t* error) -> mongoc_client_session_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(opts == opts_id);
                    CHECK(error != nullptr);
                    ++counter;
                    return session_id;
                })
            .forever();

        auto const session = client.start_session(opts);

        CHECK(v1::client_session::internal::as_mongoc(session) == session_id);
        CHECK(counter == 1);
    }
}

namespace {

template <typename Mock>
void test_watch_opts_string(
    client_mocks_type& mocks,
    Mock& mock,
    v1::change_stream::options& (v1::change_stream::options::*mem_fn)(std::string),
    char const* field,
    mongoc_change_stream_t* stream_id) {
    auto const input = GENERATE("a", "b", "c");

    int count = 0;
    mock->interpose([&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
        CHECK(ptr == mocks.client_id);
        CHECK(scoped_bson_view{pipeline}.view().empty());
        REQUIRE(opts != nullptr);

        ++count;

        auto const e = scoped_bson_view{opts}.view()[field];
        REQUIRE(e);
        CHECK(e.type_id() == bsoncxx::v1::types::id::k_string);
        CHECK(e.get_string().value == input);

        return stream_id;
    });

    {
        auto client = mocks.make();

        v1::change_stream::options stream_opts;
        CHECK_NOTHROW((stream_opts.*mem_fn)(input));

        auto const stream = client.watch(std::move(stream_opts));
        CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
    }

    CHECK(count == 1);
}

template <typename Mock>
void test_watch_opts_doc(
    client_mocks_type& mocks,
    Mock& mock,
    v1::change_stream::options& (v1::change_stream::options::*mem_fn)(bsoncxx::v1::document::value),
    char const* field,
    mongoc_change_stream_t* stream_id) {
    auto const input = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})");

    int count = 0;
    mock->interpose([&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
        CHECK(ptr == mocks.client_id);
        CHECK(scoped_bson_view{pipeline}.view().empty());
        REQUIRE(opts != nullptr);

        ++count;

        auto const e = scoped_bson_view{opts}.view()[field];
        REQUIRE(e);
        CHECK(e.type_id() == bsoncxx::v1::types::id::k_document);
        CHECK(e.get_document().value == input.view());

        return stream_id;
    });

    {
        auto client = mocks.make();

        v1::change_stream::options stream_opts;
        CHECK_NOTHROW((stream_opts.*mem_fn)(input.value()));

        auto const stream = client.watch(std::move(stream_opts));
        CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
    }

    CHECK(count == 1);
}

template <typename Mock>
void test_watch_opts_int32(
    client_mocks_type& mocks,
    Mock& mock,
    v1::change_stream::options& (v1::change_stream::options::*mem_fn)(std::int32_t),
    char const* field,
    mongoc_change_stream_t* stream_id) {
    auto const input = GENERATE(values<std::int32_t>({INT32_MIN, -1, 0, 1, INT32_MAX}));

    int count = 0;
    mock->interpose([&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
        CHECK(ptr == mocks.client_id);
        CHECK(scoped_bson_view{pipeline}.view().empty());
        REQUIRE(opts != nullptr);

        ++count;

        auto const e = scoped_bson_view{opts}.view()[field];
        REQUIRE(e);
        CHECK(e.type_id() == bsoncxx::v1::types::id::k_int32);
        CHECK(e.get_int32().value == input);

        return stream_id;
    });

    {
        auto client = mocks.make();

        v1::change_stream::options stream_opts;
        CHECK_NOTHROW((stream_opts.*mem_fn)(input));

        auto const stream = client.watch(std::move(stream_opts));
        CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
    }

    CHECK(count == 1);
}

template <typename Mock>
void test_watch_opts_type(
    client_mocks_type& mocks,
    Mock& mock,
    v1::change_stream::options& (v1::change_stream::options::*mem_fn)(bsoncxx::v1::types::value),
    char const* field,
    mongoc_change_stream_t* stream_id) {
    auto const input = GENERATE(as<bsoncxx::v1::types::value>(), bsoncxx::v1::types::b_null{}, 1, 2.0, "three");

    int count = 0;
    mock->interpose([&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
        CHECK(ptr == mocks.client_id);
        CHECK(scoped_bson_view{pipeline}.view().empty());
        REQUIRE(opts != nullptr);

        ++count;

        auto const e = scoped_bson_view{opts}.view()[field];
        REQUIRE(e);
        CHECK(e.type_id() == input.type_id());
        CHECK(e.type_view() == input);

        return stream_id;
    });

    {
        auto client = mocks.make();

        v1::change_stream::options stream_opts;
        CHECK_NOTHROW((stream_opts.*mem_fn)(bsoncxx::v1::types::value{input}));

        auto const stream = client.watch(std::move(stream_opts));
        CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
    }

    CHECK(count == 1);
}

template <typename Mock>
void test_watch_opts_timestamp(
    client_mocks_type& mocks,
    Mock& mock,
    v1::change_stream::options& (v1::change_stream::options::*mem_fn)(bsoncxx::v1::types::b_timestamp),
    char const* field,
    mongoc_change_stream_t* stream_id) {
    auto const timestamp = GENERATE(values<std::uint32_t>({0u, 1u, UINT32_MAX - 1u, 0, 1, UINT32_MAX}));
    auto const increment = GENERATE(values<std::uint32_t>({0u, 1u, UINT32_MAX - 1u, 0, 1, UINT32_MAX}));
    auto const input = bsoncxx::v1::types::b_timestamp{timestamp, increment};

    int count = 0;
    mock->interpose([&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
        CHECK(ptr == mocks.client_id);
        CHECK(scoped_bson_view{pipeline}.view().empty());
        REQUIRE(opts != nullptr);

        ++count;

        auto const e = scoped_bson_view{opts}.view()[field];
        REQUIRE(e);
        CHECK(e.type_id() == bsoncxx::v1::types::id::k_timestamp);
        CHECK(e.get_timestamp() == input);

        return stream_id;
    });

    {
        auto client = mocks.make();

        v1::change_stream::options stream_opts;
        CHECK_NOTHROW((stream_opts.*mem_fn)(input));

        auto const stream = client.watch(std::move(stream_opts));
        CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
    }

    CHECK(count == 1);
}

template <typename Mock>
void test_watch_opts_ms(
    client_mocks_type& mocks,
    Mock& mock,
    v1::change_stream::options& (v1::change_stream::options::*mem_fn)(std::chrono::milliseconds),
    char const* field,
    mongoc_change_stream_t* stream_id) {
    auto const value = GENERATE(values<std::int64_t>({INT64_MIN, -1, 0, 1, INT64_MAX}));
    auto const input = std::chrono::milliseconds{value};

    int count = 0;
    mock->interpose([&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
        CHECK(ptr == mocks.client_id);
        CHECK(scoped_bson_view{pipeline}.view().empty());
        REQUIRE(opts != nullptr);

        ++count;

        auto const e = scoped_bson_view{opts}.view()[field];
        REQUIRE(e);
        CHECK(e.type_id() == bsoncxx::v1::types::id::k_int64);
        CHECK(e.get_int64().value == value);

        return stream_id;
    });

    {
        auto client = mocks.make();

        v1::change_stream::options stream_opts;
        CHECK_NOTHROW((stream_opts.*mem_fn)(input));

        auto const stream = client.watch(std::move(stream_opts));
        CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
    }

    CHECK(count == 1);
}

} // namespace

TEST_CASE("watch", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    identity_type stream_identity;
    auto const stream_id = reinterpret_cast<mongoc_change_stream_t*>(&stream_identity);

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    auto watch = libmongoc::client_watch.create_instance();

    change_stream_destroy->interpose([&](mongoc_change_stream_t* ptr) { CHECK(ptr == stream_id); }).forever();

    v1::change_stream::options stream_opts;

    SECTION("no options") {
        int count = 0;
        watch->interpose(
            [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                CHECK(ptr == mocks.client_id);
                CHECK(scoped_bson_view{pipeline}.view().empty());
                CHECK(opts == nullptr);

                ++count;

                return stream_id;
            });

        {
            auto client = mocks.make();
            auto const stream = client.watch();
            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
        }

        CHECK(count == 1);
    }

    SECTION("with options") {
        SECTION("full_document") {
            test_watch_opts_string(mocks, watch, &v1::change_stream::options::full_document, "fullDocument", stream_id);
        }

        SECTION("full_document_before_change") {
            test_watch_opts_string(
                mocks,
                watch,
                &v1::change_stream::options::full_document_before_change,
                "fullDocumentBeforeChange",
                stream_id);
        }

        SECTION("resume_after") {
            test_watch_opts_doc(mocks, watch, &v1::change_stream::options::resume_after, "resumeAfter", stream_id);
        }

        SECTION("start_after") {
            test_watch_opts_doc(mocks, watch, &v1::change_stream::options::start_after, "startAfter", stream_id);
        }

        SECTION("batch_size") {
            test_watch_opts_int32(mocks, watch, &v1::change_stream::options::batch_size, "batchSize", stream_id);
        }

        SECTION("collation") {
            test_watch_opts_doc(mocks, watch, &v1::change_stream::options::collation, "collation", stream_id);
        }

        SECTION("comment") {
            test_watch_opts_type(mocks, watch, &v1::change_stream::options::comment, "comment", stream_id);
        }

        SECTION("start_at_operation_time") {
            test_watch_opts_timestamp(
                mocks, watch, &v1::change_stream::options::start_at_operation_time, "startAtOperationTime", stream_id);
        }

        SECTION("max_await_time") {
            test_watch_opts_ms(mocks, watch, &v1::change_stream::options::max_await_time, "maxAwaitTimeMS", stream_id);
        }
    }

    SECTION("with pipeline") {
        v1::pipeline pipeline;
        pipeline.append_stage(scoped_bson{R"({"x": 1})"}.value());
        auto const data = pipeline.view_array().data();

        int count = 0;

        SECTION("no options") {
            watch->interpose(
                [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(scoped_bson_view{pipeline}.data() == data);
                    CHECK(opts == nullptr);

                    ++count;

                    return stream_id;
                });

            auto client = mocks.make();
            auto const stream = client.watch(pipeline);
            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
        }

        SECTION("with options") {
            watch->interpose(
                [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(scoped_bson_view{pipeline}.data() == data);
                    REQUIRE(opts != nullptr);

                    CHECK(scoped_bson_view{opts}.view().empty());

                    ++count;

                    return stream_id;
                });

            auto client = mocks.make();
            auto const stream = client.watch(pipeline, v1::change_stream::options{});
            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
        }

        CHECK(count == 1);
    }

    SECTION("with session") {
        auto client = mocks.make();

        session_mocks_type session_mocks;
        auto const session = session_mocks.make(client);

        int count = 0;

        auto const op = [&](v1::pipeline const* pipeline_ptr, v1::change_stream::options const* opts_ptr) -> void {
            auto const stream = pipeline_ptr ? (opts_ptr ? client.watch(session, *pipeline_ptr, *opts_ptr)
                                                         : client.watch(session, *pipeline_ptr))
                                             : (opts_ptr ? client.watch(session, *opts_ptr) : client.watch(session));

            CHECK(v1::change_stream::internal::as_mongoc(stream) == stream_id);
            CHECK(count == 1);
        };

        SECTION("no options") {
            watch->interpose(
                [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(scoped_bson_view{pipeline}.view().empty());
                    REQUIRE(opts != nullptr);

                    CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                    ++count;

                    return stream_id;
                });

            op(nullptr, nullptr);
        }

        SECTION("with options") {
            watch->interpose(
                [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                    CHECK(ptr == mocks.client_id);
                    CHECK(scoped_bson_view{pipeline}.view().empty());
                    REQUIRE(opts != nullptr);

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
            auto const data = pipeline.view_array().data();

            SECTION("no options") {
                watch->interpose(
                    [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                        CHECK(ptr == mocks.client_id);
                        CHECK(scoped_bson_view{pipeline}.data() == data);
                        REQUIRE(opts != nullptr);

                        CHECK(scoped_bson_view{opts}.view() == session_mocks.doc.view());

                        ++count;

                        return stream_id;
                    });

                op(&pipeline, nullptr);
            }

            SECTION("with options") {
                watch->interpose(
                    [&](mongoc_client_t* ptr, bson_t const* pipeline, bson_t const* opts) -> mongoc_change_stream_t* {
                        CHECK(ptr == mocks.client_id);
                        CHECK(scoped_bson_view{pipeline}.data() == data);
                        REQUIRE(opts != nullptr);

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

TEST_CASE("reset", "[mongocxx][v1][client]") {
    client_mocks_type mocks;

    auto reset = libmongoc::client_reset.create_instance();
    reset->interpose([&](mongoc_client_t* ptr) -> void { CHECK(ptr == mocks.client_id); });

    (void)mocks.make().reset();
}

} // namespace v1
} // namespace mongocxx
