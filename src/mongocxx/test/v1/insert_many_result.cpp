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

#include <mongocxx/v1/insert_many_result.hpp>

//

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/insert_many_result.hh>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <map>
#include <tuple>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][insert_many_result]") {
    auto const source_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"source": 1})"}.value());
    auto const target_value = v1::bulk_write::result::internal::make(scoped_bson{R"({"target": 2})"}.value());

    auto const source_ids_owner = scoped_bson{R"(["source"])"};
    auto const target_ids_owner = scoped_bson{R"(["target"])"};

    auto const source_ids = bsoncxx::v1::array::value{source_ids_owner.array_view()};
    auto const target_ids = bsoncxx::v1::array::value{target_ids_owner.array_view()};

    auto const source_ids_map = v1::insert_many_result::id_map({{0, source_ids[0].type_view()}});
    auto const target_ids_map = v1::insert_many_result::id_map({{0, target_ids[0].type_view()}});

    insert_many_result source = v1::insert_many_result::internal::make(source_value, source_ids);
    insert_many_result target = v1::insert_many_result::internal::make(target_value, target_ids);

    REQUIRE(source.result() == source_value);
    REQUIRE(target.result() == target_value);

    REQUIRE(source.inserted_ids() == source_ids_map);
    REQUIRE(target.inserted_ids() == target_ids_map);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.result() == source_value);
        CHECK(move.inserted_ids() == source_ids_map);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.result() == source_value);
        CHECK(target.inserted_ids() == source_ids_map);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.result() == source_value);
        CHECK(copy.result() == source_value);

        CHECK(source.inserted_ids() == source_ids_map);
        CHECK(copy.inserted_ids() == source_ids_map);

        target = copy;

        CHECK(copy.result() == source_value);
        CHECK(target.result() == source_value);

        CHECK(copy.inserted_ids() == source_ids_map);
        CHECK(target.inserted_ids() == source_ids_map);
    }
}

TEST_CASE("basic", "[mongocxx][v1][insert_many_result]") {
    namespace types = bsoncxx::v1::types;

    std::int32_t inserted_count = GENERATE(0, 1, 2, 3);

    scoped_bson inserted_ids_owner;
    v1::insert_many_result::id_map inserted_ids_map;
    std::tie(inserted_ids_owner, inserted_ids_map) = GENERATE(
        table<scoped_bson, v1::insert_many_result::id_map>({
            {{}, {}},
            {
                scoped_bson{R"([1])"},
                {
                    {0, types::b_int32{1}},
                },
            },
            {
                scoped_bson{R"([1, 2.0])"},
                {
                    {0, types::b_int32{1}},
                    {1, types::b_double{2.0}},
                },
            },
            {
                scoped_bson{R"([1, 2.0, "three"])"},
                {
                    {0, types::b_int32{1}},
                    {1, types::b_double{2.0}},
                    {2, types::b_string{"three"}},
                },
            },
        }));

    auto const opts = v1::insert_many_result::internal::make(
        v1::bulk_write::result::internal::make(scoped_bson{BCON_NEW("nInserted", BCON_INT32(inserted_count))}.value()),
        bsoncxx::v1::array::value{inserted_ids_owner.array_view()});

    CHECK(opts.result().inserted_count() == inserted_count);
    CHECK(opts.inserted_count() == inserted_count);
    CHECK(opts.inserted_ids() == inserted_ids_map);
}

TEST_CASE("equality", "[mongocxx][v1][insert_many_result]") {
    auto const n0 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nInserted": 0})"}.value());
    auto const n1 = v1::bulk_write::result::internal::make(scoped_bson{R"({"nInserted": 1})"}.value());

    auto const i1 = bsoncxx::v1::array::value{scoped_bson{R"([1])"}.array_view()};
    auto const i2 = bsoncxx::v1::array::value{scoped_bson{R"([2])"}.array_view()};

    auto lhs = v1::insert_many_result::internal::make(n0, i1);
    auto rhs = v1::insert_many_result::internal::make(n0, i1);

    SECTION("equal") {
        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs == rhs);
    }

    SECTION("result") {
        v1::insert_many_result::internal::result(rhs) = n1;

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }

    SECTION("inserted_ids") {
        v1::insert_many_result::internal::inserted_ids(rhs) = i2;

        CHECK(lhs == lhs);
        CHECK(rhs == rhs);
        CHECK(lhs != rhs);
    }
}

} // namespace v1
} // namespace mongocxx
