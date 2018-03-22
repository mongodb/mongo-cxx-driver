// Copyright 2018-present MongoDB Inc.
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

#include "helpers.hpp"

#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/session.hpp>

namespace {
using namespace mongocxx;

  bool server_has_sessions (const client& conn) {
    auto result = conn["admin"].run_command(
        make_document(kvp("isMaster", 1)));
    auto result_view = result.view();

    return !!result_view["logicalSessionTimeoutMinutes"];
}

TEST_CASE("session options", "[session]") {
    MOCK_CLIENT

    instance::current();

    client c{uri{}};

    SECTION("default") {
        auto s = c.start_session({});

        REQUIRE(s.options().causal_consistency());
    }

    SECTION("set causal consistency") {
        options::session opts;
        REQUIRE(opts.causal_consistency());
        opts.causal_consistency(false);
        REQUIRE(!opts.causal_consistency());

        auto s = c.start_session(opts);
        REQUIRE(!s.options().causal_consistency());
    }
}

TEST_CASE("start_session failure", "[session]") {
    MOCK_CLIENT

    instance::current();

    client_start_session->interpose(
        [](mongoc_client_t*, const mongoc_session_opt_t*, bson_error_t*) {
            return (mongoc_client_session_t*)nullptr;
        });

    client c{uri{}};

    REQUIRE_THROWS_AS(c.start_session({}), mongocxx::exception);
}

TEST_CASE("start_session", "[session]") {
    MOCK_CLIENT

    instance::current();

    client c{uri{}};

    auto s = c.start_session({});
    REQUIRE(s.session_id() != stdx::nullopt);
    REQUIRE(!s.has_ended());
    // The session hasn't been used for anything yet.
    REQUIRE(s.cluster_time() == stdx::nullopt);
    REQUIRE(s.operation_time() == stdx::nullopt);

    s.end_session();
    REQUIRE(s.has_ended());
}
}  // namespace
