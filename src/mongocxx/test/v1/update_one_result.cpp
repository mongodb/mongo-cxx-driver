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

#include <mongocxx/v1/update_one_result.hpp>

//

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/update_one_result.hh>

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <bsoncxx/private/bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][update_one_result]") {
    auto const source_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"source": 1})"}.value());
    auto const target_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"target": 2})"}.value());

    update_one_result source = v1::update_one_result::internal::make(source_value);
    update_one_result target = v1::update_one_result::internal::make(target_value);

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

TEST_CASE("basic", "[mongocxx][v1][update_one_result]") {
    auto const matched_count = GENERATE(0, 1);
    auto const modified_count = GENERATE(0, 1);
    auto const upserted_id = GENERATE(as<bsoncxx::v1::types::value>{}, bsoncxx::v1::types::b_null{}, 1, 2.0, "three");

    auto const upserted_ids = [&]() -> scoped_bson {
        scoped_bson ret;

        if (upserted_id.type_id() != bsoncxx::v1::types::id::k_null) {
            scoped_bson e;
            REQUIRE(BSON_APPEND_INT32(e.inout_ptr(), "index", 0));
            REQUIRE(BSON_APPEND_VALUE(
                e.inout_ptr(), "_id", &bsoncxx::v1::types::value::internal::get_bson_value(upserted_id)));
            ret += scoped_bson{BCON_NEW("0", BCON_DOCUMENT(e.bson()))};
        }

        return ret;
    }();

    auto const res = v1::update_one_result::internal::make(
        v1::bulk_write::result::internal::make(
            scoped_bson{BCON_NEW(
                            "nMatched",
                            BCON_INT32(matched_count),
                            "nModified",
                            BCON_INT32(modified_count),
                            "upserted",
                            BCON_ARRAY(upserted_ids.bson()))}
                .value()));

    CHECK(res.result().matched_count() == matched_count);
    CHECK(res.result().modified_count() == modified_count);

    CHECK(res.matched_count() == matched_count);
    CHECK(res.modified_count() == modified_count);

    if (upserted_id.type_id() == bsoncxx::v1::types::id::k_null) {
        CHECK_FALSE(res.upserted_id().has_value());
    } else {
        CHECK(res.upserted_id() == upserted_id);
    }
}

TEST_CASE("equality", "[mongocxx][v1][update_one_result]") {
    auto const n0 = v1::bulk_write::result::internal::make(
        scoped_bson{R"({"nMatched": 0, "nModified": 0, "upserted": []})"}.value());
    auto const n1 = v1::bulk_write::result::internal::make(
        scoped_bson{R"({"nMatched": 1, "nModified": 1, "upserted": [{"index": 0, "_id": 1}]})"}.value());

    auto lhs = v1::update_one_result::internal::make(n0);
    auto rhs = v1::update_one_result::internal::make(n0);

    SECTION("equal") {
        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("result") {
        v1::update_one_result::internal::result(rhs) = n1;

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
