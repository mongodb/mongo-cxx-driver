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
// resultations under the License.

#include <mongocxx/v1/insert_one_result.hpp>

//

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/insert_one_result.hh>

#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <bsoncxx/private/bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][insert_one_result]") {
    auto const source_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"source": 1})"}.value());
    auto const target_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"target": 2})"}.value());

    auto const source_id = bsoncxx::v1::types::value{"source"};
    auto const target_id = bsoncxx::v1::types::value{"target"};

    insert_one_result source = v1::insert_one_result::internal::make(source_value, source_id);
    insert_one_result target = v1::insert_one_result::internal::make(target_value, target_id);

    REQUIRE(source.result() == source_value);
    REQUIRE(target.result() == target_value);

    REQUIRE(source.inserted_id() == source_id);
    REQUIRE(target.inserted_id() == target_id);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.result() == source_value);
        CHECK(move.inserted_id() == source_id);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.result() == source_value);
        CHECK(target.inserted_id() == source_id);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.result() == source_value);
        CHECK(copy.result() == source_value);

        CHECK(source.inserted_id() == source_id);
        CHECK(copy.inserted_id() == source_id);

        target = copy;

        CHECK(copy.result() == source_value);
        CHECK(target.result() == source_value);

        CHECK(copy.inserted_id() == source_id);
        CHECK(target.inserted_id() == source_id);
    }
}

TEST_CASE("basic", "[mongocxx][v1][insert_one_result]") {
    auto const inserted_count = GENERATE(0, 1);
    auto const inserted_id = GENERATE(as<bsoncxx::v1::types::value>{}, 1, 2.0, "three");

    auto const opts = v1::insert_one_result::internal::make(
        v1::bulk_write::result::internal::make(scoped_bson{BCON_NEW("nInserted", BCON_INT32(inserted_count))}.value()),
        inserted_id);

    CHECK(opts.result().inserted_count() == inserted_count);
    CHECK(opts.inserted_id() == inserted_id);
}

TEST_CASE("equality", "[mongocxx][v1][insert_one_result]") {
    auto const n0 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nInserted": 0})"}.value());
    auto const n1 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nInserted": 1})"}.value());

    auto const i1 = bsoncxx::v1::types::value{1};
    auto const i2 = bsoncxx::v1::types::value{2};

    auto lhs = v1::insert_one_result::internal::make(n0, i1);
    auto rhs = v1::insert_one_result::internal::make(n0, i1);

    SECTION("equal") {
        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("result") {
        v1::insert_one_result::internal::result(rhs) = n1;

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("inserted_id") {
        v1::insert_one_result::internal::inserted_id(rhs) = i2;

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
