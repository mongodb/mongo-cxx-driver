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

#include <string>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/uri.hpp>

TEST_CASE("URI", "[uri]") {
    SECTION("Default URI") {
        REQUIRE_NOTHROW(mongocxx::uri{});
        REQUIRE_NOTHROW(mongocxx::uri{mongocxx::uri::k_default_uri});
        REQUIRE(mongocxx::uri{}.to_string() ==
                mongocxx::uri{mongocxx::uri::k_default_uri}.to_string());

        mongocxx::uri u{};

        // Values that should be empty with a blank URI.
        REQUIRE(u.auth_mechanism() == "");
        REQUIRE(u.auth_source() == "admin");
        REQUIRE(u.database() == "");
        REQUIRE(u.hosts().size() == 1);
        REQUIRE(u.hosts()[0].name == "localhost");
        // Don't check 'u.hosts()[0].family'.  Value is platform-dependent.
        REQUIRE(u.hosts()[0].port == 27017);
        REQUIRE(u.options().empty());
        REQUIRE(u.password() == "");
        REQUIRE(u.read_concern().acknowledge_level() ==
                mongocxx::read_concern::level::k_server_default);
        REQUIRE(u.read_concern().acknowledge_string().empty());
        REQUIRE(u.read_preference().mode() == mongocxx::read_preference::read_mode::k_primary);
        REQUIRE(!u.read_preference().tags());
        REQUIRE(!u.read_preference().max_staleness());
        REQUIRE(u.replica_set() == "");
        REQUIRE(u.ssl() == false);
        REQUIRE(u.to_string() == mongocxx::uri::k_default_uri);
        REQUIRE(u.username() == "");
        REQUIRE(u.write_concern().journal() == false);
        REQUIRE(u.write_concern().majority() == false);
        REQUIRE(!u.write_concern().nodes());
        REQUIRE(u.write_concern().timeout() == std::chrono::milliseconds{0});
        REQUIRE(u.write_concern().acknowledge_level());
        REQUIRE(*u.write_concern().acknowledge_level() ==
                mongocxx::write_concern::level::k_default);
    }

    SECTION("Valid URI") {
        REQUIRE_NOTHROW(mongocxx::uri{"mongodb://example.com"});
    }

    SECTION("Invalid URI") {
        std::string invalid{"mongo://example.com"};
        REQUIRE_THROWS_AS(mongocxx::uri{invalid}, mongocxx::logic_error);
        try {
            mongocxx::uri{invalid};
        } catch (const mongocxx::logic_error& e) {
            REQUIRE(e.code() == mongocxx::error_code::k_invalid_uri);
        }
    }
}
