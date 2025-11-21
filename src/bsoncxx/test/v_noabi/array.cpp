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

#include <bsoncxx/test/v1/array/view.hh>

#include <algorithm>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>

#include <bsoncxx/test/catch.hh>

namespace {

using namespace bsoncxx;
using bsoncxx::types::b_int32;
using bsoncxx::types::b_string;
using types::bson_value::make_value;

TEST_CASE("array element lookup with std::find", "[bsoncxx::array::view]") {
    auto val = make_value("hello");

    SECTION("empty array") {
        array::view a;
        REQUIRE(std::find(a.cbegin(), a.cend(), val) == a.cend());
    }

    SECTION("non-matching elements of other types") {
        builder::basic::array a_builder;
        a_builder.append(b_int32{1}, b_int32{2});
        auto a = a_builder.view();
        REQUIRE(std::find(a.cbegin(), a.cend(), val) == a.cend());
    }

    SECTION("non-matching utf8 elements") {
        builder::basic::array a_builder;
        a_builder.append(b_string{"yes"}, b_string{"no"});
        auto a = a_builder.view();
        REQUIRE(std::find(a.cbegin(), a.cend(), val) == a.cend());
    }

    SECTION("array with one matching element") {
        builder::basic::array a_builder;
        a_builder.append(b_string{"yes"}, b_string{"no"}, b_string{"hello"});
        auto a = a_builder.view();
        auto it = std::find(a.cbegin(), a.cend(), val);
        REQUIRE(it != a.cend());
        REQUIRE(*it == val);
    }

    SECTION("array with multiple matching elements") {
        builder::basic::array a_builder;
        a_builder.append(b_string{"yes"}, b_string{"hello"}, b_string{"hello"});
        auto a = a_builder.view();
        auto it = std::find(a.cbegin(), a.cend(), val);
        REQUIRE(it != a.cend());
        REQUIRE(*it == val);

        // Returns the first matching instance
        it++;
        it = std::find(it, a.cend(), val);
        REQUIRE(it != a.cend());
        REQUIRE(*it == val);
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][array][element]") {
    using v1 = v1::element::view;
    using v_noabi = v_noabi::array::element;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    auto const owner = bsoncxx::builder::basic::make_array(1);
    auto const arr = owner.view();
    auto const e = arr[0];

    SECTION("from_v1") {
        v1 from{e};
        v_noabi const to = from;

        CHECK(to.raw() == to_v1(to).raw());
        CHECK(to.raw() == from.raw());
        CHECK(to.raw() != v1{}.raw());
    }

    SECTION("to_v1") {
        v_noabi from{e};
        v1 const to{from};

        CHECK(to.raw() == v_noabi{to}.raw()); // No from_v1().
        CHECK(to.raw() == from.raw());
        CHECK(to.raw() != v_noabi{}.raw());
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][array][view]") {
    using v1 = v1::array::view;
    using v_noabi = v_noabi::array::view;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    auto const owner = bsoncxx::builder::basic::make_array(1);
    auto const arr = owner.view();

    SECTION("from_v1") {
        v1 from{arr.data()};
        v_noabi const to = from;

        CHECK(to == to_v1(to));
        CHECK(to == from);
        CHECK(to != v1{});
    }

    SECTION("to_v1") {
        v_noabi from{arr.data(), arr.size()};
        v1 const to{from};

        CHECK(to == from_v1(to));
        CHECK(to == from);
        CHECK(to != v_noabi{});
    }
}

} // namespace
