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

#include <mongocxx/v1/pool.hh>

//

#include <mongocxx/v1/apm.hpp> // IWYU pragma: keep: `.apm_opts()`
#include <mongocxx/v1/auto_encryption_options.hpp>
#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/server_api.hpp>
#include <mongocxx/v1/tls.hpp>

#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/database.hh>
#include <mongocxx/v1/uri.hh>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/ssl.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = mongocxx::v1::pool::errc;

TEST_CASE("error code", "[bsoncxx][v1][pool][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = mongocxx::v1::pool::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::pool"));

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
        std::error_code const ec = code::wait_queue_timeout;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::wait_queue_timeout) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::wait_queue_timeout) == type_errc::runtime_error);
    }
}

namespace {

struct identity_type {};

// Represent a successfully-constructed mocked `v1::client` object.
struct pool_mocks_type {
    using uri_destroy_type = decltype(libmongoc::uri_destroy.create_instance());
    using uri_copy_type = decltype(libmongoc::uri_copy.create_instance());
    using uri_get_tls_type = decltype(libmongoc::uri_get_tls.create_instance());
    using pool_destroy_type = decltype(libmongoc::client_pool_destroy.create_instance());
    using pool_new_type = decltype(libmongoc::client_pool_new_with_error.create_instance());
    using pop_type = decltype(libmongoc::client_pool_pop.create_instance());
    using try_pop_type = decltype(libmongoc::client_pool_try_pop.create_instance());

    identity_type uri_identity;
    identity_type pool_identity;

    mongoc_uri_t* uri_id = reinterpret_cast<mongoc_uri_t*>(&uri_identity);
    mongoc_client_pool_t* pool_id = reinterpret_cast<mongoc_client_pool_t*>(&pool_identity);

    uri_destroy_type uri_destroy = libmongoc::uri_destroy.create_instance();
    uri_copy_type uri_copy = libmongoc::uri_copy.create_instance();
    uri_get_tls_type uri_get_tls = libmongoc::uri_get_tls.create_instance();
    pool_destroy_type destroy = libmongoc::client_pool_destroy.create_instance();
    pool_new_type new_with_error = libmongoc::client_pool_new_with_error.create_instance();

    pop_type pop = libmongoc::client_pool_pop.create_instance();
    try_pop_type try_pop = libmongoc::client_pool_try_pop.create_instance();

    v1::uri uri = v1::uri::internal::make(uri_id);

    ~pool_mocks_type() = default;
    pool_mocks_type(pool_mocks_type&& other) noexcept = delete;
    pool_mocks_type& operator=(pool_mocks_type&& other) noexcept = delete;
    pool_mocks_type(pool_mocks_type const& other) = delete;
    pool_mocks_type& operator=(pool_mocks_type const& other) = delete;

    pool_mocks_type() {
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

        destroy->interpose([&](mongoc_client_pool_t* ptr) { CHECK(ptr == pool_id); }).forever();

        new_with_error
            ->interpose([&](mongoc_uri_t const* uri, bson_error_t* error) -> mongoc_client_pool_t* {
                CHECK(uri == uri_id);
                CHECK(error != nullptr);
                return pool_id;
            })
            .forever();

        pop->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
               CHECK(ptr == pool_id);
               FAIL("should not reach this point");
               return nullptr;
           })
            .forever();

        try_pop
            ->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
                CHECK(ptr == pool_id);
                FAIL("should not reach this point");
                return nullptr;
            })
            .forever();
    }

    template <typename... Args>
    v1::pool make(Args&&... args) {
        return {std::move(uri), std::forward<Args>(args)...};
    }
};

} // namespace

