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
#include "helpers.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/view.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/read_preference.hpp>

using namespace bsoncxx;
using namespace mongocxx;

TEST_CASE("Read preference", "[read_preference]") {
    instance::current();

    read_preference rp;

    SECTION("Defaults to mode primary, empty tags, and no max staleness") {
        REQUIRE(rp.mode() == read_preference::read_mode::k_primary);
        REQUIRE_FALSE(rp.tags());
        REQUIRE_FALSE(rp.max_staleness());
    }

    SECTION("Can have mode changed") {
        rp.mode(read_preference::read_mode::k_nearest);
        REQUIRE(rp.mode() == read_preference::read_mode::k_nearest);
    }

    SECTION("Can have tags changed") {
        auto tags = builder::stream::document{} << "tag_key"
                                                << "tag_value" << builder::stream::finalize;
        rp.tags(tags.view());
        REQUIRE(rp.tags().value() == tags);
    }

    SECTION("Can have max_staleness changed") {
        std::chrono::seconds max_staleness{120};
        rp.max_staleness(max_staleness);
        REQUIRE(rp.max_staleness().value() == max_staleness);
    }

    SECTION("Rejects invalid max_staleness") {
        REQUIRE_THROWS_AS(rp.max_staleness(std::chrono::seconds{0}), logic_error);
        REQUIRE_THROWS_AS(rp.max_staleness(std::chrono::seconds{-1}), logic_error);
    }
}

TEST_CASE("Read preference can be constructed with another read_mode", "[read_preference]") {
    instance::current();

    read_preference rp(read_preference::read_mode::k_secondary);
    REQUIRE(rp.mode() == read_preference::read_mode::k_secondary);
    REQUIRE_FALSE(rp.tags());
}

TEST_CASE("Read preference can be constructed with a read_mode and tags", "[read_preference]") {
    instance::current();
    auto tags = builder::stream::document{} << "tag_key"
                                            << "tag_value" << builder::stream::finalize;

    read_preference rp(read_preference::read_mode::k_secondary, tags.view());
    REQUIRE(rp.mode() == read_preference::read_mode::k_secondary);
    REQUIRE(rp.tags().value() == tags);
}

TEST_CASE("Read preference comparison works", "[read_preference]") {
    instance::current();

    read_preference rp_a;
    read_preference rp_b;
    REQUIRE(rp_a == rp_b);

    SECTION("mode is compared") {
        rp_a.mode(read_preference::read_mode::k_nearest);
        REQUIRE_FALSE(rp_a == rp_b);
        rp_b.mode(read_preference::read_mode::k_nearest);
        REQUIRE(rp_a == rp_b);
    }

    SECTION("tags are compared") {
        auto tags = builder::stream::document{} << "tag_key"
                                                << "tag_value" << builder::stream::finalize;
        rp_a.tags(tags.view());
        REQUIRE_FALSE(rp_a == rp_b);
        rp_b.tags(tags.view());
        REQUIRE(rp_a == rp_b);
    }

    SECTION("max_staleness is compared") {
        std::chrono::seconds max_staleness{120};
        rp_a.max_staleness(max_staleness);
        REQUIRE_FALSE(rp_a == rp_b);
        rp_b.max_staleness(max_staleness);
        REQUIRE(rp_a == rp_b);
    }
}

TEST_CASE("Read preference methods call underlying mongoc methods", "[read_preference]") {
    instance::current();
    MOCK_READ_PREFERENCE

    read_preference rp;
    bool called = false;

    SECTION("mode() calls mongoc_read_prefs_set_mode()") {
        read_preference::read_mode expected_mode = read_preference::read_mode::k_nearest;
        read_prefs_set_mode->interpose([&](mongoc_read_prefs_t*, mongoc_read_mode_t mode) {
            called = true;
            REQUIRE(mode == static_cast<mongoc_read_mode_t>(expected_mode));
        });
        rp.mode(expected_mode);
    }

    SECTION("tags() calls mongoc_read_prefs_set_tags()") {
        auto expected_tags = builder::stream::document{} << "foo"
                                                         << "bar" << builder::stream::finalize;
        read_prefs_set_tags->interpose([&](mongoc_read_prefs_t*, const bson_t* tags) {
            called = true;
            REQUIRE(bson_get_data(tags) == expected_tags.view().data());
        });
        rp.tags(expected_tags.view());
    }

    SECTION("max_staleness() calls mongoc_read_prefs_set_max_staleness_seconds()") {
        std::chrono::seconds expected_max_staleness_sec{150};
        read_prefs_set_max_staleness_seconds->interpose(
            [&](mongoc_read_prefs_t*, int64_t max_staleness_sec) {
                called = true;
                REQUIRE(std::chrono::seconds{max_staleness_sec} == expected_max_staleness_sec);
            });
        rp.max_staleness(expected_max_staleness_sec);
    }

    REQUIRE(called);
}
