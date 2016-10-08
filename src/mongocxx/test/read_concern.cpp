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

TEST_CASE("valid read concern settings", "[read_concern]") {
    instance::current();

    read_concern rc{};

    read_concern::level level_setting;
    stdx::string_view string_setting;

    SECTION("default-constructed read_concern") {
        level_setting = read_concern::level::k_server_default;
        string_setting = stdx::string_view{""};
    }

    SECTION("can be assigned with acknowledge_level()") {
        SECTION("local") {
            level_setting = read_concern::level::k_local;
            string_setting = stdx::string_view{"local"};
        }

        SECTION("majority") {
            level_setting = read_concern::level::k_majority;
            string_setting = stdx::string_view{"majority"};
        }

        SECTION("linearizable") {
            level_setting = read_concern::level::k_linearizable;
            string_setting = stdx::string_view{"linearizable"};
        }

        SECTION("server default") {
            level_setting = read_concern::level::k_server_default;
            string_setting = stdx::string_view{""};
        }

        REQUIRE_NOTHROW(rc.acknowledge_level(level_setting));
    }

    SECTION("can be assigned with acknowledge_string()") {
        SECTION("local") {
            level_setting = read_concern::level::k_local;
            string_setting = stdx::string_view{"local"};
        }

        SECTION("majority") {
            level_setting = read_concern::level::k_majority;
            string_setting = stdx::string_view{"majority"};
        }

        SECTION("linearizable") {
            level_setting = read_concern::level::k_linearizable;
            string_setting = stdx::string_view{"linearizable"};
        }

        SECTION("server default") {
            level_setting = read_concern::level::k_server_default;
            string_setting = stdx::string_view{""};
        }

        REQUIRE_NOTHROW(rc.acknowledge_string(string_setting));
    }

    REQUIRE(rc.acknowledge_level() == level_setting);
    REQUIRE(rc.acknowledge_string() == string_setting);
}

TEST_CASE("setting the string to an unknown value changes the level to unknown", "[read_concern]") {
    instance::current();

    read_concern rc{};

    rc.acknowledge_string("futureCompatible");
    REQUIRE(rc.acknowledge_level() == read_concern::level::k_unknown);
}

TEST_CASE("read_concern throws when trying to set level to k_unknown", "[read_concern]") {
    instance::current();

    read_concern rc{};

    REQUIRE_THROWS(rc.acknowledge_level(read_concern::level::k_unknown));
}
