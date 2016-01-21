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

#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/stdx.hpp>

using namespace mongocxx;

TEST_CASE("a default read_concern", "[read_concern]") {
    instance::current();

    read_concern rc{};

    SECTION("has level k_server_default") {
        REQUIRE(rc.acknowledge_level() == read_concern::level::k_server_default);
    }

    SECTION("has an empty string") {
        REQUIRE(rc.acknowledge_string() == stdx::string_view{""});
    }
}

TEST_CASE("read_concern fields may be set and retrieved", "[read_concern]") {
    instance::current();

    read_concern rc{};

    REQUIRE_NOTHROW(rc.acknowledge_level(read_concern::level::k_majority));
    REQUIRE(rc.acknowledge_level() == read_concern::level::k_majority);

    REQUIRE_NOTHROW(rc.acknowledge_string(stdx::string_view{"local"}));
    REQUIRE(rc.acknowledge_string() == stdx::string_view{"local"});
}

TEST_CASE("read_concern level and string affect each other", "[read_concern]") {
    instance::current();

    read_concern rc{};

    SECTION("setting the level changes the string") {
        rc.acknowledge_level(read_concern::level::k_local);
        REQUIRE(rc.acknowledge_string() == stdx::string_view{"local"});
    }

    SECTION("setting the string changes the level") {
        rc.acknowledge_string("majority");
        REQUIRE(rc.acknowledge_level() == read_concern::level::k_majority);
    }

    SECTION("setting the string to an unknown value changes the level to unknown") {
        rc.acknowledge_string("futureCompatible");
        REQUIRE(rc.acknowledge_level() == read_concern::level::k_unknown);
    }
}

TEST_CASE("read_concern throws when trying to set level to k_unknown", "[read_concern]") {
    instance::current();

    read_concern rc{};

    REQUIRE_THROWS(rc.acknowledge_level(read_concern::level::k_unknown));
}
