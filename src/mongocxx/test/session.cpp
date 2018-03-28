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

#include <iostream>

#include <helpers.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>

namespace {
using bsoncxx::from_json;
using bsoncxx::document::value;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_timestamp;

using namespace mongocxx;

static bool server_has_sessions(const client& conn) {
    auto result = conn["admin"].run_command(make_document(kvp("isMaster", 1)));
    auto result_view = result.view();

    if (result_view["logicalSessionTimeoutMinutes"]) {
        return true;
    }

    WARN("skip: server does not support sessions");
    return false;
}

TEST_CASE("session options", "[session]") {
    instance::current();

    client c{uri{}};

    if (!server_has_sessions(c)) {
        return;
    }

    SECTION("default") {
        auto s = c.start_session();

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

    client_start_session
        ->interpose([](mongoc_client_t*, const mongoc_session_opt_t*, bson_error_t* error) {
            bson_set_error(error, MONGOC_ERROR_CLIENT, MONGOC_ERROR_CLIENT_SESSION_FAILURE, "foo");
            return (mongoc_client_session_t*)nullptr;
        })
        .forever();

    client c{uri{}};

    // TODO: Once we've upgraded Catch2 (CXX-1537) use REQUIRE_THROWS_MATCHES.
    REQUIRE_THROWS_WITH(c.start_session(), Catch::Contains("foo"));
    REQUIRE_THROWS_AS(c.start_session(), mongocxx::exception);
}

TEST_CASE("session", "[session]") {
    instance::current();

    client c{uri{}};

    if (!server_has_sessions(c)) {
        return;
    }

    auto s = c.start_session();

    SECTION("id") {
        REQUIRE(!s.id().empty());
    }

    SECTION("cluster time and operation time") {
        b_timestamp zero, nonzero;
        zero.timestamp = 0;
        zero.increment = 0;
        nonzero.timestamp = 1;
        nonzero.increment = 0;

        // The session hasn't been used for anything yet.
        REQUIRE(s.cluster_time().empty());
        REQUIRE(s.operation_time() == zero);

        // Advance the cluster time - just a basic test, rely on libmongoc's logic.
        s.advance_cluster_time(
            from_json("{\"clusterTime\": {\"$timestamp\": {\"t\": 1, \"i\": 0}}}"));
        REQUIRE(!s.cluster_time().empty());
        REQUIRE(s.operation_time() == zero);

        // Advance the operation time, just a basic test again.
        s.advance_operation_time(nonzero);
        REQUIRE(s.operation_time() == nonzero);
    }

    SECTION("pool") {
        // "Pool is LIFO" test from Driver Sessions Spec.
        auto session_a = stdx::make_unique<session>(c.start_session());
        auto session_b = stdx::make_unique<session>(c.start_session());
        auto a_id = value(session_a->id());
        auto b_id = value(session_b->id());

        // End session A, then session B.
        session_a = nullptr;
        session_b = nullptr;

        auto session_c = stdx::make_unique<session>(c.start_session());
        REQUIRE(session_c->id() == b_id);
        auto session_d = stdx::make_unique<session>(c.start_session());
        REQUIRE(session_d->id() == a_id);
    }

    SECTION("wrong client") {
        using Catch::Matchers::Contains;

        // "Session argument is for the right client" test from Driver Sessions Spec.
        client c2{uri{}};

        // TODO: Once we've upgraded Catch2 (CXX-1537) use REQUIRE_THROWS_MATCHES.
        REQUIRE_THROWS_WITH(c2["db"]["collection"].insert_one({}, s),
                            Contains("Invalid sessionId"));
        REQUIRE_THROWS_AS(c2["db"]["collection"].insert_one({}, s), bulk_write_exception);
    }
}
}  // namespace
