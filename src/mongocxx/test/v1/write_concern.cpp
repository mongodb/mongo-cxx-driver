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

#include <mongocxx/test/v1/write_concern.hh>

//

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <chrono>
#include <cstdint>
#include <tuple>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][write_concern]") {
    write_concern source;
    write_concern target;

    source.tag("source");
    target.tag("target");

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.tag() == "source");

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.tag() == "source");
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.tag() == "source");
        CHECK(copy.tag() == "source");

        target = copy;

        CHECK(copy.tag() == "source");
        CHECK(target.tag() == "source");
    }
}

TEST_CASE("default", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern const wc;

    CHECK(wc.acknowledge_level() == level::k_default);
    CHECK(wc.timeout().count() == 0);
    CHECK_FALSE(wc.nodes().has_value());
    CHECK_FALSE(wc.tag().has_value());
    CHECK_FALSE(wc.journal().has_value());
    CHECK(wc.is_acknowledged());
}

TEST_CASE("acknowledge_level", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern wc;

    SECTION("normal") {
        level input = {};
        level expected = {};

        std::tie(input, expected) = GENERATE(
            table<level, level>({
                {level::k_majority, level::k_majority},
                {level::k_default, level::k_default},
                {level::k_tag, level::k_default}, // Undocumented behavior.
                {level::k_unacknowledged, level::k_unacknowledged},
                {level::k_acknowledged, level::k_acknowledged},
                {level::k_unknown, level::k_default},       // Undocumented behavior.
                {static_cast<level>(-1), level::k_default}, // Undocumented behavior.
                {static_cast<level>(6), level::k_default},  // Undocumented behavior.
            }));

        CAPTURE(input);
        CHECK(wc.acknowledge_level(input).acknowledge_level() == expected);
    }

    SECTION("nodes") {
        level input = {};
        bsoncxx::v1::stdx::optional<std::int32_t> expected = {};

        std::tie(input, expected) = GENERATE(
            table<level, bsoncxx::v1::stdx::optional<std::int32_t>>({
                {level::k_majority, {}},
                {level::k_default, {}},
                {level::k_unacknowledged, 0},
                {level::k_acknowledged, 1},
                {level::k_unknown, {}},
            }));

        CAPTURE(input);
        CHECK(wc.acknowledge_level(input).nodes() == expected);
    }

    {
        auto const input = GENERATE(values({
            level::k_majority,
            level::k_default,
            level::k_unacknowledged,
            level::k_acknowledged,
        }));

        CAPTURE(input);

        SECTION("tag") {
            CHECK_FALSE(wc.acknowledge_level(input).tag().has_value());
        }

        SECTION("journal") {
            CHECK_FALSE(wc.acknowledge_level(input).journal().has_value());
        }

        SECTION("journal") {
            CHECK_FALSE(wc.acknowledge_level(input).journal().has_value());
        }
    }
}

TEST_CASE("majority", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern wc;

    SECTION("normal") {
        wc.majority();

        CHECK(wc.acknowledge_level() == level::k_majority);
        CHECK(wc.timeout().count() == 0);
        CHECK_FALSE(wc.nodes().has_value());
        CHECK_FALSE(wc.tag().has_value());
        CHECK_FALSE(wc.journal().has_value());
        CHECK(wc.is_acknowledged());
    }

    SECTION("timeout") {
        auto const timeout = std::chrono::milliseconds{123};

        wc.majority(timeout);

        CHECK(wc.acknowledge_level() == level::k_majority);
        CHECK(wc.timeout() == timeout);
        CHECK_FALSE(wc.nodes().has_value());
        CHECK_FALSE(wc.tag().has_value());
        CHECK_FALSE(wc.journal().has_value());
        CHECK(wc.is_acknowledged());
    }
}

TEST_CASE("timeout", "[mongocxx][v1][write_concern]") {
    write_concern wc;

    std::int64_t before_i64 = {};
    std::int64_t after_i64 = {};

    std::tie(before_i64) = GENERATE(table<std::int64_t>({0, 1, INT64_MAX}));
    std::tie(after_i64) = GENERATE(table<std::int64_t>({INT64_MIN, -1, 0, 1, INT64_MAX}));

    std::chrono::milliseconds const before{before_i64};
    std::chrono::milliseconds const after{after_i64};

    CAPTURE(before);
    CAPTURE(after);

    CHECK_NOTHROW(wc.timeout(before));
    CHECK(wc.timeout() == before);

    CHECK_NOTHROW(wc.timeout(after));

    CHECKED_IF(after.count() < 0) {
        CHECK(wc.timeout() == before); // Undocumented behavior.
    }

    else {
        CHECK(wc.timeout() == after);
    }
}

TEST_CASE("nodes", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern wc;

    std::int32_t before = {};
    std::int32_t after = {};

    std::tie(before) = GENERATE(table<std::int32_t>({0, 1, INT32_MAX}));
    std::tie(after) = GENERATE(table<std::int32_t>({INT32_MIN, -1, 0, 1, INT32_MAX}));

    CAPTURE(before);
    CAPTURE(after);

    CHECK_NOTHROW(wc.nodes(before));
    CHECK(wc.nodes() == before);

    CHECKED_IF(before == 0) {
        CHECK(wc.acknowledge_level() == level::k_unacknowledged);
    }

    else {
        CHECK(wc.acknowledge_level() == level::k_acknowledged);
    }

    CHECK_NOTHROW(wc.nodes(after));

    CHECKED_IF(after < 0) {
        CHECK_FALSE(wc.nodes().has_value()); // Undocumented behavior.
        CHECK(wc.acknowledge_level() == level::k_default);
    }

    else {
        CHECK(wc.nodes() == after);

        CHECKED_IF(after == 0) {
            CHECK(wc.acknowledge_level() == level::k_unacknowledged);
        }

        else {
            CHECK(wc.acknowledge_level() == level::k_acknowledged);
        }
    }

    CHECK_NOTHROW(wc.acknowledge_level(level::k_default));
    CHECK_FALSE(wc.nodes().has_value());
}

