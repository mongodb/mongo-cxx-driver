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

#include <bsoncxx/decimal128.hpp>

//

#include <bsoncxx/test/v1/decimal128.hh>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/test/catch.hh>

namespace {

using bsoncxx::v_noabi::decimal128;
using code = bsoncxx::v_noabi::error_code;

TEST_CASE("exceptions", "[bsoncxx][v_noabi][decimal128]") {
    CHECK_THROWS_WITH_CODE(decimal128{bsoncxx::stdx::string_view{}}, code::k_invalid_decimal128);
    CHECK_THROWS_WITH_CODE(decimal128{"invalid"}, code::k_invalid_decimal128);
}

TEST_CASE("basic", "[bsoncxx][v_noabi][decimal128]") {
    SECTION("to_string") {
        CHECK(decimal128{}.to_string() == "0E-6176");
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][decimal128]") {
    using v1 = bsoncxx::v1::decimal128;
    using v_noabi = bsoncxx::v_noabi::decimal128;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    SECTION("from_v1") {
        v1 from{1, 1};
        v_noabi to = from;

        CHECK(to == to_v1(to));
        CHECK(to == from);
        CHECK(to != v1{});
    }

    SECTION("to_v1") {
        v_noabi from{1, 1};
        v1 const to{from};

        CHECK(to == from_v1(to));
        CHECK(to == from);
        CHECK(to != v_noabi{});
    }
}

} // namespace
