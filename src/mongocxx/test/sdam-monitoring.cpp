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

#include <set>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/to_string.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>

#include <bsoncxx/test/catch.hh>
#include <bsoncxx/test/exception_guard.hh>

#include <mongocxx/test/client_helpers.hh>
#include <mongocxx/test/spec/operation.hh>

// Don't use SDAM Monitoring spec tests, we'd need libmongoc internals to send hello replies.

namespace {
using namespace mongocxx;
using bsoncxx::oid;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::string::to_string;

void open_and_close_client(uri const& test_uri, options::apm const& apm_opts) {
    // Apply listeners and trigger connection.
    options::client client_opts;
    client_opts.apm_opts(apm_opts);
    client client{test_uri, test_util::add_test_server_api(client_opts)};
    client["admin"].run_command(make_document(kvp("ping", 1)));
}

TEST_CASE("SDAM Monitoring", "[sdam_monitoring]") {
    instance::current();
    std::string rs_name;
    uri test_uri;

    auto topology_type = test_util::get_topology();
    if (topology_type == "replicaset") {
        rs_name = test_util::replica_set_name();
        test_uri = uri{"mongodb://localhost/?replicaSet=" + rs_name};
    }

    bsoncxx::test::exception_guard_state eguard;
    options::apm apm_opts;
    bsoncxx::stdx::optional<oid> topology_id;

    SECTION("Server Events") {
        int server_opening_events = 0;
        int server_changed_events = 0;
        int server_closed_events = 0;

        ///////////////////////////////////////////////////////////////////////
        // Begin server description listener lambdas
        ///////////////////////////////////////////////////////////////////////

        // ServerOpeningEvent
        apm_opts.on_server_opening([&](events::server_opening_event const& event) {
            server_opening_events++;
            if (topology_id) {
                // A previous server was opened first.
                CHECK(topology_id.value() == event.topology_id());
            }

            topology_id = event.topology_id();
        });

        // ServerDescriptionChanged
        apm_opts.on_server_changed([&](events::server_changed_event const& event) {
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);

            server_changed_events++;
            // A server_opening_event should have set topology_id.
            REQUIRE(topology_id);
            REQUIRE(topology_id.value() == event.topology_id());

            auto old_sd = event.previous_description();
            auto new_sd = event.new_description();
            auto new_type = to_string(new_sd.type());

            REQUIRE(old_sd.hello().empty());
            REQUIRE(to_string(old_sd.host()) == to_string(event.host()));
            REQUIRE(old_sd.port() == event.port());
            REQUIRE(old_sd.round_trip_time() == -1);
            REQUIRE(to_string(old_sd.type()) == "Unknown");

            REQUIRE(to_string(new_sd.host()) == to_string(event.host()));
            REQUIRE_FALSE(new_sd.hello().empty());
            REQUIRE(new_sd.port() == event.port());
            REQUIRE(new_sd.round_trip_time() >= 0);
            if (topology_type == "single") {
                REQUIRE(new_type == "Standalone");
            } else if (topology_type == "replicaset") {
                // RSPrimary, RSSecondary, etc.
                REQUIRE(new_type.substr(0, 2) == "RS");
            } else {
                REQUIRE(topology_type == "sharded");
                REQUIRE(new_type == "Mongos");
            }

            REQUIRE(old_sd.id() == new_sd.id());

            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard);
        });

        apm_opts.on_server_closed([&](events::server_closed_event const& event) {
            server_closed_events++;
            CHECK(topology_id.value() == event.topology_id());
        });

        ///////////////////////////////////////////////////////////////////////
        // End server description listener lambdas
        ///////////////////////////////////////////////////////////////////////

