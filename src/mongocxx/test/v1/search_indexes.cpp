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

#include <mongocxx/v1/search_indexes.hh>

//

#include <mongocxx/test/private/scoped_bson.hh>

#include <utility>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][search_indexes]") {
    identity_type id1;
    identity_type id2;
    identity_type id3;

    auto const coll1 = reinterpret_cast<mongoc_collection_t*>(&id1);
    auto const coll2 = reinterpret_cast<mongoc_collection_t*>(&id2);
    auto const coll3 = reinterpret_cast<mongoc_collection_t*>(&id3);

    auto destroy = libmongoc::collection_destroy.create_instance();
    auto copy = libmongoc::collection_copy.create_instance();

    int destroy_count = 0;
    destroy
        ->interpose([&](mongoc_collection_t* ptr) -> void {
            if (ptr) {
                if (ptr != coll1 && ptr != coll2 && ptr != coll3) {
                    FAIL_CHECK("unexpected mongoc_collection_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    int copy_count = 0;
    copy->interpose([&](mongoc_collection_t const* ptr) -> mongoc_collection_t* {
            if (ptr == coll1) {
                ++copy_count;
                return coll2;
            } else if (ptr == coll2) {
                ++copy_count;
                return coll3;
            } else {
                FAIL("unexpected mongoc_collection_t");
                return nullptr;
            }
        })
        .forever();

    auto source = v1::search_indexes::internal::make(coll1);
    auto target = v1::search_indexes::internal::make(coll2);

    auto const is_moved_from = [](search_indexes const& idx) {
        return v1::search_indexes::internal::is_moved_from(idx);
    };

    auto const get_collection = [](search_indexes const& idx) {
        return v1::search_indexes::internal::get_collection(idx);
    };

    REQUIRE(get_collection(source) == coll1);
    REQUIRE(get_collection(target) == coll2);

    SECTION("move") {
        {
            auto move = std::move(source);

            CHECK(is_moved_from(source));
            REQUIRE_FALSE(is_moved_from(move));
            CHECK(get_collection(move) == coll1);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 0);

            target = std::move(move);

            CHECK(is_moved_from(move));
            REQUIRE_FALSE(is_moved_from(target));
            CHECK(get_collection(target) == coll1);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 0);
        }

        CHECK(destroy_count == 1);
    }

    SECTION("copy") {
        {
            auto copy = source;

            CHECK_FALSE(is_moved_from(source));
            REQUIRE_FALSE(is_moved_from(copy));
            CHECK(get_collection(source) == coll1);
            CHECK(get_collection(copy) == coll2);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 1);

            target = copy;

            CHECK_FALSE(is_moved_from(copy));
            REQUIRE_FALSE(is_moved_from(target));
            CHECK(get_collection(copy) == coll2);
            CHECK(get_collection(target) == coll3);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 2);
        }

        CHECK(destroy_count == 2);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][search_indexes][model]") {
    scoped_bson source_value{R"({"source": 1})"};
    scoped_bson target_value{R"({"target": 2})"};

    search_indexes::model source{source_value.value()};
    search_indexes::model target{target_value.value()};

    REQUIRE(source.definition() == source_value.view());
    REQUIRE(target.definition() == target_value.view());

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.definition() == source_value.view());

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.definition() == source_value.view());
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.definition() == source_value.view());
        CHECK(copy.definition() == source_value.view());

        target = copy;

        CHECK(copy.definition() == source_value.view());
        CHECK(target.definition() == source_value.view());
    }
}

} // namespace v1
} // namespace mongocxx
