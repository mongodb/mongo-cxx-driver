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

#include <mongocxx/v1/gridfs/upload_result.hh>

//

#include <bsoncxx/test/v1/types/value.hh>

#include <utility>

#include <catch2/catch_test_macros.hpp>

namespace mongocxx {
namespace v1 {
namespace gridfs {

TEST_CASE("ownership", "[mongocxx][v1][gridfs][upload_result]") {
    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    auto source = upload_result::internal::make(source_value);
    auto target = upload_result::internal::make(target_value);

    REQUIRE(source.id() == source_value);
    REQUIRE(target.id() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.id() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.id() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.id() == source_value);
        CHECK(copy.id() == source_value);

        target = copy;

        CHECK(copy.id() == source_value);
        CHECK(target.id() == source_value);
    }
}

TEST_CASE("equality", "[mongocxx][v1][gridfs][upload_result]") {
    auto const id0 = bsoncxx::v1::types::value{"id0"};
    auto const id1 = bsoncxx::v1::types::value{"id1"};

    auto const lhs = upload_result::internal::make(id0);

    SECTION("equal") {
        auto const rhs = upload_result::internal::make(id0);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("id") {
        auto const rhs = upload_result::internal::make(id1);

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
