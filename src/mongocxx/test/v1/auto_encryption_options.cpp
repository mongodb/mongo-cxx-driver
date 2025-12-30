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
// extra_optionsations under the License.

#include <mongocxx/v1/auto_encryption_options.hpp>

//

#include <mongocxx/v1/client-fwd.hpp>
#include <mongocxx/v1/pool-fwd.hpp>

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][auto_encryption_options]") {
    auto const source_value = scoped_bson{R"({"source": 1})"}.value();
    auto const target_value = scoped_bson{R"({"target": 2})"}.value();

    auto_encryption_options source;
    auto_encryption_options target;

    source.extra_options(source_value);
    target.extra_options(target_value);

    REQUIRE(source.extra_options() == source_value);
    REQUIRE(target.extra_options() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.extra_options() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.extra_options() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.extra_options() == source_value);
        CHECK(copy.extra_options() == source_value);

        target = copy;

        CHECK(copy.extra_options() == source_value);
        CHECK(target.extra_options() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][auto_encryption_options]") {
    auto_encryption_options const opts;

    CHECK(opts.key_vault_client() == nullptr);
    CHECK(opts.key_vault_pool() == nullptr);
    CHECK_FALSE(opts.key_vault_namespace().has_value());
    CHECK_FALSE(opts.kms_providers().has_value());
    CHECK_FALSE(opts.tls_opts().has_value());
    CHECK_FALSE(opts.schema_map().has_value());
    CHECK_FALSE(opts.encrypted_fields_map().has_value());
    CHECK_FALSE(opts.bypass_auto_encryption());
    CHECK_FALSE(opts.bypass_query_analysis());
    CHECK_FALSE(opts.extra_options().has_value());
}

TEST_CASE("key_vault_client", "[mongocxx][v1][auto_encryption_options]") {
    identity_type identity;

    auto const v = reinterpret_cast<v1::client*>(&identity);

    CHECK(auto_encryption_options{}.key_vault_client(v).key_vault_client() == v);
}

TEST_CASE("key_vault_pool", "[mongocxx][v1][auto_encryption_options]") {
    identity_type identity;

    auto const v = reinterpret_cast<v1::pool*>(&identity);

    CHECK(auto_encryption_options{}.key_vault_pool(v).key_vault_pool() == v);
}

TEST_CASE("key_vault_namespace", "[mongocxx][v1][auto_encryption_options]") {
    using T = auto_encryption_options::ns_pair;

    auto const v = GENERATE(values({
        T{"", ""},
        T{"x", "1"},
        T{"abc", "123"},
    }));

    CHECK(auto_encryption_options{}.key_vault_namespace(v).key_vault_namespace() == v);
}

TEST_CASE("kms_providers", "[mongocxx][v1][auto_encryption_options]") {
    using T = scoped_bson;

    auto const v = GENERATE(values({
        T{},
        T{R"({"x": 1})"},
    }));

    CHECK(auto_encryption_options{}.kms_providers(v.value()).kms_providers() == v.view());
}

TEST_CASE("tls_opts", "[mongocxx][v1][auto_encryption_options]") {
    using T = scoped_bson;

    auto const v = GENERATE(values({
        T{},
        T{R"({"x": 1})"},
    }));

    CHECK(auto_encryption_options{}.tls_opts(v.value()).tls_opts() == v.view());
}

TEST_CASE("schema_map", "[mongocxx][v1][auto_encryption_options]") {
    using T = scoped_bson;

    auto const v = GENERATE(values({
        T{},
        T{R"({"x": 1})"},
    }));

    CHECK(auto_encryption_options{}.schema_map(v.value()).schema_map() == v.view());
}

TEST_CASE("encrypted_fields_map", "[mongocxx][v1][auto_encryption_options]") {
    using T = scoped_bson;

    auto const v = GENERATE(values({
        T{},
        T{R"({"x": 1})"},
    }));

    CHECK(auto_encryption_options{}.encrypted_fields_map(v.value()).encrypted_fields_map() == v.view());
}

TEST_CASE("bypass_auto_encryption", "[mongocxx][v1][auto_encryption_options]") {
    auto const v = GENERATE(false, true);

    CHECK(auto_encryption_options{}.bypass_auto_encryption(v).bypass_auto_encryption() == v);
}

TEST_CASE("bypass_query_analysis", "[mongocxx][v1][auto_encryption_options]") {
    auto const v = GENERATE(false, true);

    CHECK(auto_encryption_options{}.bypass_query_analysis(v).bypass_query_analysis() == v);
}

TEST_CASE("extra_options", "[mongocxx][v1][auto_encryption_options]") {
    using T = scoped_bson;

    auto const v = GENERATE(values({
        T{},
        T{R"({"x": 1})"},
        T{R"({"x": 1, "y": 2})"},
    }));

    CHECK(auto_encryption_options{}.extra_options(v.value()).extra_options() == v.view());
}

} // namespace v1
} // namespace mongocxx
