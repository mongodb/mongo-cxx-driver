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

#include <mongocxx/result/insert_many.hpp>

//

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/insert_many_result.hh>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <catch2/catch_test_macros.hpp>

namespace mongocxx {
namespace v_noabi {

TEST_CASE("equality", "[mongocxx][v_noabi][result][insert_many]") {
    auto const n0 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nInserted": 0})"}.value());
    auto const n1 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nInserted": 1})"}.value());

    auto const i1 = bsoncxx::v1::array::value{scoped_bson{R"([1, 1.0, "one"])"}.array_view()};
    auto const i2 = bsoncxx::v1::array::value{scoped_bson{R"([2, 2.0, "two"])"}.array_view()};

    SECTION("equal") {
        auto const lhs = from_v1(v1::insert_many_result::internal::make(n0, i1));
        auto const rhs = from_v1(v1::insert_many_result::internal::make(n0, i1));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("result") {
        auto const lhs = from_v1(v1::insert_many_result::internal::make(n0, i1));
        auto const rhs = from_v1(v1::insert_many_result::internal::make(n1, i1));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("inserted_ids") {
        auto const lhs = from_v1(v1::insert_many_result::internal::make(n0, i1));
        auto const rhs = from_v1(v1::insert_many_result::internal::make(n0, i2));

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v_noabi
} // namespace mongocxx