TEST_CASE("tag", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern wc;

    bsoncxx::v1::stdx::string_view v;

    std::tie(v) = GENERATE(table<bsoncxx::v1::stdx::string_view>({{}, "", "abc", "custom"}));

    CAPTURE(v);

    CHECK_NOTHROW(wc.tag(v));
    CHECK(wc.tag() == v);
    CHECK(wc.acknowledge_level() == level::k_tag);

    CHECK_NOTHROW(wc.acknowledge_level(level::k_default));
    CHECK_FALSE(wc.tag().has_value());
}

TEST_CASE("journal", "[mongocxx][v1][write_concern]") {
    write_concern wc;

    auto const before = static_cast<bool>(GENERATE(values({0, 1})));
    auto const after = static_cast<bool>(GENERATE(values({0, 1})));

    CAPTURE(before);
    CAPTURE(after);

    CHECK_NOTHROW(wc.journal(before));
    CHECK(wc.journal().value() == before);

    CHECK_NOTHROW(wc.journal(after));
    CHECK(wc.journal().value() == after);

    wc = {};

    CHECK_FALSE(wc.journal().has_value());
}

TEST_CASE("is_acknowledged", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern wc;

    CHECK_NOTHROW(wc.acknowledge_level(level::k_unacknowledged));
    CHECK_FALSE(wc.is_acknowledged());

    CHECK_NOTHROW(wc.journal(true));
    CHECK(wc.is_acknowledged());

    CHECK_NOTHROW(wc.journal(false));
    CHECK_FALSE(wc.is_acknowledged());

    CHECK_NOTHROW(wc.acknowledge_level(level::k_acknowledged));
    CHECK(wc.is_acknowledged());
}

TEST_CASE("to_document", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern wc;

    SECTION("default") {
        CHECK(wc.to_document().view() == bsoncxx::v1::document::view{});
    }

    SECTION("acknowledge_level") {
        level v = {};
        scoped_bson expected;

        std::tie(v, expected) = GENERATE(map(
            [](std::tuple<level, char const*> i) {
                return std::make_tuple(std::get<0>(i), scoped_bson{std::get<1>(i)});
            },
            table<level, char const*>({
                {level::k_majority, R"({"w": "majority"})"},
                {level::k_default, R"({})"},
                {level::k_unacknowledged, R"({"w": 0})"},
                {level::k_acknowledged, R"({"w": 1})"},
            })));

        CAPTURE(v);

        CHECK_NOTHROW(wc.acknowledge_level(v));
        CHECK(wc.to_document().view() == expected.view());
    }

    SECTION("timeout") {
        std::int64_t v;
        scoped_bson expected;

        std::tie(v, expected) = GENERATE(map(
            [](std::tuple<std::int64_t, char const*> const& i) {
                return std::make_tuple(std::get<0>(i), scoped_bson{std::get<1>(i)});
            },
            table<std::int64_t, char const*>({
                {0, R"({})"},
                {1, R"({"wtimeout": 1})"},
                {INT32_MAX, R"({"wtimeout": 2147483647})"},
                {std::int64_t{INT32_MAX} + 1, R"({"wtimeout": {"$numberLong": "2147483648"}})"},
                {std::int64_t{INT64_MAX} - 1, R"({"wtimeout": {"$numberLong": "9223372036854775806"}})"},
                {std::int64_t{INT64_MAX}, R"({"wtimeout": {"$numberLong": "9223372036854775807"}})"},
            })));
        CAPTURE(v);

        auto const doc = wc.timeout(std::chrono::milliseconds{v}).to_document();
        CHECK(doc.view() == expected.view());
    }

    SECTION("nodes") {
        CHECK_NOTHROW(wc.nodes(3));
        CHECK(wc.to_document().view() == scoped_bson{R"({"w": 3})"}.view());
    }

    SECTION("tag") {
        CHECK_NOTHROW(wc.tag("abc"));
        CHECK(wc.to_document().view() == scoped_bson{R"({"w": "abc"})"}.view());
    }

    SECTION("journal") {
        auto const v = static_cast<bool>(GENERATE(values({0, 1})));
        CHECK_NOTHROW(wc.journal(v));
        CHECK(wc.to_document().view() == scoped_bson{BCON_NEW("j", BCON_BOOL(v))}.view());
    }
}

TEST_CASE("equality", "[mongocxx][v1][write_concern]") {
    using level = write_concern::level;

    write_concern lhs;
    write_concern rhs;

    CHECK(lhs == lhs);
    CHECK(rhs == rhs);
    CHECK(lhs == rhs);

    SECTION("acknowledge_level") {
        rhs.acknowledge_level(level::k_acknowledged);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("timeout") {
        rhs.timeout(std::chrono::milliseconds{123});

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("nodes") {
        rhs.nodes(3);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("tag") {
        rhs.tag("abc");

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("journal") {
        rhs.journal(false);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
