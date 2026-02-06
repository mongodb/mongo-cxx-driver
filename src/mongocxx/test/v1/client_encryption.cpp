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
// key_vault_clientations under the License.

#include <mongocxx/v1/client_encryption.hh>

//

#include <mongocxx/v1/client.hpp>

#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/exception.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][client_encryption]") {
    identity_type id1;
    identity_type id2;

    auto const ce1 = reinterpret_cast<mongoc_client_encryption_t*>(&id1);
    auto const ce2 = reinterpret_cast<mongoc_client_encryption_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::client_encryption_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_client_encryption_t* ptr) -> void {
            if (ptr) {
                if (ptr != ce1 && ptr != ce2) {
                    FAIL_CHECK("unexpected mongoc_client_encryption_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    auto source = client_encryption::internal::make(ce1);
    auto target = client_encryption::internal::make(ce2);

    REQUIRE(client_encryption::internal::as_mongoc(source) == ce1);
    REQUIRE(client_encryption::internal::as_mongoc(target) == ce2);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.

            CHECK(client_encryption::internal::as_mongoc(move) == ce1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.

            CHECK(client_encryption::internal::as_mongoc(target) == ce1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][client_encryption][options]") {
    identity_type source_identity;
    identity_type target_identity;

    auto const source_value = reinterpret_cast<v1::client*>(&source_identity);
    auto const target_value = reinterpret_cast<v1::client*>(&target_identity);

    v1::client_encryption::options source;
    v1::client_encryption::options target;

    source.key_vault_client(source_value);
    target.key_vault_client(target_value);

    REQUIRE(source.key_vault_client() == source_value);
    REQUIRE(target.key_vault_client() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.key_vault_client() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.key_vault_client() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.key_vault_client() == source_value);
        CHECK(copy.key_vault_client() == source_value);

        target = copy;

        CHECK(copy.key_vault_client() == source_value);
        CHECK(target.key_vault_client() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_encryption][options]") {
    client_encryption::options const opts;

    CHECK(opts.key_vault_client() == nullptr);
    CHECK_FALSE(opts.key_vault_namespace().has_value());
    CHECK_FALSE(opts.kms_providers().has_value());
    CHECK_FALSE(opts.tls_opts().has_value());
}

TEST_CASE("key_vault_client", "[mongocxx][v1][client_encryption][options]") {
    identity_type id1;
    identity_type id2;

    auto const client1 = reinterpret_cast<v1::client*>(&id1);
    auto const client2 = reinterpret_cast<v1::client*>(&id2);

    auto const v = GENERATE(false, true) ? client1 : client2;

    CHECK(client_encryption::options{}.key_vault_client(v).key_vault_client() == v);
}

TEST_CASE("key_vault_namespace", "[mongocxx][v1][client_encryption][options]") {
    auto const v = GENERATE(
        values<client_encryption::options::ns_pair>({
            {},
            {"db", "coll"},
        }));

    CHECK(client_encryption::options{}.key_vault_namespace(v).key_vault_namespace() == v);
}

TEST_CASE("kms_providers", "[mongocxx][v1][client_encryption][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"});

    CHECK(client_encryption::options{}.kms_providers(v.value()).kms_providers() == v.view());
}

TEST_CASE("tls_opts", "[mongocxx][v1][client_encryption][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"});

    CHECK(client_encryption::options{}.tls_opts(v.value()).tls_opts() == v.view());
}

} // namespace v1
} // namespace mongocxx
