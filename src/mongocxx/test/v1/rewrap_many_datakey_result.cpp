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

#include <mongocxx/v1/rewrap_many_datakey_result.hpp>

//

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/rewrap_many_datakey_result.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][rewrap_many_datakey_result]") {
    auto const source_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"source": 1})"}.value());
    auto const target_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"target": 2})"}.value());

    rewrap_many_datakey_result source = v1::rewrap_many_datakey_result::internal::make();
    rewrap_many_datakey_result target = v1::rewrap_many_datakey_result::internal::make();

    v1::rewrap_many_datakey_result::internal::result(source) = source_value;
    v1::rewrap_many_datakey_result::internal::result(target) = target_value;

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

TEST_CASE("default", "[mongocxx][v1][rewrap_many_datakey_result]") {
    auto const opts = v1::rewrap_many_datakey_result::internal::make();

    CHECK_FALSE(opts.result().has_value());
}

} // namespace v1
} // namespace mongocxx
