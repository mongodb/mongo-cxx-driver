//
// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/collection.hh>

//

#include <mongocxx/v1/exception.hpp>

#include <string>
#include <system_error>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = collection::errc;

namespace {

struct identity_type {};

} // namespace

TEST_CASE("error code", "[mongocxx][v1][collection][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = collection::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::collection"));

    auto const zero_errc = make_error_condition(static_cast<std::errc>(0));

    SECTION("unknown") {
        std::error_code const ec = static_cast<code>(-1);

        CHECK(ec.category() == category);
        CHECK(ec.value() == -1);
        CHECK(ec);
        CHECK(ec.message() == std::string(category.name()) + ":-1");
    }

    SECTION("zero") {
        std::error_code const ec = code::zero;

        CHECK(ec.category() == category);
        CHECK(ec.value() == 0);
        CHECK_FALSE(ec);
        CHECK(ec.message() == "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("non-zero") {
        std::error_code const ec = code::max_time_u32;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::max_time_u32) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::max_time_u32) == type_errc::invalid_argument);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][collection]") {
    identity_type client_identity;
    auto const client_id = reinterpret_cast<mongoc_client_t*>(&client_identity);

    identity_type id1;
    identity_type id2;
    identity_type id3;

    auto const coll1 = reinterpret_cast<mongoc_collection_t*>(&id1);
    auto const coll2 = reinterpret_cast<mongoc_collection_t*>(&id2);
    auto const coll3 = reinterpret_cast<mongoc_collection_t*>(&id3);

    auto destroy = libmongoc::collection_destroy.create_instance();
    auto copy = libmongoc::collection_copy.create_instance();

    int destroy_count = 0;
    int copy_count = 0;

    destroy
        ->interpose([&](mongoc_collection_t* ptr) -> void {
            if (ptr) {
                if (ptr != coll1 && ptr != coll2 && ptr != coll3) {
                    FAIL("unexpected mongoc_collection_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    copy->interpose([&](mongoc_collection_t* ptr) -> mongoc_collection_t* {
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

    auto source = collection::internal::make(coll1, client_id);
    auto target = collection::internal::make(coll2, client_id);

    REQUIRE(collection::internal::as_mongoc(source) == coll1);
    REQUIRE(collection::internal::as_mongoc(target) == coll2);

    SECTION("move") {
        {
            auto move = std::move(source);

            CHECK_FALSE(source);
            REQUIRE(move);
            CHECK(collection::internal::as_mongoc(move) == coll1);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 0);

            target = std::move(move);

            CHECK_FALSE(move);
            REQUIRE(target);
            CHECK(collection::internal::as_mongoc(target) == coll1);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 0);
        }

        CHECK(destroy_count == 1);
    }

    SECTION("copy") {
        {
            auto copy = source;

            REQUIRE(source);
            REQUIRE(copy);
            CHECK(collection::internal::as_mongoc(source) == coll1);
            CHECK(collection::internal::as_mongoc(copy) == coll2);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 1);

            target = copy;

            REQUIRE(copy);
            REQUIRE(target);
            CHECK(collection::internal::as_mongoc(copy) == coll2);
            CHECK(collection::internal::as_mongoc(target) == coll3);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 2);
        }

        CHECK(destroy_count == 2);
    }
}

TEST_CASE("default", "[mongocxx][v1][collection]") {
    collection const coll;

    CHECK_FALSE(coll);
}

} // namespace v1
} // namespace mongocxx
