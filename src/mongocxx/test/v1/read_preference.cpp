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

#include <mongocxx/test/v1/read_preference.hh>

//

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <chrono>
#include <tuple>
#include <utility>

#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][read_preference]") {
    using mode = read_preference::read_mode;

    read_preference source;
    read_preference target;

    source.mode(mode::k_primary_preferred);
    target.mode(mode::k_secondary_preferred);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.mode() == mode::k_primary_preferred);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.mode() == mode::k_primary_preferred);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.mode() == mode::k_primary_preferred);
        CHECK(copy.mode() == mode::k_primary_preferred);

        target = copy;

        CHECK(copy.mode() == mode::k_primary_preferred);
        CHECK(target.mode() == mode::k_primary_preferred);
    }
}

TEST_CASE("default", "[mongocxx][v1][read_preference]") {
    using mode = read_preference::read_mode;

    read_preference const rp;

    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
    CHECK_FALSE(rp.hedge().has_value());
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END

    CHECK(rp.mode() == mode::k_primary);
    CHECK(rp.tags().empty());
    CHECK_FALSE(rp.max_staleness().has_value());
}

TEST_CASE("hedge", "[mongocxx][v1][read_preference]") {
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN

    read_preference rp;

    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({})"},
        scoped_bson{R"({"enabled": false})"},
        scoped_bson{R"({"enabled": true})"},
    }));

    CHECK_NOTHROW(rp.hedge(v.view()));

    CHECKED_IF(v.view().empty()) {
        CHECK_FALSE(rp.hedge().has_value());
    }

    else {
        CHECK(rp.hedge() == v.view());
    }

    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END
}

TEST_CASE("mode", "[mongocxx][v1][read_preference]") {
    using mode = read_preference::read_mode;

    mode input = {};
    mode expected = {};

    std::tie(input, expected) = GENERATE(
        table<mode, mode>({
            {mode::k_primary, mode::k_primary},
            {mode::k_primary_preferred, mode::k_primary_preferred},
            {mode::k_secondary, mode::k_secondary},
            {mode::k_secondary_preferred, mode::k_secondary_preferred},
            {mode::k_nearest, mode::k_nearest},
            {static_cast<mode>(-1), mode::k_primary}, // Undocumented behavior.
            {static_cast<mode>(5), mode::k_primary},  // Undocumented behavior.
        }));

    CAPTURE(input);

    read_preference rp;
    CHECK_NOTHROW(rp.mode(input));
    CHECK(rp.mode() == expected);
}

TEST_CASE("tags", "[mongocxx][v1][read_preference]") {
    auto const v = GENERATE(values({
        scoped_bson{R"([])"},
        scoped_bson{R"(["x"])"},
        scoped_bson{R"(["a", "b", "c"])"},
    }));

    CAPTURE(v.view());

    read_preference rp;

    CHECK_NOTHROW(rp.tags(v.view()));
    CHECK(rp.tags() == v.array_view());

    CHECK_NOTHROW(rp.tags(v.array_view()));
    CHECK(rp.tags() == v.array_view());
}

TEST_CASE("max_staleness", "[mongocxx][v1][read_preference]") {
    using secs = std::chrono::seconds;

    // std::chrono::seconds only guarantees Rep = int35.
    auto const v = GENERATE(values({
        secs{secs::min()},
        secs{-2},
        secs{-1},
        secs{0},
        secs{1},
        secs{secs::max()},
    }));

    CAPTURE(v);

    read_preference rp;
    CHECK_NOTHROW(rp.max_staleness(v));

    CHECKED_IF(v.count() == -1) {
        CHECK_FALSE(rp.max_staleness().has_value());
    }

    else {
        CHECK(rp.max_staleness() == v);

        CHECK_NOTHROW(rp.max_staleness(secs{-1}));
        CHECK_FALSE(rp.max_staleness().has_value());
    }
}

TEST_CASE("equality", "[mongocxx][v1][read_preference]") {
    using mode = read_preference::read_mode;
    using secs = std::chrono::seconds;

    read_preference lhs;
    read_preference rhs;

    CHECK(lhs == lhs);
    CHECK(rhs == rhs);
    CHECK(lhs == rhs);

    SECTION("hedge") {
        BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
        CHECK_NOTHROW(rhs.hedge(scoped_bson{R"({"enabled": true})"}.view()));
        BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("mode") {
        CHECK_NOTHROW(rhs.mode(mode::k_primary_preferred));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("tags") {
        scoped_bson const tags{R"(["x"])"};

        CHECK_NOTHROW(rhs.tags(tags.view()));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);

        CHECK_NOTHROW(lhs.tags(tags.array_view()));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("max_staleness") {
        CHECK_NOTHROW(rhs.max_staleness(secs{123}));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
