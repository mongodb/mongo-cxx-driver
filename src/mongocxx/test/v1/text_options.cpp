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

#include <mongocxx/v1/text_options.hh>

//

#include <mongocxx/v1/string_options.hpp>

#include <cstdint>
#include <utility>

#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][text_options]") {
    text_options source;
    text_options target;

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

TEST_CASE("ownership", "[mongocxx][v1][text_options][prefix]") {
    text_options::prefix source;
    text_options::prefix target;

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

TEST_CASE("ownership", "[mongocxx][v1][text_options][suffix]") {
    text_options::suffix source;
    text_options::suffix target;

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

TEST_CASE("ownership", "[mongocxx][v1][text_options][substring]") {
    text_options::substring source;
    text_options::substring target;

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

TEST_CASE("default", "[mongocxx][v1][text_options]") {
    text_options const opts;

    CHECK_FALSE(opts.case_sensitive().has_value());
    CHECK_FALSE(opts.diacritic_sensitive().has_value());
    CHECK_FALSE(opts.prefix_opts().has_value());
    CHECK_FALSE(opts.suffix_opts().has_value());
    CHECK_FALSE(opts.substring_opts().has_value());
}

TEST_CASE("default", "[mongocxx][v1][text_options][prefix]") {
    text_options::prefix const opts;

    CHECK_FALSE(opts.str_max_query_length().has_value());
    CHECK_FALSE(opts.str_min_query_length().has_value());
}

TEST_CASE("default", "[mongocxx][v1][text_options][suffix]") {
    text_options::suffix const opts;

    CHECK_FALSE(opts.str_max_query_length().has_value());
    CHECK_FALSE(opts.str_min_query_length().has_value());
}

TEST_CASE("default", "[mongocxx][v1][text_options][substring]") {
    text_options::substring const opts;

    CHECK_FALSE(opts.str_max_length().has_value());
    CHECK_FALSE(opts.str_max_query_length().has_value());
    CHECK_FALSE(opts.str_min_query_length().has_value());
}

TEST_CASE("case_sensitive", "[mongocxx][v1][text_options]") {
    auto const v = GENERATE(false, true);

    CHECK(text_options{}.case_sensitive(v).case_sensitive() == v);
}

TEST_CASE("diacritic_sensitive", "[mongocxx][v1][text_options]") {
    auto const v = GENERATE(false, true);

    CHECK(text_options{}.diacritic_sensitive(v).diacritic_sensitive() == v);
}

TEST_CASE("prefix_opts", "[mongocxx][v1][text_options]") {
    using T = text_options::prefix;

    auto const v = GENERATE(values({
        T{},
        T{}.str_max_query_length(123),
    }));

    CHECK(text_options{}.prefix_opts(v).prefix_opts().value().str_max_query_length() == v.str_max_query_length());
}

TEST_CASE("suffix_opts", "[mongocxx][v1][text_options]") {
    using T = text_options::suffix;

    auto const v = GENERATE(values({
        T{},
        T{}.str_max_query_length(123),
    }));

    CHECK(text_options{}.suffix_opts(v).suffix_opts().value().str_max_query_length() == v.str_max_query_length());
}

TEST_CASE("substring_opts", "[mongocxx][v1][text_options]") {
    using T = text_options::substring;

    auto const v = GENERATE(values({
        T{},
        T{}.str_max_length(123),
    }));

    CHECK(text_options{}.substring_opts(v).substring_opts().value().str_max_length() == v.str_max_length());
}

TEST_CASE("str_max_query_length", "[mongocxx][v1][text_options][prefix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::prefix{}.str_max_query_length(v).str_max_query_length() == v);
}

TEST_CASE("str_min_query_length", "[mongocxx][v1][text_options][prefix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::prefix{}.str_min_query_length(v).str_min_query_length() == v);
}

TEST_CASE("str_max_query_length", "[mongocxx][v1][text_options][suffix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::suffix{}.str_max_query_length(v).str_max_query_length() == v);
}

TEST_CASE("str_min_query_length", "[mongocxx][v1][text_options][suffix]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::suffix{}.str_min_query_length(v).str_min_query_length() == v);
}

TEST_CASE("str_max_length", "[mongocxx][v1][text_options][substring]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::substring{}.str_max_length(v).str_max_length() == v);
}

TEST_CASE("str_max_query_length", "[mongocxx][v1][text_options][substring]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::substring{}.str_max_query_length(v).str_max_query_length() == v);
}

TEST_CASE("str_min_query_length", "[mongocxx][v1][text_options][substring]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            INT32_MIN + 1,
            0,
            INT32_MAX - 1,
            INT32_MAX,
        }));

    CHECK(text_options::substring{}.str_min_query_length(v).str_min_query_length() == v);
}

