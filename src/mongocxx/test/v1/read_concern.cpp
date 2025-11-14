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

#include <mongocxx/test/v1/read_concern.hh>

//

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <tuple>
#include <utility>

#include <mongocxx/private/scoped_bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][read_concern]") {
    read_concern source;
    read_concern target;

    source.acknowledge_string("source");
    target.acknowledge_string("target");

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.acknowledge_string() == "source");

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.acknowledge_string() == "source");
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.acknowledge_string() == "source");
        CHECK(copy.acknowledge_string() == "source");

        target = copy;

        CHECK(copy.acknowledge_string() == "source");
        CHECK(target.acknowledge_string() == "source");
    }
}

TEST_CASE("default", "[mongocxx][v1][read_concern]") {
    read_concern const rc;

    CHECK(rc.acknowledge_level() == read_concern::level::k_server_default);
    CHECK(rc.acknowledge_string().empty());
}

TEST_CASE("acknowledge_level", "[mongocxx][v1][read_concern]") {
    using level = read_concern::level;

    read_concern rc;

    SECTION("normal") {
        level input = {};
        level expected{};

        std::tie(input, expected) = GENERATE(
            table<level, level>({
                {level::k_local, level::k_local},
                {level::k_majority, level::k_majority},
                {level::k_linearizable, level::k_linearizable},
                {level::k_server_default, level::k_server_default},
                {level::k_unknown, level::k_server_default}, // Undocumented behavior.
                {level::k_available, level::k_available},
                {level::k_snapshot, level::k_snapshot},
                {static_cast<level>(-1), level::k_server_default}, // Undocumented behavior.
            }));

        CAPTURE(input);
        CHECK(rc.acknowledge_level(input).acknowledge_level() == expected);
    }

    SECTION("acknowledge_string") {
        level input = {};
        bsoncxx::v1::stdx::string_view expected;

        std::tie(input, expected) = GENERATE(
            table<level, bsoncxx::v1::stdx::string_view>({
                {level::k_local, "local"},
                {level::k_majority, "majority"},
                {level::k_linearizable, "linearizable"},
                {level::k_server_default, {}},
                {level::k_unknown, {}}, // Undocumented behavior.
                {level::k_available, "available"},
                {level::k_snapshot, "snapshot"},
                {static_cast<level>(-1), {}}, // Undocumented behavior.
            }));

        CAPTURE(input);
        CHECK(rc.acknowledge_level(input).acknowledge_string() == expected);
    }
}

TEST_CASE("acknowledge_string", "[mongocxx][v1][read_concern]") {
    using level = read_concern::level;

    static constexpr bsoncxx::v1::stdx::string_view empty;

    read_concern rc;

    SECTION("normal") {
        auto const v = GENERATE(
            as<bsoncxx::v1::stdx::string_view>{},
            // Normal.
            "local",
            "majority",
            "linearizable",
            empty,
            "unknown", // Arbitrary string.
            "available",
            "snapshot",

            // Arbitrary.
            "",
            "abc",
            "123",
            "arbitrary",
            "level");

        CAPTURE(v);
        CHECK(rc.acknowledge_string(v).acknowledge_string() == v);
    }

    SECTION("acknowledge_level") {
        bsoncxx::v1::stdx::string_view input;
        level expected = {};

        std::tie(input, expected) = GENERATE(
            table<bsoncxx::v1::stdx::string_view, level>({
                // Normal.
                {"local", level::k_local},
                {"majority", level::k_majority},
                {"linearizable", level::k_linearizable},
                {empty, level::k_server_default},
                {"", level::k_server_default},
                {"available", level::k_available},
                {"snapshot", level::k_snapshot},

                // Arbitrary.
                {"abc", level::k_unknown},
                {"123", level::k_unknown},
                {"arbitrary", level::k_unknown},
                {"level", level::k_unknown},
            }));

        CAPTURE(input);
        CHECK(rc.acknowledge_string(input).acknowledge_level() == expected);
    }
}

TEST_CASE("to_document", "[mongocxx][v1][read_concern]") {
    using level = read_concern::level;

    SECTION("default") {
        CHECK(read_concern{}.to_document() == bsoncxx::v1::document::view{});
    }

    SECTION("acknowledge_level") {
        level v = {};
        scoped_bson expected;

        std::tie(v, expected) = GENERATE(map(
            [](std::tuple<level, char const*> i) {
                return std::make_tuple(std::get<0>(i), scoped_bson{std::get<1>(i)});
            },
            table<level, char const*>({
                // Normal.
                {level::k_local, R"({"level": "local"})"},
                {level::k_majority, R"({"level": "majority"})"},
                {level::k_linearizable, R"({"level": "linearizable"})"},
                {level::k_server_default, R"({})"},
                {level::k_unknown, R"({})"}, // Undocumented behavior.
                {level::k_available, R"({"level": "available"})"},
                {level::k_snapshot, R"({"level": "snapshot"})"},
                {static_cast<level>(-1), R"({})"}, // Undocumented behavior.
            })));

        CAPTURE(v);

        auto const doc = read_concern{}.acknowledge_level(v).to_document();
        CHECK(doc.view() == expected.view());
    }

    SECTION("acknowledge_string") {
        bsoncxx::v1::stdx::string_view str;
        scoped_bson expected;

        std::tie(str, expected) = GENERATE(map(
            [](std::tuple<bsoncxx::v1::stdx::string_view, char const*> i) {
                return std::make_tuple(std::get<0>(i), scoped_bson{std::get<1>(i)});
            },
            table<bsoncxx::v1::stdx::string_view, char const*>({
                // Normal.
                {"local", R"({"level": "local"})"},
                {"majority", R"({"level": "majority"})"},
                {"linearizable", R"({"level": "linearizable"})"},
                {bsoncxx::v1::stdx::string_view{}, R"({})"},
                {"", R"({})"},
                {"available", R"({"level": "available"})"},
                {"snapshot", R"({"level": "snapshot"})"},

                // Arbitrary.
                {"abc", R"({"level": "abc"})"},
                {"123", R"({"level": "123"})"},
                {"arbitrary", R"({"level": "arbitrary"})"},
                {"level", R"({"level": "level"})"},
            })));

        CAPTURE(str);

        auto const doc = read_concern{}.acknowledge_string(str).to_document();
        CHECK(doc.view() == expected.view());
    }
}

TEST_CASE("equality", "[mongocxx][v1][read_concern]") {
    using level = read_concern::level;

    read_concern lhs;
    read_concern rhs;

    SECTION("normal") {
        lhs.acknowledge_level(level::k_majority);
        rhs.acknowledge_level(level::k_local);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);

        lhs.acknowledge_level(level::k_available);
        rhs.acknowledge_level(level::k_server_default);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("unknown") {
        lhs.acknowledge_level(level::k_majority);
        rhs.acknowledge_level(level::k_unknown);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);

        lhs.acknowledge_string("lhs");
        rhs.acknowledge_string("rhs");

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
