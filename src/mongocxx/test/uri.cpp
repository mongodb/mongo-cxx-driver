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