TEST_CASE("exceptions", "[mongocxx][v1][pool]") {
    pool_mocks_type mocks;

    SECTION("mongocxx") {
#if MONGOCXX_SSL_IS_ENABLED()
        SECTION("tls_not_enabled") {
            mocks.uri_get_tls->interpose([&](mongoc_uri_t const* uri) -> bool {
                CHECK(uri == mocks.uri_id);
                return false;
            });

            auto set_ssl_opts = libmongoc::client_pool_set_ssl_opts.create_instance();
            set_ssl_opts->interpose([&](mongoc_client_pool_t* pool, mongoc_ssl_opt_t const* opts) -> void {
                CHECK(pool == mocks.pool_id);
                CHECK(opts != nullptr);
                FAIL("should not reach this point");
            });

            client::options opts;
            CHECK_NOTHROW(opts.tls_opts(v1::tls{}));
            CHECK_THROWS_WITH_CODE(mocks.make(pool::options{std::move(opts)}), v1::client::errc::tls_not_enabled);
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
                CHECK_THROWS_WITH_CODE(mocks.make(pool::options{std::move(opts)}), v1::client::errc::tls_not_supported);
            }
        }
#endif

        SECTION("wait_queue_timeout") {
            mocks.pop->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
                CHECK(ptr == mocks.pool_id);
                return nullptr;
            });

            auto pool = mocks.make();

            CHECK_THROWS_WITH_CODE(pool.acquire(), code::wait_queue_timeout);
        }
    }

    SECTION("mongoc") {
        auto const v = GENERATE(1, 2, 3);
        auto const msg = GENERATE("one", "two", "three");

        auto const set_error = [&](bson_error_t* error) {
            REQUIRE(error != nullptr);
            bson_set_error(error, MONGOC_ERROR_CLIENT, static_cast<std::uint32_t>(v), "%s", msg);
            error->reserved = 2; // MONGOC_ERROR_CATEGORY
        };

        SECTION("new_with_error") {
            mocks.new_with_error->interpose([&](mongoc_uri_t const* uri, bson_error_t* error) -> mongoc_client_pool_t* {
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
            auto enable_auto_encryption = libmongoc::client_pool_enable_auto_encryption.create_instance();
            enable_auto_encryption->interpose(
                [&](mongoc_client_pool_t* ptr, mongoc_auto_encryption_opts_t* opts, bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.pool_id);
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
            auto set_server_api = libmongoc::client_pool_set_server_api.create_instance();
            set_server_api->interpose(
                [&](mongoc_client_pool_t* ptr, mongoc_server_api_t const* api, bson_error_t* error) -> bool {
                    CHECK(ptr == mocks.pool_id);
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
    }
}

TEST_CASE("ownership", "[mongocxx][v1][pool]") {
    identity_type id1;
    identity_type id2;

    auto const pool1 = reinterpret_cast<mongoc_client_pool_t*>(&id1);
    auto const pool2 = reinterpret_cast<mongoc_client_pool_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::client_pool_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_client_pool_t* ptr) -> void {
            if (ptr != pool1 && ptr != pool2) {
                FAIL_CHECK("unexpected mongoc_client_pool_t");
            }
            ++destroy_count;
        })
        .forever();

    auto source = pool::internal::make(pool1);
    auto target = pool::internal::make(pool2);

    REQUIRE(pool::internal::as_mongoc(source) == pool1);
    REQUIRE(pool::internal::as_mongoc(target) == pool2);

    REQUIRE(source);
    REQUIRE(target);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.
            CHECK_FALSE(source);

            CHECK(move);
            CHECK(pool::internal::as_mongoc(move) == pool1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.
            CHECK_FALSE(move);

            CHECK(target);
            CHECK(pool::internal::as_mongoc(target) == pool1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][pool][options]") {
    auto source_value = std::string{"source"};
    auto target_value = std::string{"target"};

    pool::options source = v1::client::options{}.tls_opts(std::move(v1::tls{}.ca_file(source_value)));
    pool::options target = v1::client::options{}.tls_opts(std::move(v1::tls{}.ca_file(target_value)));

    auto const ca_file = [](pool::options& opts) -> std::string {
        return std::string{opts.client_opts().tls_opts().value().ca_file().value()};
    };

    REQUIRE(ca_file(source) == "source");
    REQUIRE(ca_file(target) == "target");

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(ca_file(move) == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(ca_file(target) == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(ca_file(source) == source_value);
        CHECK(ca_file(copy) == source_value);

        target = copy;

        CHECK(ca_file(copy) == source_value);
        CHECK(ca_file(target) == source_value);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][pool][entry]") {
    identity_type id1;
    identity_type id2;

    auto const pool1 = reinterpret_cast<mongoc_client_pool_t*>(&id1);
    auto const pool2 = reinterpret_cast<mongoc_client_pool_t*>(&id2);

    identity_type client1_id;
    identity_type client2_id;

    auto const client1 = reinterpret_cast<mongoc_client_t*>(&client1_id);
    auto const client2 = reinterpret_cast<mongoc_client_t*>(&client2_id);

    auto client_destroy = libmongoc::client_destroy.create_instance();
    auto push = libmongoc::client_pool_push.create_instance();

    client_destroy
        ->interpose([&](mongoc_client_t* ptr) {
            CHECK(ptr == nullptr); // Always released back into mongoc_client_pool_t.
        })
        .forever();

    int push_count = 0;
    push->interpose([&](mongoc_client_pool_t* ptr, mongoc_client_t* client) -> void {
            if (ptr != pool1 && ptr != pool2) {
                FAIL_CHECK("unexpected mongoc_client_pool_t");
            }

            if (client != client1 && client != client2) {
                FAIL_CHECK("unexpected mongoc_client_t");
            }

            ++push_count;
        })
        .forever();

    auto source = pool::entry::internal::make(pool1, client1);
    auto target = pool::entry::internal::make(pool2, client2);

    REQUIRE(source);
    REQUIRE(target);

    REQUIRE(v1::client::internal::as_mongoc(*source) == client1);
    REQUIRE(v1::client::internal::as_mongoc(*target) == client2);

    REQUIRE(push_count == 0);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.
            CHECK_FALSE(source);

            REQUIRE(move);
            REQUIRE(*move);
            CHECK(v1::client::internal::as_mongoc(*move) == client1);
            CHECK(v1::client::internal::as_mongoc(*move.operator->()) == client1);
            CHECK(push_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.
            CHECK_FALSE(move);

            REQUIRE(target);
            REQUIRE(*target);
            CHECK(v1::client::internal::as_mongoc(*target) == client1);
            CHECK(v1::client::internal::as_mongoc(*target.operator->()) == client1);
            CHECK(push_count == 1);
        }

        CHECK(push_count == 1);
    }
}

TEST_CASE("default", "[mongocxx][v1][pool]") {
    v1::pool const pool;

    CHECK(pool);
}

TEST_CASE("default", "[mongocxx][v1][pool][options]") {
    auto const client_opts = pool::options{}.client_opts();

    CHECK_FALSE(client_opts.tls_opts().has_value());
    CHECK_FALSE(client_opts.auto_encryption_opts().has_value());
    CHECK_FALSE(client_opts.apm_opts().has_value());
    CHECK_FALSE(client_opts.server_api_opts().has_value());
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

// Defer thorough test coverage to v1::client::options.
TEST_CASE("apm_opts", "[mongocxx][v1][pool]") {
    pool_mocks_type mocks;

    identity_type callbacks_identity;
    auto const callbacks_id = reinterpret_cast<mongoc_apm_callbacks_t*>(&callbacks_identity);

    v1::apm callbacks;
    callbacks.on_command_started(callback_fn_type{});

    auto callbacks_destroy = libmongoc::apm_callbacks_destroy.create_instance();
    auto callbacks_new = libmongoc::apm_callbacks_new.create_instance();
    auto set_apm_callbacks = libmongoc::client_pool_set_apm_callbacks.create_instance();
    auto set_command_started = libmongoc::apm_set_command_started_cb.create_instance();

    callbacks_destroy->interpose([&](mongoc_apm_callbacks_t* ptr) { CHECK(ptr == callbacks_id); });
    callbacks_new->interpose([&]() -> mongoc_apm_callbacks_t* { return callbacks_id; }).forever();
    set_apm_callbacks->interpose(
        [&](mongoc_client_pool_t* ptr, mongoc_apm_callbacks_t* callbacks, void* context) -> bool {
            CHECK(ptr == mocks.pool_id);
            CHECK(callbacks == callbacks_id);
            CHECK(context != nullptr);
            return true;
        });

    int counter = 0;
    set_command_started->interpose(callback_mock_fn_type{callbacks_id, &counter});

    {
        client::options opts;
        CHECK_NOTHROW(opts.apm_opts(std::move(callbacks)));
        (void)mocks.make(std::move(opts));
    }

    CHECK(counter == 1);
}

// Defer thorough test coverage to v1::client::options.
TEST_CASE("auto_encryption_opts", "[mongocxx][v1][pool]") {
    // Workaround baffling segmentation faults during destruction of the `mocks` local variable when compiled with GCC
    // on RHEL 8 ARM64. Not observed on any other target platform. Compiling with Clang or enabling ASAN suppresses this
    // runtime error. This issue seems to only affect this specific test case. (???)
    std::unique_ptr<pool_mocks_type> mocks_owner{new pool_mocks_type{}};
    auto& mocks = *mocks_owner;

    identity_type opts_identity;
    auto const opts_id = reinterpret_cast<mongoc_auto_encryption_opts_t*>(&opts_identity);

    auto opts_destroy = libmongoc::auto_encryption_opts_destroy.create_instance();
    auto opts_new = libmongoc::auto_encryption_opts_new.create_instance();
    auto enable_auto_encryption = libmongoc::client_pool_enable_auto_encryption.create_instance();

    opts_destroy->interpose([&](mongoc_auto_encryption_opts_t* ptr) { CHECK(ptr == opts_id); }).forever();

    opts_new->interpose([&]() -> mongoc_auto_encryption_opts_t* { return opts_id; }).forever();

    int enable_count = 0;
    enable_auto_encryption->interpose(
        [&](mongoc_client_pool_t* ptr, mongoc_auto_encryption_opts_t* opts, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.pool_id);
            CHECK(opts == opts_id);
            CHECK(error != nullptr);
            ++enable_count;
            return true;
        });

    {
        client::options opts;
        CHECK_NOTHROW(opts.auto_encryption_opts(v1::auto_encryption_options{}));
        (void)mocks.make(std::move(opts));
    }

    CHECK(enable_count == 1);
}

// Defer thorough test coverage to v1::client::options.
TEST_CASE("server_api_opts", "[mongocxx][v1][pool]") {
    pool_mocks_type mocks;

    identity_type server_api_identity;
    auto const server_api_id = reinterpret_cast<mongoc_server_api_t*>(&server_api_identity);

    auto server_api_destroy = libmongoc::server_api_destroy.create_instance();
    auto server_api_new = libmongoc::server_api_new.create_instance();
    auto set_server_api = libmongoc::client_pool_set_server_api.create_instance();

    server_api_destroy->interpose([&](mongoc_server_api_t* ptr) { CHECK(ptr == server_api_id); }).forever();

    server_api_new
        ->interpose([&](mongoc_server_api_version_t version) -> mongoc_server_api_t* {
            CHECK(version == mongoc_server_api_version_t::MONGOC_SERVER_API_V1);
            return server_api_id;
        })
        .forever();

    int set_counter = 0;
    set_server_api->interpose(
        [&](mongoc_client_pool_t* ptr, mongoc_server_api_t const* api, bson_error_t* error) -> bool {
            CHECK(ptr == mocks.pool_id);
            CHECK(api == server_api_id);
            CHECK(error != nullptr);
            ++set_counter;
            return true;
        });

    v1::server_api api{v1::server_api::version::k_version_1};

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

// Defer thorough test coverage to v1::client::options.
TEST_CASE("tls_opts", "[mongocxx][v1][pool]") {
#if MONGOCXX_SSL_IS_ENABLED()
    pool_mocks_type mocks;

    auto set_ssl_opts = libmongoc::client_pool_set_ssl_opts.create_instance();
    set_ssl_opts
        ->interpose([&](mongoc_client_pool_t* ptr, mongoc_ssl_opt_t const* opts) {
            CHECK(ptr == mocks.pool_id);
            CHECK(opts != nullptr);
            FAIL("should not reach this point");
        })
        .forever();

    auto const value = GENERATE(false, true);

    int set_counter = 0;
    set_ssl_opts->interpose([&](mongoc_client_pool_t* ptr, mongoc_ssl_opt_t const* opts) -> void {
        CHECK(ptr == mocks.pool_id);
        REQUIRE(opts != nullptr);
        CHECK(opts->weak_cert_validation == value);
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
        CHECK_NOTHROW(tls_opts.allow_invalid_certificates(std::move(value)));

        v1::client::options opts;
        CHECK_NOTHROW(opts.tls_opts(std::move(tls_opts)));
        (void)mocks.make(std::move(opts));
    }

    CHECK(set_counter == 1);
    CHECK(get_counter == 1);
#else
    SKIP(std::error_code{code::tls_not_supported}.message());
#endif
}

namespace {

struct entry_mocks_type {
    using client_destroy_type = decltype(libmongoc::client_destroy.create_instance());
    using push_type = decltype(libmongoc::client_pool_push.create_instance());

    client_destroy_type client_destroy = libmongoc::client_destroy.create_instance();
    push_type push = libmongoc::client_pool_push.create_instance();

    identity_type client_identity;
    mongoc_client_t* client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    int push_count = 0;

    explicit entry_mocks_type(pool_mocks_type& mocks) {
        mocks.pop
            ->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
                CHECK(ptr == mocks.pool_id);
                return client_id;
            })
            .forever();

        client_destroy
            ->interpose([&](mongoc_client_t* ptr) {
                CHECK(ptr == nullptr); // Always released back into mongoc_client_pool_t.
            })
            .forever();

        push->interpose([&](mongoc_client_pool_t* ptr, mongoc_client_t* client) -> void {
            CHECK(ptr == mocks.pool_id);
            CHECK(client == client_id);

            ++push_count;
        });
    }
};

} // namespace

TEST_CASE("acquire", "[mongocxx][v1][pool]") {
    pool_mocks_type mocks;

    auto pool = mocks.make();

    identity_type client_identity;
    auto const client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    auto client_destroy = libmongoc::client_destroy.create_instance();
    auto push = libmongoc::client_pool_push.create_instance();

    mocks.pop->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
        CHECK(ptr == mocks.pool_id);
        return client_id;
    });
    client_destroy
        ->interpose([&](mongoc_client_t* ptr) {
            CHECK(ptr == nullptr); // Always released back into mongoc_client_pool_t.
        })
        .forever();

    int push_count = 0;
    push->interpose([&](mongoc_client_pool_t* ptr, mongoc_client_t* client) -> void {
        CHECK(ptr == mocks.pool_id);
        CHECK(client == client_id);

        ++push_count;
    });

    {
        auto entry = pool.acquire();

        REQUIRE(entry);
        REQUIRE(*entry);
        CHECK(v1::client::internal::as_mongoc(*entry) == client_id);
        CHECK(v1::client::internal::as_mongoc(*entry.operator->()) == client_id);

        CHECK(push_count == 0);
    }

    CHECK(push_count == 1);
}

TEST_CASE("try_acquire", "[mongocxx][v1][pool]") {
    pool_mocks_type mocks;

    auto pool = mocks.make();

    identity_type client_identity;
    auto const client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    auto client_destroy = libmongoc::client_destroy.create_instance();
    auto push = libmongoc::client_pool_push.create_instance();

    client_destroy
        ->interpose([&](mongoc_client_t* ptr) {
            CHECK(ptr == nullptr); // Always released back into mongoc_client_pool_t.
        })
        .forever();

    int push_count = 0;
    push->interpose([&](mongoc_client_pool_t* ptr, mongoc_client_t* client) -> void {
        CHECK(ptr == mocks.pool_id);
        CHECK(client == client_id);

        ++push_count;
    });

    SECTION("none") {
        mocks.try_pop->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
            CHECK(ptr == mocks.pool_id);
            return nullptr;
        });

        {
            auto entry_opt = pool.try_acquire();

            CHECK_FALSE(entry_opt.has_value());
        }

        CHECK(push_count == 0);
    }

    SECTION("value") {
        mocks.try_pop->interpose([&](mongoc_client_pool_t* ptr) -> mongoc_client_t* {
            CHECK(ptr == mocks.pool_id);
            return client_id;
        });

        {
            auto entry_opt = pool.try_acquire();

            REQUIRE(entry_opt.has_value());

            auto& entry = *entry_opt;

            REQUIRE(entry);
            REQUIRE(*entry);
            CHECK(v1::client::internal::as_mongoc(*entry) == client_id);
            CHECK(v1::client::internal::as_mongoc(*entry.operator->()) == client_id);

            CHECK(push_count == 0);
        }

        CHECK(push_count == 1);
    }
}

