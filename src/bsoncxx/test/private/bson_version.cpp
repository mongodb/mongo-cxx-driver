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

#include <string>
#include <vector>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/config/config.hh>

#include <bsoncxx/test/catch.hh>

namespace {

using bsoncxx::test_util::split_version;

TEST_CASE("split_version", "[bsoncxx][test][split_version]") {
    CHECK(split_version("1") == std::vector<int>({1}));
    CHECK(split_version("1.2") == std::vector<int>({1, 2}));
    CHECK(split_version("1.2.3") == std::vector<int>({1, 2, 3}));
    CHECK(split_version("11.22.33") == std::vector<int>({11, 22, 33}));

    CHECK(split_version("1-") == std::vector<int>({1}));
    CHECK(split_version("1.2-") == std::vector<int>({1, 2}));
    CHECK(split_version("1.2.3-") == std::vector<int>({1, 2, 3}));
    CHECK(split_version("1.2.3-suffix") == std::vector<int>({1, 2, 3}));
}

TEST_CASE("bson version numbers", "[bsoncxx][test]") {
    auto expect_str = bsoncxx::stdx::string_view{BSON_REQUIRED_VERSION()};

    CAPTURE(expect_str);

    auto const expect = split_version(expect_str);

    // The major version number should always be present.
    REQUIRE(expect.size() > 0);

    CHECK(bson_get_major_version() == expect[0]);

    CHECKED_IF(expect.size() > 1) {
        CHECK(bson_get_minor_version() >= expect[1]);

        // Only when minor version number compares equal.
        CHECKED_IF(expect.size() > 2) {
            CHECK(bson_get_micro_version() >= expect[2]);
        }
    }
}

TEST_CASE(
    "bson version string"
    "[bsoncxx][test]") {
    auto const expect_str = bsoncxx::stdx::string_view{BSON_REQUIRED_VERSION()};
    auto const actual_str = bsoncxx::stdx::string_view{BSON_VERSION_S};

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
