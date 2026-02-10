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

#include <mongocxx/options/client_encryption.hpp>

//

#include <mongocxx/v1/client-fwd.hpp>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/client-fwd.hpp>

#include <bsoncxx/document/view.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][client_encryption]") {
    struct identity_type {};

    identity_type client_identity_v_noabi;
    identity_type client_identity_v1;

    auto const client_id_v_noabi = reinterpret_cast<v_noabi::client*>(&client_identity_v_noabi);
    auto const client_id_v1 = reinterpret_cast<v1::client*>(&client_identity_v1);

    auto const has_value = GENERATE(false, true);
    auto const with_client = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<v1::client_encryption::options::ns_pair> key_vault_namespace;
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

    using v_noabi = v_noabi::options::client_encryption;
    using v1 = v1::client_encryption::options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.key_vault_client(client_id_v1);
            from.key_vault_namespace(*key_vault_namespace);
            from.kms_providers(*kms_providers);
            from.tls_opts(*tls_opts);
        }

        v_noabi const to = with_client ? from_v1(from, client_id_v_noabi) : from_v1(from);

        if (with_client) {
            CHECK(to.key_vault_client() == client_id_v_noabi);
        } else {
            CHECK_FALSE(to.key_vault_client().has_value());
        }

        if (has_value) {
            CHECK(to.key_vault_namespace() == key_vault_namespace);
            CHECK(to.kms_providers() == kms_providers->view());
            CHECK(to.tls_opts() == tls_opts->view());
        } else {
            CHECK_FALSE(to.key_vault_namespace().has_value());
            CHECK_FALSE(to.kms_providers().has_value());
            CHECK_FALSE(to.tls_opts().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.key_vault_client(client_id_v_noabi);
            from.key_vault_namespace(*key_vault_namespace);
            from.kms_providers(from_v1(kms_providers->view()));
            from.tls_opts(from_v1(tls_opts->view()));
        }

        v1 const to = with_client ? to_v1(from, client_id_v1) : to_v1(from);

        if (with_client) {
            CHECK(to.key_vault_client() == client_id_v1);
        } else {
            CHECK(to.key_vault_client() == nullptr);
        }

        if (has_value) {
            CHECK(to.key_vault_namespace() == key_vault_namespace);
            CHECK(to.kms_providers() == kms_providers->view());
            CHECK(to.tls_opts() == tls_opts->view());
        } else {
            CHECK_FALSE(to.key_vault_namespace().has_value());
            CHECK_FALSE(to.kms_providers().has_value());
            CHECK_FALSE(to.tls_opts().has_value());
        }
    }
}

} // namespace mongocxx
