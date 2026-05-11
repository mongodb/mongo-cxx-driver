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

#include <mongocxx/v1/oidc_credential.hpp>

//

#include <chrono>

#include <catch2/catch_test_macros.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][oidc_credential]") {
    oidc_credential source{"source_token"};
    oidc_credential target{"target_token"};

    REQUIRE(source.access_token() == "source_token");
    REQUIRE(target.access_token() == "target_token");

    SECTION("move") {
        auto moved = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(moved.access_token() == "source_token");

        target = std::move(moved);

        // moved is in an assign-or-destroy-only state.

        CHECK(target.access_token() == "source_token");
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.access_token() == "source_token");
        CHECK(copy.access_token() == "source_token");

        target = copy;

        CHECK(copy.access_token() == "source_token");
        CHECK(target.access_token() == "source_token");
    }
}

TEST_CASE("getters", "[mongocxx][v1][oidc_credential]") {
    SECTION("without expiration") {
        oidc_credential creds{"foo"};
        CHECK(creds.access_token() == "foo");
        CHECK(!creds.expires_in());
    }
    SECTION("with expiration") {
        oidc_credential creds{"bar", std::chrono::milliseconds{123}};
        CHECK(creds.access_token() == "bar");
        CHECK(creds.expires_in());
        CHECK(creds.expires_in()->count() == 123);
    }
}

} // namespace v1
} // namespace mongocxx
