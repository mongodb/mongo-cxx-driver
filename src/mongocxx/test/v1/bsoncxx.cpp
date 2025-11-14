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

#include <bsoncxx/test/v1/document/view.hh> // IWYU pragma: keep: Catch::StringMaker<bsoncxx::v1::document::view>

#include <mongocxx/private/scoped_bson.hh>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE("StringMaker", "[bsoncxx][test][v1]") {
    mongocxx::scoped_bson const owner{R"({"x": 1, "y": 2.0, "z": [1, 2.0, "3"]})"};

    // Ensure definitions for Catch::StringMaker<T> specializations from bsoncxx are correctly redefined here.
    CHECK(bsoncxx::test::stringify(owner.view()) == R"({"x": 1, "y": 2.0, "z": [1, 2.0, "3"]})");
}

} // namespace
