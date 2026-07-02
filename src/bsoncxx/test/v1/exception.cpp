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

#include <bsoncxx/test/v1/exception.hh>

//

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

TEST_CASE("source", "[bsoncxx][v1][error]") {
    auto const& c = bsoncxx::v1::source_error_category();

    SECTION("name") {
        CHECK_THAT(c.name(), Catch::Matchers::Equals("bsoncxx::v1::source_errc"));
    }

    SECTION("message") {
        CHECK(c.message(-1) == "bsoncxx::v1::source_errc:-1");
        CHECK(c.message(0) == "zero");
        CHECK(c.message(1) == "bsoncxx");
        CHECK(c.message(2) == "bson");
        CHECK(c.message(3) == "bsoncxx::v1::source_errc:3");
    }
}

TEST_CASE("type", "[bsoncxx][v1][error]") {
    auto const& c = bsoncxx::v1::type_error_category();

    SECTION("name") {
        CHECK_THAT(c.name(), Catch::Matchers::Equals("bsoncxx::v1::type_errc"));
    }

    SECTION("message") {
        CHECK(c.message(-1) == "bsoncxx::v1::type_errc:-1");
        CHECK(c.message(0) == "zero");
        CHECK(c.message(1) == "invalid argument");
        CHECK(c.message(2) == "runtime error");
        CHECK(c.message(3) == "bsoncxx::v1::type_errc:3");
    }
}

} // namespace