        open_and_close_client(test_uri, apm_opts);
        BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard);
        REQUIRE(server_opening_events > 0);
        REQUIRE(server_changed_events > 0);
        REQUIRE(server_closed_events > 0);
    }

    SECTION("Topology Events") {
        int topology_opening_events = 0;
        int topology_changed_events = 0;
        int topology_closed_events = 0;
        bool found_servers = false;

        ///////////////////////////////////////////////////////////////////////
        // Begin topology description listener lambdas
        ///////////////////////////////////////////////////////////////////////

        // TopologyOpeningEvent
        apm_opts.on_topology_opening([&](events::topology_opening_event const& event) {
            topology_opening_events++;
            if (topology_id) {
                // A previous server was opened first.
                CHECK(topology_id.value() == event.topology_id());
            }

            topology_id = event.topology_id();
        });

        // TopologyDescriptionChanged
        apm_opts.on_topology_changed([&](events::topology_changed_event const& event) {
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);

            topology_changed_events++;
            // A topology_opening_event should have set topology_id.
            REQUIRE(topology_id);
            REQUIRE(topology_id.value() == event.topology_id());

            auto old_td = event.previous_description();
            auto new_td = event.new_description();
            auto new_type = to_string(new_td.type());
            auto new_servers = new_td.servers();

            if (topology_changed_events == 1) {
                // First event, nothing discovered yet.
                REQUIRE(old_td.servers().size() == 0);
                REQUIRE_FALSE(old_td.has_readable_server(read_preference{}));
                REQUIRE_FALSE(old_td.has_writable_server());
            }

            // A topology_changed_event may also be triggered when server monitoring closes,
            // which transitions the topology description into an "Unknown" state.
            CHECKED_IF(new_type != "Unknown") {
                if (topology_type == "replicaset") {
                    if (new_td.has_writable_server()) {
                        REQUIRE(new_type == "ReplicaSetWithPrimary");
                    } else {
                        REQUIRE(new_type == "ReplicaSetNoPrimary");
                    }
                } else {
                    REQUIRE(new_type == "Single");
                }
            }

            for (auto&& new_sd : new_servers) {
                found_servers = true;
                auto new_sd_type = to_string(new_sd.type());
                REQUIRE(new_sd.host().length());
                REQUIRE_FALSE(new_sd.hello().empty());
                REQUIRE(new_sd.port() > 0);
                REQUIRE(new_sd.round_trip_time() >= 0);
                if (topology_type == "single") {
                    REQUIRE(new_sd_type == "Standalone");
                } else if (topology_type == "replicaset") {
                    // RSPrimary, RSSecondary, etc.
                    REQUIRE(new_sd_type.substr(0, 2) == "RS");
                } else {
                    REQUIRE(topology_type == "sharded");
                    REQUIRE(new_sd_type == "Mongos");
                }
            }

            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard);
        });

        // TopologyClosedEvent
        apm_opts.on_topology_closed([&](events::topology_closed_event const& event) {
            topology_closed_events++;
            CHECK(topology_id.value() == event.topology_id());
        });

        ///////////////////////////////////////////////////////////////////////
        // End topology description listener lambdas
        ///////////////////////////////////////////////////////////////////////

        open_and_close_client(test_uri, apm_opts);
        BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard);
        REQUIRE(topology_opening_events > 0);
        REQUIRE(topology_changed_events > 0);
        REQUIRE(topology_closed_events > 0);
        REQUIRE(found_servers);
    }

    SECTION("Heartbeat Events") {
        int heartbeat_started_events = 0;
        int heartbeat_succeeded_events = 0;
        auto mock_started_awaited = libmongoc::apm_server_heartbeat_started_get_awaited.create_instance();
        auto mock_succeeded_awaited = libmongoc::apm_server_heartbeat_succeeded_get_awaited.create_instance();
        bool started_awaited_called = false;
        bool succeeded_awaited_called = false;

        mock_started_awaited->visit(
            [&](mongoc_apm_server_heartbeat_started_t const*) { started_awaited_called = true; });

        mock_succeeded_awaited->visit(
            [&](mongoc_apm_server_heartbeat_succeeded_t const*) { succeeded_awaited_called = true; });

        ///////////////////////////////////////////////////////////////////////
        // Begin heartbeat listener lambdas
        ///////////////////////////////////////////////////////////////////////

        // ServerHeartbeatStartedEvent
        apm_opts.on_heartbeat_started([&](events::heartbeat_started_event const& event) {
            heartbeat_started_events++;
            CHECK_FALSE(event.host().empty());
            CHECK(event.port() != 0);
            // Client is single-threaded, and will never perform an awaitable hello.
            CHECK(!event.awaited());
        });

        // ServerHeartbeatSucceededEvent
        apm_opts.on_heartbeat_succeeded([&](events::heartbeat_succeeded_event const& event) {
            heartbeat_succeeded_events++;
            CHECK_FALSE(event.host().empty());
            CHECK(event.port() != 0);
            CHECK_FALSE(event.reply().empty());
            // Client is single-threaded, and will never perform an awaitable hello.
            CHECK(!event.awaited());
        });

        // Don't expect a ServerHeartbeatFailedEvent here, see the test below.

        ///////////////////////////////////////////////////////////////////////
        // End heartbeat listener lambdas
        ///////////////////////////////////////////////////////////////////////

        open_and_close_client(test_uri, apm_opts);
        BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard);
        REQUIRE(heartbeat_started_events > 0);
        REQUIRE(heartbeat_succeeded_events > 0);
        REQUIRE(started_awaited_called);
        REQUIRE(succeeded_awaited_called);
    }
}

TEST_CASE("Heartbeat failed event", "[sdam_monitoring]") {
    instance::current();
    options::apm apm_opts;
    bool failed_awaited_called = false;
    auto mock_failed_awaited = libmongoc::apm_server_heartbeat_failed_get_awaited.create_instance();

    mock_failed_awaited->visit([&](mongoc_apm_server_heartbeat_failed_t const*) { failed_awaited_called = true; });

    int heartbeat_failed_events = 0;

    // ServerHeartbeatFailedEvent
    apm_opts.on_heartbeat_failed([&](events::heartbeat_failed_event const& event) {
        heartbeat_failed_events++;
        CHECK_FALSE(event.host().empty());
        CHECK_FALSE(event.message().empty());
        CHECK(event.port() != 0);
        CHECK(!event.awaited());
    });

    REQUIRE_THROWS_AS(
        open_and_close_client(uri{"mongodb://bad-host/?connectTimeoutMS=1"}, apm_opts), mongocxx::exception);

    REQUIRE(heartbeat_failed_events > 0);
    REQUIRE(failed_awaited_called);
}
} // namespace
