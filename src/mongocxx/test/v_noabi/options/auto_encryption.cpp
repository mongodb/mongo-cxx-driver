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

#include <mongocxx/options/auto_encryption.hpp>

//

#include <mongocxx/v1/client-fwd.hpp>
#include <mongocxx/v1/pool-fwd.hpp>

#include <mongocxx/v1/detail/macros.hpp>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/pool-fwd.hpp>

#include <bsoncxx/document/view.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][auto_encryption]") {
    struct identity_type {};

    identity_type client_identity_v_noabi;
    identity_type pool_identity_v_noabi;
    identity_type client_identity_v1;
    identity_type pool_identity_v1;

    auto const client_id_v_noabi = reinterpret_cast<v_noabi::client*>(&client_identity_v_noabi);
    auto const pool_id_v_noabi = reinterpret_cast<v_noabi::pool*>(&pool_identity_v_noabi);
    auto const client_id_v1 = reinterpret_cast<v1::client*>(&client_identity_v1);
    auto const pool_id_v1 = reinterpret_cast<v1::pool*>(&pool_identity_v1);

    auto const bypass = GENERATE(false, true);
    auto const bypass_query_analysis = GENERATE(false, true);
    auto const has_value = GENERATE(false, true);

    // 0: none.
    // 1: client.
    // 2: pool.
    auto const with_ptrs = GENERATE(0, 1, 2);

    bsoncxx::v1::stdx::optional<v1::auto_encryption_options::ns_pair> key_vault_namespace;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> kms_providers;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> tls_opts;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> schema_map;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> encrypted_fields_map;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> extra_options;

    if (has_value) {
        key_vault_namespace.emplace();
        kms_providers.emplace();
        tls_opts.emplace();
        schema_map.emplace();
        encrypted_fields_map.emplace();
        extra_options.emplace();
    }

    using bsoncxx::v_noabi::from_v1;
    using mongocxx::v_noabi::from_v1;
    using mongocxx::v_noabi::to_v1;

    using v_noabi = v_noabi::options::auto_encryption;
    using v1 = v1::auto_encryption_options;

    SECTION("from_v1") {
        v1 from;

        from.bypass_auto_encryption(bypass);
        from.bypass_query_analysis(bypass_query_analysis);

        if (has_value) {
            from.key_vault_client(client_id_v1);
            from.key_vault_pool(pool_id_v1);
            from.key_vault_namespace(*key_vault_namespace);
            from.kms_providers(*kms_providers);
            from.tls_opts(*tls_opts);
            from.schema_map(*schema_map);
            from.encrypted_fields_map(*encrypted_fields_map);
            from.extra_options(*extra_options);
        }

        v_noabi const to = [&] {
            switch (with_ptrs) {
                case 0:
                    return from_v1(from);
                case 1:
                    return from_v1(from, client_id_v_noabi);
                case 2:
                    return from_v1(from, pool_id_v_noabi);
                default:
                    FAIL("should not reach this point");
                    MONGOCXX_PRIVATE_UNREACHABLE;
            }
        }();

        CHECK(to.bypass_auto_encryption() == bypass);
        CHECK(to.bypass_query_analysis() == bypass_query_analysis);

        switch (with_ptrs) {
            case 0: {
                CHECK_FALSE(to.key_vault_client().has_value());
                CHECK_FALSE(to.key_vault_pool().has_value());

            } break;

            case 1: {
                CHECK(to.key_vault_client() == client_id_v_noabi);
                CHECK_FALSE(to.key_vault_pool().has_value());

            } break;

            case 2: {
                CHECK_FALSE(to.key_vault_client().has_value());
                CHECK(to.key_vault_pool() == pool_id_v_noabi);

            } break;

            default: {
                FAIL("should not reach this point");
            } break;
        }

        if (has_value) {
            CHECK(to.key_vault_namespace() == key_vault_namespace);
            CHECK(to.kms_providers() == kms_providers->view());
            CHECK(to.tls_opts() == tls_opts->view());
            CHECK(to.schema_map() == schema_map->view());
            CHECK(to.encrypted_fields_map() == encrypted_fields_map->view());
            CHECK(to.extra_options() == extra_options->view());
        } else {
            CHECK_FALSE(to.key_vault_namespace().has_value());
            CHECK_FALSE(to.kms_providers().has_value());
            CHECK_FALSE(to.tls_opts().has_value());
            CHECK_FALSE(to.schema_map().has_value());
            CHECK_FALSE(to.encrypted_fields_map().has_value());
            CHECK_FALSE(to.extra_options().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        from.bypass_auto_encryption(bypass);
        from.bypass_query_analysis(bypass_query_analysis);

        if (has_value) {
            from.key_vault_client(client_id_v_noabi);
            from.key_vault_pool(pool_id_v_noabi);
            from.key_vault_namespace(*key_vault_namespace);
            from.kms_providers(from_v1(kms_providers->view()));
            from.tls_opts(from_v1(tls_opts->view()));
            from.schema_map(from_v1(schema_map->view()));
            from.encrypted_fields_map(from_v1(encrypted_fields_map->view()));
            from.extra_options(from_v1(extra_options->view()));
        }

        v1 const to = [&] {
            switch (with_ptrs) {
                case 0:
                    return to_v1(from);
                case 1:
                    return to_v1(from, client_id_v1);
                case 2:
                    return to_v1(from, pool_id_v1);
                default:
                    FAIL("should not reach this point");
                    MONGOCXX_PRIVATE_UNREACHABLE;
            }
        }();

        CHECK(to.bypass_auto_encryption() == bypass);
        CHECK(to.bypass_query_analysis() == bypass_query_analysis);

        switch (with_ptrs) {
            case 0: {
                CHECK(to.key_vault_client() == nullptr);
                CHECK(to.key_vault_pool() == nullptr);

            } break;

            case 1: {
                CHECK(to.key_vault_client() == client_id_v1);
                CHECK(to.key_vault_pool() == nullptr);

            } break;

            case 2: {
                CHECK(to.key_vault_client() == nullptr);
                CHECK(to.key_vault_pool() == pool_id_v1);

            } break;

            default: {
                FAIL("should not reach this point");
            } break;
        }

        if (has_value) {
            CHECK(to.key_vault_namespace() == key_vault_namespace);
            CHECK(to.kms_providers() == kms_providers->view());
            CHECK(to.tls_opts() == tls_opts->view());
            CHECK(to.schema_map() == schema_map->view());
            CHECK(to.encrypted_fields_map() == encrypted_fields_map->view());
            CHECK(to.extra_options() == extra_options->view());
        } else {
            CHECK_FALSE(to.key_vault_namespace().has_value());
            CHECK_FALSE(to.kms_providers().has_value());
            CHECK_FALSE(to.tls_opts().has_value());
            CHECK_FALSE(to.schema_map().has_value());
            CHECK_FALSE(to.encrypted_fields_map().has_value());
            CHECK_FALSE(to.extra_options().has_value());
        }
    }
}

} // namespace mongocxx
