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

#include <mongocxx/v1/string_options.hpp>

//

#include <cstdint>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][string_options]") {
    string_options source;
    string_options target;

    auto const source_value = false;
    auto const target_value = true;

    source.case_sensitive(source_value);
    target.case_sensitive(target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.case_sensitive() == source_value);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.case_sensitive() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.case_sensitive() == source_value);
        CHECK(copy.case_sensitive() == source_value);

        target = copy;

        CHECK(copy.case_sensitive() == source_value);
        CHECK(target.case_sensitive() == source_value);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][string_options][prefix]") {
    string_options::prefix source;
    string_options::prefix target;

    auto const source_value = std::int32_t{123};
    auto const target_value = std::int32_t{456};

    source.str_max_query_length(source_value);
    target.str_max_query_length(target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.str_max_query_length() == source_value);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.str_max_query_length() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.str_max_query_length() == source_value);
        CHECK(copy.str_max_query_length() == source_value);

        target = copy;

        CHECK(copy.str_max_query_length() == source_value);
        CHECK(target.str_max_query_length() == source_value);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][string_options][suffix]") {
    string_options::suffix source;
    string_options::suffix target;

    auto const source_value = std::int32_t{123};
    auto const target_value = std::int32_t{456};

    source.str_max_query_length(source_value);
    target.str_max_query_length(target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.str_max_query_length() == source_value);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.str_max_query_length() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.str_max_query_length() == source_value);
        CHECK(copy.str_max_query_length() == source_value);

        target = copy;

        CHECK(copy.str_max_query_length() == source_value);
        CHECK(target.str_max_query_length() == source_value);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][string_options][substring]") {
    string_options::substring source;
    string_options::substring target;

    auto const source_value = std::int32_t{123};
    auto const target_value = std::int32_t{456};

    source.str_max_length(source_value);
    target.str_max_length(target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.str_max_length() == source_value);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.str_max_length() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.str_max_length() == source_value);
        CHECK(copy.str_max_length() == source_value);

        target = copy;

        CHECK(copy.str_max_length() == source_value);
        CHECK(target.str_max_length() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][string_options]") {
    string_options const opts;

    CHECK_FALSE(opts.case_sensitive().has_value());
    CHECK_FALSE(opts.diacritic_sensitive().has_value());
    CHECK_FALSE(opts.prefix_opts().has_value());
    CHECK_FALSE(opts.suffix_opts().has_value());
    CHECK_FALSE(opts.substring_opts().has_value());
}

TEST_CASE("default", "[mongocxx][v1][string_options][prefix]") {
    string_options::prefix const opts;

    CHECK_FALSE(opts.str_max_query_length().has_value());
    CHECK_FALSE(opts.str_min_query_length().has_value());
}

TEST_CASE("default", "[mongocxx][v1][string_options][suffix]") {
    string_options::suffix const opts;

    CHECK_FALSE(opts.str_max_query_length().has_value());
    CHECK_FALSE(opts.str_min_query_length().has_value());
}

TEST_CASE("default", "[mongocxx][v1][string_options][substring]") {
    string_options::substring const opts;

    CHECK_FALSE(opts.str_max_length().has_value());
    CHECK_FALSE(opts.str_max_query_length().has_value());
    CHECK_FALSE(opts.str_min_query_length().has_value());
}

TEST_CASE("case_sensitive", "[mongocxx][v1][string_options]") {
    auto const v = GENERATE(false, true);

    CHECK(string_options{}.case_sensitive(v).case_sensitive() == v);
}

TEST_CASE("diacritic_sensitive", "[mongocxx][v1][string_options]") {
    auto const v = GENERATE(false, true);

    CHECK(string_options{}.diacritic_sensitive(v).diacritic_sensitive() == v);
}

TEST_CASE("prefix_opts", "[mongocxx][v1][string_options]") {
    using T = string_options::prefix;

    auto const v = GENERATE(values({
        T{},
        T{}.str_max_query_length(123),
    }));

    CHECK(string_options{}.prefix_opts(v).prefix_opts().value().str_max_query_length() == v.str_max_query_length());
}

TEST_CASE("suffix_opts", "[mongocxx][v1][string_options]") {
    using T = string_options::suffix;

    auto const v = GENERATE(values({
        T{},
        T{}.str_max_query_length(123),
    }));

    CHECK(string_options{}.suffix_opts(v).suffix_opts().value().str_max_query_length() == v.str_max_query_length());
}

TEST_CASE("substring_opts", "[mongocxx][v1][string_options]") {
    using T = string_options::substring;

    auto const v = GENERATE(values({
        T{},
        T{}.str_max_length(123),
    }));

    CHECK(string_options{}.substring_opts(v).substring_opts().value().str_max_length() == v.str_max_length());
}

TEST_CASE("str_max_query_length", "[mongocxx][v1][string_options][prefix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::prefix{}.str_max_query_length(v).str_max_query_length() == v);
}

TEST_CASE("str_min_query_length", "[mongocxx][v1][string_options][prefix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::prefix{}.str_min_query_length(v).str_min_query_length() == v);
}

TEST_CASE("str_max_query_length", "[mongocxx][v1][string_options][suffix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::suffix{}.str_max_query_length(v).str_max_query_length() == v);
}

TEST_CASE("str_min_query_length", "[mongocxx][v1][string_options][suffix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::suffix{}.str_min_query_length(v).str_min_query_length() == v);
}

TEST_CASE("str_max_length", "[mongocxx][v1][string_options][substring]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::substring{}.str_max_length(v).str_max_length() == v);
}

TEST_CASE("str_max_query_length", "[mongocxx][v1][string_options][substring]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::substring{}.str_max_query_length(v).str_max_query_length() == v);
}

TEST_CASE("str_min_query_length", "[mongocxx][v1][string_options][substring]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(string_options::substring{}.str_min_query_length(v).str_min_query_length() == v);
}

} // namespace v1
} // namespace mongocxx
