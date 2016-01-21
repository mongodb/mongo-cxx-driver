// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/read_preference.hpp>

using namespace mongocxx;
using namespace bsoncxx;

TEST_CASE("Read Preference", "[read_preference]") {
    instance::current();

    read_preference rp;
    auto tags = builder::stream::document{} << "blah"
                                            << "wow" << builder::stream::finalize;

    SECTION("Defaults to mode primary and empty tags") {
        REQUIRE(rp.mode() == read_preference::read_mode::k_primary);
        REQUIRE_FALSE(rp.tags());

        SECTION("Can have mode changed") {
            rp.mode(read_preference::read_mode::k_nearest);
            REQUIRE(rp.mode() == read_preference::read_mode::k_nearest);
        }

        SECTION("Can have tags changed") {
            rp.tags(tags.view());
            REQUIRE(rp.tags().value() == tags);
        }
    }

    SECTION("Can be constructed with another read_mode") {
        read_preference rp(read_preference::read_mode::k_secondary);
        REQUIRE(rp.mode() == read_preference::read_mode::k_secondary);
        REQUIRE_FALSE(rp.tags());
    }

    SECTION("Can be constructed with a read_mode and tags") {
        read_preference rp(read_preference::read_mode::k_secondary, tags.view());
        REQUIRE(rp.mode() == read_preference::read_mode::k_secondary);
        REQUIRE(rp.tags().value() == tags);
    }

    SECTION("Can be compared with another read preference") {
        read_preference other;
        REQUIRE(rp == other);
        other.mode(read_preference::read_mode::k_nearest);
        REQUIRE_FALSE(rp == other);
        other.mode(read_preference::read_mode::k_primary);
        REQUIRE(rp == other);
        other.tags(tags.view());
        REQUIRE_FALSE(rp == other);
        rp.tags(tags.view());
        REQUIRE(rp == other);
        auto other_tags = builder::stream::document{} << "blah"
                                                      << "other" << builder::stream::finalize;
        other.tags(other_tags.view());
        REQUIRE_FALSE(rp == other);
    }
}
