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

#include <mongocxx/v1/hint.hpp>

//

#include <bsoncxx/test/v1/types/view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][hint]") {
    auto const source_value = "source";
    auto const target_value = "target";

    hint source{source_value};
    hint target{target_value};

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.str() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.str() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.str() == source_value);
        CHECK(copy.str() == source_value);

        target = copy;

        CHECK(copy.str() == source_value);
        CHECK(target.str() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][hint]") {
    hint const v;

    CHECK_FALSE(v.str().has_value());
    CHECK_FALSE(v.doc().has_value());
}

TEST_CASE("str", "[mongocxx][v1][hint]") {
    auto const input = GENERATE(as<std ::string>{}, std::string{}, "", "abc");

    hint const v{input};

    CHECK(v.str() == input);
    CHECK_FALSE(v.doc().has_value());

    CHECK(v.to_value().type_id() == bsoncxx::v1::types::id::k_string);
    CHECK(v.to_value().get_string().value == input);
    CHECK(v.to_value() == v); // operator bsoncxx::v1::types::view()
}

TEST_CASE("doc", "[mongocxx][v1][hint]") {
    auto const input = GENERATE(as<scoped_bson>{}, scoped_bson{}, R"({})", R"({"x": 1})");

    hint const v{input.value()};

    CHECK(v.doc() == input.view());
    CHECK_FALSE(v.str().has_value());

    CHECK(v.to_value().type_id() == bsoncxx::v1::types::id::k_document);
    CHECK(v.to_value().get_document().value == input.view());
    CHECK(v.to_value() == v); // operator bsoncxx::v1::types::view()
}

TEST_CASE("equality", "[mongocxx][v1][hint]") {
    hint lhs;
    hint rhs;

    SECTION("default") {
        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("str") {
        lhs = hint{"lhs"};
        rhs = hint{"rhs"};

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("doc") {
        lhs = hint{scoped_bson{R"({"lhs": 1})"}.value()};
        rhs = hint{scoped_bson{R"({"rhs": 1})"}.value()};

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("mixed") {
        auto lhs_value = std::string{"lhs"};
        auto rhs_value = scoped_bson{R"({"rhs": 1})"}.value();

        lhs = hint{lhs_value};
        rhs = hint{rhs_value};

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);

        CHECK(lhs == lhs_value);
        CHECK(lhs_value == lhs);

        CHECK(rhs == rhs_value);
        CHECK(rhs_value == rhs);

        CHECK(rhs != lhs_value);
        CHECK(lhs_value != rhs);

        CHECK(lhs != rhs_value);
        CHECK(rhs_value != lhs);
    }
}

} // namespace v1
} // namespace mongocxx