TEST_CASE("release", "[mongocxx][v1][pool][entry]") {
    pool_mocks_type mocks;
    entry_mocks_type entry_mocks{mocks};

    auto pool = mocks.make();

    {
        auto entry = pool.acquire();

        REQUIRE(entry);
        REQUIRE(*entry);
        CHECK(v1::client::internal::as_mongoc(*entry) == entry_mocks.client_id);
        CHECK(v1::client::internal::as_mongoc(*entry.operator->()) == entry_mocks.client_id);

        CHECK(entry_mocks.push_count == 0);

        entry = nullptr;

        CHECK(entry_mocks.push_count == 1);
        CHECK_FALSE(entry);

        entry = nullptr;

        CHECK(entry_mocks.push_count == 1);
        CHECK_FALSE(entry);
    }

    CHECK(entry_mocks.push_count == 1);
}

TEST_CASE("database", "[mongocxx][v1][pool][entry]") {
    pool_mocks_type mocks;
    entry_mocks_type entry_mocks{mocks};

    auto database_destroy = libmongoc::database_destroy.create_instance();
    auto get_database = libmongoc::client_get_database.create_instance();

    identity_type database_identity;
    auto const database_id = reinterpret_cast<mongoc_database_t*>(&database_identity);

    auto const input = GENERATE("a", "b", "c");

    database_destroy->interpose([&](mongoc_database_t* ptr) -> void {
        if (ptr) {
            CHECK(ptr == database_id);
        }
    });

    get_database->interpose([&](mongoc_client_t* ptr, char const* name) -> mongoc_database_t* {
        CHECK(ptr == entry_mocks.client_id);
        CHECK_THAT(name, Catch::Matchers::Equals(input));
        return database_id;
    });

    auto pool = mocks.make();
    auto entry = pool.acquire();
    REQUIRE(entry);
    REQUIRE(*entry);
    auto const db = entry[input];

    CHECK(v1::database::internal::as_mongoc(db) == database_id);
}

} // namespace v1
} // namespace mongocxx
