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
// key_alt_nameations under the License.

#include <mongocxx/v1/encrypt_options.hpp>

//

#include <mongocxx/v1/range_options.hpp>
#include <mongocxx/v1/text_options.hpp>

#include <bsoncxx/test/v1/types/value.hh>

#include <cstdint>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][encrypt_options]") {
    std::string const source_value = "source";
    std::string const target_value = "target";

    encrypt_options source;
    encrypt_options target;

    source.key_alt_name(source_value);
    target.key_alt_name(target_value);

    REQUIRE(source.key_alt_name() == source_value);
    REQUIRE(target.key_alt_name() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.key_alt_name() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.key_alt_name() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.key_alt_name() == source_value);
        CHECK(copy.key_alt_name() == source_value);

        target = copy;

        CHECK(copy.key_alt_name() == source_value);
        CHECK(target.key_alt_name() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][encrypt_options]") {
    encrypt_options const opts;

    CHECK_FALSE(opts.key_id().has_value());
    CHECK_FALSE(opts.key_alt_name().has_value());
    CHECK_FALSE(opts.algorithm().has_value());
    CHECK_FALSE(opts.contention_factor().has_value());
    CHECK_FALSE(opts.query_type().has_value());
    CHECK_FALSE(opts.range_opts().has_value());
    CHECK_FALSE(opts.text_opts().has_value());
}

TEST_CASE("key_id", "[mongocxx][v1][encrypt_options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(encrypt_options{}.key_id(v).key_id() == v);
}

TEST_CASE("key_alt_name", "[mongocxx][v1][encrypt_options]") {
    auto const v = GENERATE(
        values<std::string>({
            {},
            "",
            "abc",
            "pem_file",
        }));

    CHECK(encrypt_options{}.key_alt_name(v).key_alt_name() == v);
}

TEST_CASE("algorithm", "[mongocxx][v1][encrypt_options]") {
    using T = encrypt_options::encryption_algorithm;

    auto const v = GENERATE(values({
        T::k_deterministic,
        T::k_random,
        T::k_unindexed,
        T::k_range,
        T::k_textPreview,
    }));

    CHECK(encrypt_options{}.algorithm(v).algorithm() == v);
}

TEST_CASE("contention_factor", "[mongocxx][v1][encrypt_options]") {
    auto const v = GENERATE(values({
        std::int64_t{INT64_MIN},
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::int64_t{INT64_MAX},
    }));

    CHECK(encrypt_options{}.contention_factor(v).contention_factor() == v);
}

TEST_CASE("query_type", "[mongocxx][v1][encrypt_options]") {
    using T = encrypt_options::encryption_query_type;

    auto const v = GENERATE(values({
        T::k_equality,
        T::k_range,
        T::k_prefixPreview,
        T::k_suffixPreview,
        T::k_substringPreview,
    }));

    CHECK(encrypt_options{}.query_type(v).query_type() == v);
}

TEST_CASE("range_opts", "[mongocxx][v1][encrypt_options]") {
    using T = v1::range_options;

    auto const v = GENERATE(
        values<T>({
            {},
            T{}.precision(123),
        }));

    CHECK(encrypt_options{}.range_opts(v).range_opts().value().precision() == v.precision());
}

TEST_CASE("text_opts", "[mongocxx][v1][encrypt_options]") {
    using T = v1::text_options;

    auto const v = GENERATE(values({
        T{},
        T{}.case_sensitive(false),
        T{}.case_sensitive(true),
    }));

    CHECK(encrypt_options{}.text_opts(v).text_opts().value().case_sensitive() == v.case_sensitive());
}

} // namespace v1
} // namespace mongocxx