// `to_string_options` is how the deprecated "textOpts" field reaches libmongocrypt as a "stringOpts"
// field. It is otherwise only reachable through `client_encryption`, which requires a live server.
TEST_CASE("to_string_options", "[mongocxx][v1][text_options]") {
    auto const convert = [](text_options const& opts) { return text_options::internal::to_string_options(opts); };

    SECTION("an empty text_options converts to an empty string_options") {
        auto const to = convert(text_options{});

        CHECK_FALSE(to.case_sensitive().has_value());
        CHECK_FALSE(to.diacritic_sensitive().has_value());
        CHECK_FALSE(to.prefix_opts().has_value());
        CHECK_FALSE(to.suffix_opts().has_value());
        CHECK_FALSE(to.substring_opts().has_value());
    }

    SECTION("case_sensitive and diacritic_sensitive") {
        auto const v = GENERATE(false, true);
        auto const to = convert(text_options{}.case_sensitive(v).diacritic_sensitive(!v));

        CHECK(to.case_sensitive() == v);
        CHECK(to.diacritic_sensitive() == !v);
    }

    SECTION("prefix_opts") {
        auto const to = convert(
            text_options{}.prefix_opts(text_options::prefix{}.str_max_query_length(10).str_min_query_length(2)));

        REQUIRE(to.prefix_opts());
        CHECK(to.prefix_opts()->str_max_query_length() == 10);
        CHECK(to.prefix_opts()->str_min_query_length() == 2);
        CHECK_FALSE(to.suffix_opts().has_value());
        CHECK_FALSE(to.substring_opts().has_value());
    }

    SECTION("suffix_opts") {
        auto const to = convert(
            text_options{}.suffix_opts(text_options::suffix{}.str_max_query_length(11).str_min_query_length(3)));

        REQUIRE(to.suffix_opts());
        CHECK(to.suffix_opts()->str_max_query_length() == 11);
        CHECK(to.suffix_opts()->str_min_query_length() == 3);
        CHECK_FALSE(to.prefix_opts().has_value());
        CHECK_FALSE(to.substring_opts().has_value());
    }

    SECTION("substring_opts") {
        auto const to = convert(
            text_options{}.substring_opts(
                text_options::substring{}.str_max_length(20).str_max_query_length(6).str_min_query_length(4)));

        REQUIRE(to.substring_opts());
        CHECK(to.substring_opts()->str_max_length() == 20);
        CHECK(to.substring_opts()->str_max_query_length() == 6);
        CHECK(to.substring_opts()->str_min_query_length() == 4);
        CHECK_FALSE(to.prefix_opts().has_value());
        CHECK_FALSE(to.suffix_opts().has_value());
    }

    SECTION("every field at once") {
        auto const to = convert(
            text_options{}
                .case_sensitive(true)
                .diacritic_sensitive(true)
                .prefix_opts(text_options::prefix{}.str_max_query_length(10).str_min_query_length(2))
                .suffix_opts(text_options::suffix{}.str_max_query_length(11).str_min_query_length(3))
                .substring_opts(
                    text_options::substring{}.str_max_length(20).str_max_query_length(6).str_min_query_length(4)));

        CHECK(to.case_sensitive() == true);
        CHECK(to.diacritic_sensitive() == true);
        REQUIRE(to.prefix_opts());
        CHECK(to.prefix_opts()->str_max_query_length() == 10);
        CHECK(to.prefix_opts()->str_min_query_length() == 2);
        REQUIRE(to.suffix_opts());
        CHECK(to.suffix_opts()->str_max_query_length() == 11);
        CHECK(to.suffix_opts()->str_min_query_length() == 3);
        REQUIRE(to.substring_opts());
        CHECK(to.substring_opts()->str_max_length() == 20);
        CHECK(to.substring_opts()->str_max_query_length() == 6);
        CHECK(to.substring_opts()->str_min_query_length() == 4);
    }

    SECTION("a partially set nested field leaves the rest unset") {
        auto const to = convert(text_options{}.prefix_opts(text_options::prefix{}.str_min_query_length(2)));

        REQUIRE(to.prefix_opts());
        CHECK_FALSE(to.prefix_opts()->str_max_query_length().has_value());
        CHECK(to.prefix_opts()->str_min_query_length() == 2);
    }
}

} // namespace v1
} // namespace mongocxx

BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END
