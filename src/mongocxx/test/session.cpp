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

#include <helpers.hpp>

#include <bsoncxx/private/helpers.hh>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/private/libmongoc.hh>

namespace {
using bsoncxx::from_json;
using bsoncxx::document::value;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_timestamp;

using namespace mongocxx;

bool server_has_sessions(const client& conn) {
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
    using namespace mongocxx::test_util;

    MOCK_CLIENT

    instance::current();

    client_start_session
        ->interpose([](mongoc_client_t*, const mongoc_session_opt_t*, bson_error_t* error) {
            bson_set_error(error, MONGOC_ERROR_CLIENT, MONGOC_ERROR_CLIENT_SESSION_FAILURE, "foo");
            return (mongoc_client_session_t*)nullptr;
        })
        .forever();

    client c{uri{}};

    REQUIRE_THROWS_MATCHES(
        c.start_session(), mongocxx::exception, mongocxx_exception_matcher{"foo"});
}

TEST_CASE("session", "[session]") {
    using namespace mongocxx::test_util;

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

        REQUIRE_THROWS_MATCHES(c2["db"]["collection"].insert_one(s, {}),
                               bulk_write_exception,
                               mongocxx_exception_matcher{"Invalid sessionId"});
    }
}

// Receive command-started events from libmongoc's APM to test session ids.
// TODO: Port to C++ Driver's APM once it's implemented, CXX-1562.
void command_started(const mongoc_apm_command_started_t* event);

class session_test {
   public:
    session_test() : client{uri{}} {
        auto client_t = static_cast<mongoc_client_t*>(client_t_from_client(client));
        auto callbacks = mongoc_apm_callbacks_new();
        mongoc_apm_set_command_started_cb(callbacks, command_started);
        mongoc_client_set_apm_callbacks(client_t, callbacks, this);
        mongoc_apm_callbacks_destroy(callbacks);
    }

    void test_method_with_session(std::string method_name,
                                  const std::function<void(bool)>& f,
                                  const session& s) {
        using std::string;

        events.clear();

        // A method with an explicit session must send its logical session id or "lsid".
        f(true);
        if (events.size() == 0) {
            throw std::logic_error{"no events after calling command with explicit session"};
        }

        for (auto& event : events) {
            if (!event.command["lsid"]) {
                throw std::logic_error{method_name + " sent no lsid with " + event.command_name +
                                       " and explicit session"};
            }
            if (event.command["lsid"].get_document().view() != s.id()) {
                throw std::logic_error{method_name + " sent wrong lsid with " + event.command_name +
                                       " and explicit session"};
            }
        }

        events.clear();

        // A method called with no session must send an implicit session id with the command.
        f(false);
        if (events.size() == 0) {
            throw std::logic_error{"no events after calling command with implicit session"};
        }

        for (auto& event : events) {
            if (!event.command["lsid"]) {
                throw std::logic_error{method_name + " sent no lsid with " + event.command_name +
                                       " and implicit session"};
            }
            if (event.command["lsid"].get_document().view() == s.id()) {
                throw std::logic_error{method_name + " sent wrong lsid with " + event.command_name +
                                       " and implicit session"};
            }
        }
    }

    class apm_event {
       public:
        apm_event(const std::string& command_name_, const bsoncxx::document::value& document_)
            : command_name(command_name_), value(document_), command(value.view()) {}

        std::string command_name;
        bsoncxx::document::value value;
        bsoncxx::document::view command;
    };

    std::vector<apm_event> events;
    mongocxx::client client;
};

void command_started(const mongoc_apm_command_started_t* event) {
    using namespace bsoncxx::helpers;

    std::string command_name{mongoc_apm_command_started_get_command_name(event)};

    // Ignore auth commands like "saslStart", and handshakes with "isMaster".
    std::string sasl{"sasl"};
    if (command_name.substr(0, sasl.size()) == sasl || command_name == "isMaster") {
        return;
    }

    auto& listener =
        *(reinterpret_cast<session_test*>(mongoc_apm_command_started_get_context(event)));
    auto document = value_from_bson_t(mongoc_apm_command_started_get_command(event));

    listener.events.emplace_back("command_started_event", document);
}

TEST_CASE("lsid", "[session]") {
    instance::current();

    session_test test;

    if (!server_has_sessions(test.client)) {
        return;
    }

    auto s = test.client.start_session();
    auto collection = test.client["lsid"]["collection"];

    SECTION("create_bulk_write") {
        auto f = [&s, &collection](bool use_session) {
            auto bulk =
                use_session ? collection.create_bulk_write(s) : collection.create_bulk_write();

            bulk.append(model::insert_one{{}});
            collection.bulk_write(bulk);
        };

        test.test_method_with_session("create_bulk_write", f, s);
    }

    SECTION("bulk_write") {
        std::vector<model::write> vec;
        vec.emplace_back(model::insert_one{{}});

        auto bulk_write_vector = [&s, &collection, &vec](bool use_session) {
            use_session ? collection.bulk_write(s, vec) : collection.bulk_write(vec);
        };

        test.test_method_with_session("vector bulk_write", bulk_write_vector, s);

        auto bulk_write_iterator = [&s, &collection, &vec](bool use_session) {
            use_session ? collection.bulk_write(s, vec.begin(), vec.end())
                        : collection.bulk_write(vec.begin(), vec.end());
        };

        test.test_method_with_session("iterator bulk_write", bulk_write_iterator, s);
    }

    SECTION("write") {
        auto f = [&s, &collection](bool use_session) {
            auto insert_op = model::insert_one{{}};
            use_session ? collection.write(s, insert_op) : collection.write(insert_op);
        };

        test.test_method_with_session("write", f, s);
    }

    SECTION("insert_one") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.insert_one(s, {}) : collection.insert_one({});
        };

        test.test_method_with_session("insert_one", f, s);
    }

    SECTION("insert_many") {
        bsoncxx::document::value doc({});
        std::vector<bsoncxx::document::view> docs{doc.view()};

        auto insert_vector = [&s, &collection, &docs](bool use_session) {
            return use_session ? collection.insert_many(s, docs) : collection.insert_many(docs);
        };

        test.test_method_with_session("vector insert_many", insert_vector, s);

        auto insert_iter = [&s, &collection, &docs](bool use_session) {
            use_session ? collection.insert_many(s, docs.begin(), docs.end())
                        : collection.insert_many(docs.begin(), docs.end());
        };

        test.test_method_with_session("iterator insert_many", insert_iter, s);
    }
}
}  // namespace
