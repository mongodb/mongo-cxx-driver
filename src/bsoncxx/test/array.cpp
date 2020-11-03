// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

namespace {

using namespace bsoncxx;
using bsoncxx::types::b_int32;
using bsoncxx::types::b_utf8;
using types::bson_value::make_value;

TEST_CASE("array has_value and find_value", "[bsoncxx::array::view]") {
    auto val = make_value("hello");

    SECTION("empty array") {
        array::view a;
        REQUIRE(!a.has_value(val));
        REQUIRE(a.find_value(val) == a.cend());
    }

    SECTION("non-matching elements of other types") {
        builder::basic::array a_builder;
        a_builder.append(b_int32{1}, b_int32{2});
        auto a = a_builder.view();
        REQUIRE(!a.has_value(val));
        REQUIRE(a.find_value(val) == a.cend());
    }

    SECTION("non-matching utf8 elements") {
        builder::basic::array a_builder;
        a_builder.append(b_utf8{"yes"}, b_utf8{"no"});
        auto a = a_builder.view();
        REQUIRE(!a.has_value(val));
        REQUIRE(a.find_value(val) == a.cend());
    }

    SECTION("array with one matching element") {
        builder::basic::array a_builder;
        a_builder.append(b_utf8{"yes"}, b_utf8{"no"}, b_utf8{"hello"});
        auto a = a_builder.view();
        REQUIRE(a.has_value(val));
        REQUIRE(a.find_value(val) != a.cend());
    }

    SECTION("array with multiple matching elements") {
        builder::basic::array a_builder;
        a_builder.append(b_utf8{"yes"}, b_utf8{"hello"}, b_utf8{"hello"});
        auto a = a_builder.view();
        REQUIRE(a.has_value(val));
        REQUIRE(a.find_value(val) != a.cend());
        REQUIRE(a.find_value(val) == a.find_value(val));

        // Returns the first matching instance
        auto it = a.find_value(val);
        REQUIRE(it->get_value() == val);
        it++;
        REQUIRE(it->get_value() == val);
        it++;
        REQUIRE(it == a.cend());
    }
}
}  // namespace
