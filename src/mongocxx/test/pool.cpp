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

#include "helpers.hpp"

#include <cstddef>
#include <string>

#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/ssl.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/private/libmongoc.hh>

namespace {
using namespace mongocxx;

TEST_CASE("a pool is created with the correct MongoDB URI", "[pool]") {
    instance::current();

    std::string expected_uri("mongodb://localhost/");
    uri mongodb_uri{expected_uri};

    pool pool{mongodb_uri};

    REQUIRE(pool.acquire()->uri().to_string() == expected_uri);
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

    ::mongoc_ssl_opt_t interposed = {};

    client_pool_set_ssl_opts->interpose(
        [&](::mongoc_client_pool_t*, const ::mongoc_ssl_opt_t* opts) {
            set_ssl_opts_called = true;
            interposed = *opts;
        });

    pool p{uri{"mongodb://mongodb.example.com:9999/?ssl=true"},
           options::client().ssl_opts(ssl_opts)};

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
    client_pool_pop->interpose([&](::mongoc_client_pool_t* pool) {
        pop_called = true;
        return libmongoc::client_pool_pop(pool);
    });

    bool push_called = false;
    client_pool_push->interpose([&](::mongoc_client_pool_t* pool, ::mongoc_client_t* client) {
        push_called = true;
        libmongoc::client_pool_push(pool, client);
    });

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
    instance::current();

    pool p{uri{}};
    auto client = p.try_acquire();

    REQUIRE(!!client);
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
}  // namespace
