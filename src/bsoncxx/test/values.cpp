// Copyright 2016 MongoDB Inc.
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

#include "catch.hpp"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using namespace bsoncxx;

TEST_CASE("A default constructed document::value is falsy", "[bsoncxx]") {
    document::value val;
    REQUIRE(!val);
}

TEST_CASE("A default constructed array::value is falsy", "[bsoncxx]") {
    array::value val;
    REQUIRE(!val);
}

TEST_CASE("A populated document::value is truish", "[bsoncxx]") {
    document::value val = builder::stream::document{} << builder::stream::finalize;
    REQUIRE(val);
}

TEST_CASE("A populated array::value is truish", "[bsoncxx]") {
    array::value val = builder::stream::array{} << builder::stream::finalize;
    REQUIRE(val);
}

TEST_CASE("A moved-from document::value is falsy", "[bsoncxx]") {
    document::value val = builder::stream::document{} << builder::stream::finalize;
    document::value val2 = std::move(val);
    REQUIRE(!val);
}

TEST_CASE("A moved-from array::value is falsy", "[bsoncxx]") {
    array::value val = builder::stream::array{} << builder::stream::finalize;
    array::value val2 = std::move(val);
    REQUIRE(!val);
}
