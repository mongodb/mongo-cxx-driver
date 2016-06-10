// Copyright 2015 MongoDB Inc.
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

#include "catch.hpp"
#include "helpers.hpp"

#include <cstddef>
#include <string>

#include <mongocxx/instance.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/options/ssl.hpp>
#include <mongocxx/pool.hpp>

using namespace mongocxx;

TEST_CASE("a pool is created with the correct MongoDB URI", "[pool]") {
    MOCK_POOL

    instance::current();

    bool destroy_called = false;
    client_pool_destroy->interpose([&](::mongoc_client_pool_t*) { destroy_called = true; });

    std::string expected_uri("mongodb://mongodb.example.com:9999");
    uri mongodb_uri{expected_uri};

    std::string actual_uri{};
    bool new_called = false;

    client_pool_new->interpose([&](const mongoc_uri_t* uri) {
        new_called = true;
        actual_uri = mongoc_uri_get_string(uri);
        return nullptr;
    });

    {
        pool p{mongodb_uri};

        REQUIRE(new_called);
        REQUIRE(expected_uri == actual_uri);

        REQUIRE(!destroy_called);
    }

    REQUIRE(destroy_called);
}

#if defined(MONGOC_ENABLE_SSL)
TEST_CASE(
    "If we pass an engaged SSL options struct to the pool class, we will use it to configure the "
    "underlying mongoc pool",
    "[pool]") {
    MOCK_POOL

    instance::current();

    const std::string pem_file = "foo";
    const std::string pem_password = "bar";
    const std::string ca_file = "baz";
    const std::string ca_dir = "garply";
    const std::string crl_file = "crl_file";
    const bool allow_invalid_certificates = true;

    bool set_ssl_opts_called = false;
    options::ssl ssl_opts;
    ssl_opts.pem_file(pem_file);
    ssl_opts.pem_password(pem_password);
    ssl_opts.ca_file(ca_file);
    ssl_opts.ca_dir(ca_dir);
    ssl_opts.crl_file(crl_file);
    ssl_opts.allow_invalid_certificates(allow_invalid_certificates);

    ::mongoc_ssl_opt_t interposed = {0};

    client_pool_set_ssl_opts->interpose(
        [&](::mongoc_client_pool_t*, const ::mongoc_ssl_opt_t* opts) {
            set_ssl_opts_called = true;
            interposed = *opts;
        });

    pool p{uri{"mongodb://mongodb.example.com:9999?ssl=true"}, std::move(ssl_opts)};

    REQUIRE(set_ssl_opts_called);
    REQUIRE(interposed.pem_file == pem_file);
    REQUIRE(interposed.pem_pwd == pem_password);
    REQUIRE(interposed.ca_file == ca_file);
    REQUIRE(interposed.ca_dir == ca_dir);
    REQUIRE(interposed.crl_file == crl_file);
    REQUIRE(interposed.weak_cert_validation == allow_invalid_certificates);
}
#endif

TEST_CASE(
    "calling acquire on a pool returns a entry that is released when it goes out of "
    "scope",
    "[pool]") {
    MOCK_POOL

    instance::current();

    bool pop_called = false;
    client_pool_pop->interpose([&](::mongoc_client_pool_t*) {
        pop_called = true;
        return nullptr;
    });

    bool push_called = false;
    client_pool_push->interpose(
        [&](::mongoc_client_pool_t*, ::mongoc_client_t*) { push_called = true; });

    {
        pool p{};
        auto client = p.acquire();

        REQUIRE(pop_called);
        REQUIRE(!push_called);
    }

    REQUIRE(push_called);
}

TEST_CASE(
    "try_acquire returns an engaged stdx::optional<entry> if mongoc_client_pool_try_pop "
    "returns a non-null pointer",
    "[pool]") {
    MOCK_POOL

    instance::current();

    // libstdc++ before GCC 4.9 places max_align_t in the wrong
    // namespace. Use a limited scope 'using namespace std' to name it
    // in a way that always works.
    auto dummy_address = []() {
        using namespace std;
        return max_align_t{};
    }();

    bool try_pop_called = false;

    mongoc_client_t* fake = reinterpret_cast<mongoc_client_t*>(&dummy_address);

    client_pool_try_pop->interpose([&](::mongoc_client_pool_t*) {
        try_pop_called = true;
        return fake;
    });

    {
        pool p{};
        auto client = p.try_acquire();

        REQUIRE(!!client);
        REQUIRE(try_pop_called);
    }
}

TEST_CASE(
    "try_acquire returns a disengaged stdx::optional<entry> if mongoc_client_pool_try_pop "
    "returns a null pointer",
    "[pool]") {
    MOCK_POOL

    instance::current();

    client_pool_try_pop->interpose([](::mongoc_client_pool_t*) { return nullptr; });

    {
        pool p{};
        auto client = p.try_acquire();
        REQUIRE(!client);
    }
}
