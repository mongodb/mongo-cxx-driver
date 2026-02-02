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

#include <mongocxx/v1/indexes.hh>

//

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/test/private/scoped_bson.hh>

#include <chrono>
#include <string>
#include <system_error>
#include <utility>

#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = mongocxx::v1::indexes::errc;

namespace {

struct identity_type {};

} // namespace

TEST_CASE("error code", "[mongocxx][v1][indexes][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = mongocxx::v1::indexes::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::indexes"));

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
        std::error_code const ec = code::invalid_name;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_name) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_name) == type_errc::invalid_argument);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes]") {
    identity_type coll1_identity;
    identity_type coll2_identity;

    auto const coll1_id = reinterpret_cast<mongoc_collection_t*>(&coll1_identity);
    auto const coll2_id = reinterpret_cast<mongoc_collection_t*>(&coll2_identity);

    identity_type client1_identity;
    identity_type client2_identity;

    auto const client1_id = reinterpret_cast<mongoc_client_t*>(&client1_identity);
    auto const client2_id = reinterpret_cast<mongoc_client_t*>(&client2_identity);

    auto source = v1::indexes::internal::make(coll1_id, client1_id);
    auto target = v1::indexes::internal::make(coll2_id, client2_id);

    auto const get_collection = [](indexes const& idx) { return v1::indexes::internal::get_collection(idx); };

    REQUIRE(get_collection(source) == coll1_id);
    REQUIRE(get_collection(target) == coll2_id);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(get_collection(move) == coll1_id);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(get_collection(target) == coll1_id);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(get_collection(source) == coll1_id);
        CHECK(get_collection(copy) == coll1_id);

        target = copy;

        CHECK(get_collection(copy) == coll1_id);
        CHECK(get_collection(target) == coll1_id);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes][model]") {
    scoped_bson source_value{R"({"source": 1})"};
    scoped_bson target_value{R"({"target": 2})"};

    indexes::model source{source_value.value()};
    indexes::model target{target_value.value()};

    REQUIRE(source.keys() == source_value.view());
    REQUIRE(target.keys() == target_value.view());

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.keys() == source_value.view());

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.keys() == source_value.view());
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.keys() == source_value.view());
        CHECK(copy.keys() == source_value.view());

        target = copy;

        CHECK(copy.keys() == source_value.view());
        CHECK(target.keys() == source_value.view());
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes][options]") {
    indexes::options source;
    indexes::options target;

    auto source_value = std::chrono::milliseconds{1};
    auto target_value = std::chrono::milliseconds{2};

    source.max_time(source_value);
    target.max_time(target_value);

    REQUIRE(source.max_time() == source_value);
    REQUIRE(target.max_time() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.max_time() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.max_time() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.max_time() == source_value);
        CHECK(copy.max_time() == source_value);

        target = copy;

        CHECK(copy.max_time() == source_value);
        CHECK(target.max_time() == source_value);
    }
}

} // namespace v1
} // namespace mongocxx
