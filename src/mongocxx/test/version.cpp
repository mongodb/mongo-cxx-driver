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

#include <bsoncxx/private/version.hh>

//

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/private/config/config.hh>
#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/catch.hh>

namespace {

using bsoncxx::test_util::split_version;

TEST_CASE("mongoc version numbers", "[mongocxx][test]") {
    auto expect_str = bsoncxx::stdx::string_view{MONGOC_REQUIRED_VERSION()};

    CAPTURE(expect_str);

    auto const expect = split_version(expect_str);

    // The major version number should always be present.
    REQUIRE(expect.size() > 0);

    CHECK(mongoc_get_major_version() == expect[0]);

    CHECKED_IF(expect.size() > 1) {
        CHECK(bson_get_minor_version() >= expect[1]);

        // Only when minor version number compares equal.
        CHECKED_IF(expect.size() > 2) {
            CHECK(bson_get_micro_version() >= expect[2]);
        }
    }
}

TEST_CASE(
    "mongoc version string"
    "[mongocxx][test]") {
    auto const expect_str = bsoncxx::stdx::string_view{MONGOC_REQUIRED_VERSION()};
    auto const actual_str = bsoncxx::stdx::string_view{MONGOC_VERSION_S};

    CAPTURE(expect_str);
    CAPTURE(actual_str);

    auto const expect = split_version(expect_str);
    auto const actual = split_version(actual_str);

    // The major version number should always be present.
    REQUIRE(expect.size() > 0);
    REQUIRE(actual.size() > 0);

    // Major version number.
    CHECK(actual[0] == expect[0]);

    CHECKED_IF(expect.size() > 1) {
        REQUIRE(actual.size() > 1);

        // Minor version number.
        CHECK(actual[1] >= expect[1]);

        // Only when minor version number compares equal.
        CHECKED_IF(expect.size() > 2) {
            REQUIRE(actual.size() > 2);

            // Patch version number.
            CHECK(actual[2] >= expect[2]);
        }
    }
}

} // namespace
