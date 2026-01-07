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

#include <mongocxx/v1/delete_many_result.hpp>

//

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/delete_many_result.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <bsoncxx/private/bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][delete_many_result]") {
    auto const source_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"source": 1})"}.value());
    auto const target_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"target": 2})"}.value());

    delete_many_result source = v1::delete_many_result::internal::make(source_value);
    delete_many_result target = v1::delete_many_result::internal::make(target_value);

    REQUIRE(source.result() == source_value);
    REQUIRE(target.result() == target_value);
    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.result() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.result() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.result() == source_value);
        CHECK(copy.result() == source_value);

        target = copy;

        CHECK(copy.result() == source_value);
        CHECK(target.result() == source_value);
    }
}

TEST_CASE("basic", "[mongocxx][v1][delete_many_result]") {
    auto const deleted_count = GENERATE(0, 1);

    auto const res = v1::delete_many_result::internal::make(
        v1::bulk_write::result::internal::make(scoped_bson{BCON_NEW("nRemoved", BCON_INT32(deleted_count))}.value()));

    CHECK(res.result().deleted_count() == deleted_count);
    CHECK(res.deleted_count() == deleted_count);
}

TEST_CASE("equality", "[mongocxx][v1][delete_many_result]") {
    auto const n0 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nRemoved": 0})"}.value());
    auto const n1 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nRemoved": 1})"}.value());

    auto lhs = v1::delete_many_result::internal::make(n0);
    auto rhs = v1::delete_many_result::internal::make(n0);

    SECTION("equal") {
        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("result") {
        v1::delete_many_result::internal::result(rhs) = n1;

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
