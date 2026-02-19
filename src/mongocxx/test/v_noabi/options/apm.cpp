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

#include <mongocxx/options/apm.hpp>

//

#include <mongocxx/v1/config/export.hpp>

#include <mongocxx/v1/apm.hh>
#include <mongocxx/v1/events/command_failed.hh>
#include <mongocxx/v1/events/command_started.hh>
#include <mongocxx/v1/events/command_succeeded.hh>
#include <mongocxx/v1/events/server_closed.hh>
#include <mongocxx/v1/events/server_description_changed.hh>
#include <mongocxx/v1/events/server_heartbeat_failed.hh>
#include <mongocxx/v1/events/server_heartbeat_started.hh>
#include <mongocxx/v1/events/server_heartbeat_succeeded.hh>
#include <mongocxx/v1/events/server_opening.hh>
#include <mongocxx/v1/events/topology_closed.hh>
#include <mongocxx/v1/events/topology_description_changed.hh>
#include <mongocxx/v1/events/topology_opening.hh>

#include <functional>
#include <utility>

#include <mongocxx/events/command_failed_event.hpp>
#include <mongocxx/events/command_started_event.hpp>
#include <mongocxx/events/command_succeeded_event.hpp>
#include <mongocxx/events/heartbeat_failed_event.hpp>
#include <mongocxx/events/heartbeat_started_event.hpp>
#include <mongocxx/events/heartbeat_succeeded_event.hpp>
#include <mongocxx/events/server_changed_event.hpp>
#include <mongocxx/events/server_closed_event.hpp>
#include <mongocxx/events/server_opening_event.hpp>
#include <mongocxx/events/topology_changed_event.hpp>
#include <mongocxx/events/topology_closed_event.hpp>
#include <mongocxx/events/topology_opening_event.hpp>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

namespace {

template <typename T>
using fn_type = std::function<void MONGOCXX_ABI_CDECL(T const&)>;

} // namespace

TEST_CASE("v1", "[mongocxx][v_noabi][options][apm]") {
    using command_failed_type_v_noabi = v_noabi::events::command_failed_event;
    using command_started_type_v_noabi = v_noabi::events::command_started_event;
    using command_succeeded_type_v_noabi = v_noabi::events::command_succeeded_event;
    using server_closed_type_v_noabi = v_noabi::events::server_closed_event;
    using server_description_changed_type_v_noabi = v_noabi::events::server_changed_event;
    using server_heartbeat_failed_type_v_noabi = v_noabi::events::heartbeat_failed_event;
    using server_heartbeat_started_type_v_noabi = v_noabi::events::heartbeat_started_event;
    using server_heartbeat_succeeded_type_v_noabi = v_noabi::events::heartbeat_succeeded_event;
    using server_opening_type_v_noabi = v_noabi::events::server_opening_event;
    using topology_closed_type_v_noabi = v_noabi::events::topology_closed_event;
    using topology_description_changed_type_v_noabi = v_noabi::events::topology_changed_event;
    using topology_opening_type_v_noabi = v_noabi::events::topology_opening_event;

    using command_failed_type_v1 = v1::events::command_failed;
    using command_started_type_v1 = v1::events::command_started;
    using command_succeeded_type_v1 = v1::events::command_succeeded;
    using server_closed_type_v1 = v1::events::server_closed;
    using server_description_changed_type_v1 = v1::events::server_description_changed;
    using server_heartbeat_failed_type_v1 = v1::events::server_heartbeat_failed;
    using server_heartbeat_started_type_v1 = v1::events::server_heartbeat_started;
    using server_heartbeat_succeeded_type_v1 = v1::events::server_heartbeat_succeeded;
    using server_opening_type_v1 = v1::events::server_opening;
    using topology_closed_type_v1 = v1::events::topology_closed;
    using topology_description_changed_type_v1 = v1::events::topology_description_changed;
    using topology_opening_type_v1 = v1::events::topology_opening;

    struct identity_type {};

    identity_type command_failed_identity;
    identity_type command_started_identity;
    identity_type command_succeeded_identity;
    identity_type server_closed_identity;
    identity_type server_description_changed_identity;
    identity_type server_heartbeat_failed_identity;
    identity_type server_heartbeat_started_identity;
    identity_type server_heartbeat_succeeded_identity;
    identity_type server_opening_identity;
    identity_type topology_closed_identity;
    identity_type topology_description_changed_identity;
    identity_type topology_opening_identity;

    // clang-format off
    auto const command_failed_id = reinterpret_cast<mongoc_apm_command_failed_t*>(&command_failed_identity);
    auto const command_started_id = reinterpret_cast<mongoc_apm_command_started_t*>(&command_started_identity);
    auto const command_succeeded_id = reinterpret_cast<mongoc_apm_command_succeeded_t*>(&command_succeeded_identity);
    auto const server_closed_id = reinterpret_cast<mongoc_apm_server_closed_t*>(&server_closed_identity);
    auto const server_description_changed_id = reinterpret_cast<mongoc_apm_server_changed_t*>(&server_description_changed_identity);
    auto const server_heartbeat_failed_id = reinterpret_cast<mongoc_apm_server_heartbeat_failed_t*>(&server_heartbeat_failed_identity);
    auto const server_heartbeat_started_id = reinterpret_cast<mongoc_apm_server_heartbeat_started_t*>(&server_heartbeat_started_identity);
    auto const server_heartbeat_succeeded_id = reinterpret_cast<mongoc_apm_server_heartbeat_succeeded_t*>(&server_heartbeat_succeeded_identity);
    auto const server_opening_id = reinterpret_cast<mongoc_apm_server_opening_t*>(&server_opening_identity);
    auto const topology_closed_id = reinterpret_cast<mongoc_apm_topology_closed_t*>(&topology_closed_identity);
    auto const topology_description_changed_id = reinterpret_cast<mongoc_apm_topology_changed_t*>(&topology_description_changed_identity);
    auto const topology_opening_id = reinterpret_cast<mongoc_apm_topology_opening_t*>(&topology_opening_identity);
    // clang-format on

    // clang-format off
    auto const command_failed_event = command_failed_type_v1::internal::make(command_failed_id);
    auto const command_started_event = command_started_type_v1::internal::make(command_started_id);
    auto const command_succeeded_event = command_succeeded_type_v1::internal::make(command_succeeded_id);
    auto const server_closed_event = server_closed_type_v1::internal::make(server_closed_id);
    auto const server_description_changed_event = server_description_changed_type_v1::internal::make(server_description_changed_id);
    auto const server_heartbeat_failed_event = server_heartbeat_failed_type_v1::internal::make(server_heartbeat_failed_id);
    auto const server_heartbeat_started_event = server_heartbeat_started_type_v1::internal::make(server_heartbeat_started_id);
    auto const server_heartbeat_succeeded_event = server_heartbeat_succeeded_type_v1::internal::make(server_heartbeat_succeeded_id);
    auto const server_opening_event = server_opening_type_v1::internal::make(server_opening_id);
    auto const topology_closed_event = topology_closed_type_v1::internal::make(topology_closed_id);
    auto const topology_description_changed_event =topology_description_changed_type_v1::internal::make(topology_description_changed_id);
    auto const topology_opening_event = topology_opening_type_v1::internal::make(topology_opening_id);
    // clang-format on

    int command_failed_count = 0;
    int command_started_count = 0;
    int command_succeeded_count = 0;
    int server_closed_count = 0;
    int server_description_changed_count = 0;
    int server_heartbeat_failed_count = 0;
    int server_heartbeat_started_count = 0;
    int server_heartbeat_succeeded_count = 0;
    int server_opening_count = 0;
    int topology_closed_count = 0;
    int topology_description_changed_count = 0;
    int topology_opening_count = 0;

    // clang-format off
    auto const command_failed_v_noabi = [&](command_failed_type_v_noabi const& event) {
        command_failed_type_v1 v1{event};
        CHECK(command_failed_type_v1::internal::as_mongoc(v1) == command_failed_id);
        ++command_failed_count;
    };
    auto const command_started_v_noabi = [&](command_started_type_v_noabi const& event) {
        command_started_type_v1 v1{event};
        CHECK(command_started_type_v1::internal::as_mongoc(v1) == command_started_id);
        ++command_started_count;
    };
    auto const command_succeeded_v_noabi = [&](command_succeeded_type_v_noabi const& event) {
        command_succeeded_type_v1 v1{event};
        CHECK(command_succeeded_type_v1::internal::as_mongoc(v1) == command_succeeded_id);
        ++command_succeeded_count;
    };
    auto const server_closed_v_noabi = [&](server_closed_type_v_noabi const& event) {
        server_closed_type_v1 v1{event};
        CHECK(server_closed_type_v1::internal::as_mongoc(v1) == server_closed_id);
        ++server_closed_count;
    };
    auto const server_description_changed_v_noabi = [&](server_description_changed_type_v_noabi const& event) {
        server_description_changed_type_v1 v1{event};
        CHECK(server_description_changed_type_v1::internal::as_mongoc(v1) == server_description_changed_id);
        ++server_description_changed_count;
    };
    auto const server_heartbeat_failed_v_noabi = [&](server_heartbeat_failed_type_v_noabi const& event) {
        server_heartbeat_failed_type_v1 v1{event};
        CHECK(server_heartbeat_failed_type_v1::internal::as_mongoc(v1) == server_heartbeat_failed_id);
        ++server_heartbeat_failed_count;
    };
    auto const server_heartbeat_started_v_noabi = [&](server_heartbeat_started_type_v_noabi const& event) {
        server_heartbeat_started_type_v1 v1{event};
        CHECK(server_heartbeat_started_type_v1::internal::as_mongoc(v1) == server_heartbeat_started_id);
        ++server_heartbeat_started_count;
    };
    auto const server_heartbeat_succeeded_v_noabi = [&](server_heartbeat_succeeded_type_v_noabi const& event) {
        server_heartbeat_succeeded_type_v1 v1{event};
        CHECK(server_heartbeat_succeeded_type_v1::internal::as_mongoc(v1) == server_heartbeat_succeeded_id);
        ++server_heartbeat_succeeded_count;
    };
    auto const server_opening_v_noabi = [&](server_opening_type_v_noabi const& event) {
        server_opening_type_v1 v1{event};
        CHECK(server_opening_type_v1::internal::as_mongoc(v1) == server_opening_id);
        ++server_opening_count;
    };
    auto const topology_closed_v_noabi = [&](topology_closed_type_v_noabi const& event) {
        topology_closed_type_v1 v1{event};
        CHECK(topology_closed_type_v1::internal::as_mongoc(v1) == topology_closed_id);
        ++topology_closed_count;
    };
    auto const topology_description_changed_v_noabi = [&](topology_description_changed_type_v_noabi const& event) {
        topology_description_changed_type_v1 v1{event};
        CHECK(topology_description_changed_type_v1::internal::as_mongoc(v1) == topology_description_changed_id);
        ++topology_description_changed_count;
    };
    auto const topology_opening_v_noabi = [&](topology_opening_type_v_noabi const& event) {
        topology_opening_type_v1 v1{event};
        CHECK(topology_opening_type_v1::internal::as_mongoc(v1) == topology_opening_id);
        ++topology_opening_count;
    };
    // clang-format on

    // clang-format off
    auto command_failed_v1 = [&](command_failed_type_v1 const& event) {
        CHECK(command_failed_type_v1::internal::as_mongoc(event) == command_failed_id);
        ++command_failed_count;
    };
    auto command_started_v1 = [&](command_started_type_v1 const& event) {
        CHECK(command_started_type_v1::internal::as_mongoc(event) == command_started_id);
        ++command_started_count;
    };
    auto command_succeeded_v1 = [&](command_succeeded_type_v1 const& event) {
        CHECK(command_succeeded_type_v1::internal::as_mongoc(event) == command_succeeded_id);
        ++command_succeeded_count;
    };
    auto server_closed_v1 = [&](server_closed_type_v1 const& event) {
        CHECK(server_closed_type_v1::internal::as_mongoc(event) == server_closed_id);
        ++server_closed_count;
    };
    auto server_description_changed_v1 = [&](server_description_changed_type_v1 const& event) {
        CHECK(server_description_changed_type_v1::internal::as_mongoc(event) == server_description_changed_id);
        ++server_description_changed_count;
    };
    auto server_heartbeat_failed_v1 = [&](server_heartbeat_failed_type_v1 const& event) {
        CHECK(server_heartbeat_failed_type_v1::internal::as_mongoc(event) == server_heartbeat_failed_id);
        ++server_heartbeat_failed_count;
    };
    auto server_heartbeat_started_v1 = [&](server_heartbeat_started_type_v1 const& event) {
        CHECK(server_heartbeat_started_type_v1::internal::as_mongoc(event) == server_heartbeat_started_id);
        ++server_heartbeat_started_count;
    };
    auto server_heartbeat_succeeded_v1 = [&](server_heartbeat_succeeded_type_v1 const& event) {
        CHECK(server_heartbeat_succeeded_type_v1::internal::as_mongoc(event) == server_heartbeat_succeeded_id);
        ++server_heartbeat_succeeded_count;
    };
    auto server_opening_v1 = [&](server_opening_type_v1 const& event) {
        CHECK(server_opening_type_v1::internal::as_mongoc(event) == server_opening_id);
        ++server_opening_count;
    };
    auto topology_closed_v1 = [&](topology_closed_type_v1 const& event) {
        CHECK(topology_closed_type_v1::internal::as_mongoc(event) == topology_closed_id);
        ++topology_closed_count;
    };
    auto topology_description_changed_v1 = [&](topology_description_changed_type_v1 const& event) {
        CHECK(topology_description_changed_type_v1::internal::as_mongoc(event) == topology_description_changed_id);
        ++topology_description_changed_count;
    };
    auto topology_opening_v1 = [&](topology_opening_type_v1 const& event) {
        CHECK(topology_opening_type_v1::internal::as_mongoc(event) == topology_opening_id);
        ++topology_opening_count;
    };
    // clang-format on

    using v_noabi = v_noabi::options::apm;
    using v1 = v1::apm;

    SECTION("from_v1") {
        v1 from;

        from.on_command_failed(std::ref(command_failed_v1));
        from.on_command_started(std::ref(command_started_v1));
        from.on_command_succeeded(std::ref(command_succeeded_v1));
        from.on_server_closed(std::ref(server_closed_v1));
        from.on_server_description_changed(std::ref(server_description_changed_v1));
        from.on_server_heartbeat_failed(std::ref(server_heartbeat_failed_v1));
        from.on_server_heartbeat_started(std::ref(server_heartbeat_started_v1));
        from.on_server_heartbeat_succeeded(std::ref(server_heartbeat_succeeded_v1));
        from.on_server_opening(std::ref(server_opening_v1));
        from.on_topology_closed(std::ref(topology_closed_v1));
        from.on_topology_description_changed(std::ref(topology_description_changed_v1));
        from.on_topology_opening(std::ref(topology_opening_v1));

        v_noabi const to{std::move(from)};

        to.command_failed()(command_failed_event);
        to.command_started()(command_started_event);
        to.command_succeeded()(command_succeeded_event);
        to.server_closed()(server_closed_event);
        to.server_changed()(server_description_changed_event);
        to.heartbeat_failed()(server_heartbeat_failed_event);
        to.heartbeat_started()(server_heartbeat_started_event);
        to.heartbeat_succeeded()(server_heartbeat_succeeded_event);
        to.server_opening()(server_opening_event);
        to.topology_closed()(topology_closed_event);
        to.topology_changed()(topology_description_changed_event);
        to.topology_opening()(topology_opening_event);

        CHECK(command_failed_count == 1);
        CHECK(command_started_count == 1);
        CHECK(command_succeeded_count == 1);
        CHECK(server_closed_count == 1);
        CHECK(server_description_changed_count == 1);
        CHECK(server_heartbeat_failed_count == 1);
        CHECK(server_heartbeat_started_count == 1);
        CHECK(server_heartbeat_succeeded_count == 1);
        CHECK(server_opening_count == 1);
        CHECK(topology_closed_count == 1);
        CHECK(topology_description_changed_count == 1);
        CHECK(topology_opening_count == 1);
    }

    SECTION("to_v1") {
        v_noabi from;

        from.on_command_failed(std::ref(command_failed_v_noabi));
        from.on_command_started(std::ref(command_started_v_noabi));
        from.on_command_succeeded(std::ref(command_succeeded_v_noabi));
        from.on_server_closed(std::ref(server_closed_v_noabi));
        from.on_server_changed(std::ref(server_description_changed_v_noabi));
        from.on_heartbeat_failed(std::ref(server_heartbeat_failed_v_noabi));
        from.on_heartbeat_started(std::ref(server_heartbeat_started_v_noabi));
        from.on_heartbeat_succeeded(std::ref(server_heartbeat_succeeded_v_noabi));
        from.on_server_opening(std::ref(server_opening_v_noabi));
        from.on_topology_closed(std::ref(topology_closed_v_noabi));
        from.on_topology_changed(std::ref(topology_description_changed_v_noabi));
        from.on_topology_opening(std::ref(topology_opening_v_noabi));

        v1 const to{std::move(from)};

        to.command_failed()(command_failed_event);
        to.command_started()(command_started_event);
        to.command_succeeded()(command_succeeded_event);
        to.server_closed()(server_closed_event);
        to.server_description_changed()(server_description_changed_event);
        to.server_heartbeat_failed()(server_heartbeat_failed_event);
        to.server_heartbeat_started()(server_heartbeat_started_event);
        to.server_heartbeat_succeeded()(server_heartbeat_succeeded_event);
        to.server_opening()(server_opening_event);
        to.topology_closed()(topology_closed_event);
        to.topology_description_changed()(topology_description_changed_event);
        to.topology_opening()(topology_opening_event);

        CHECK(command_failed_count == 1);
        CHECK(command_started_count == 1);
        CHECK(command_succeeded_count == 1);
        CHECK(server_closed_count == 1);
        CHECK(server_description_changed_count == 1);
        CHECK(server_heartbeat_failed_count == 1);
        CHECK(server_heartbeat_started_count == 1);
        CHECK(server_heartbeat_succeeded_count == 1);
        CHECK(server_opening_count == 1);
        CHECK(topology_closed_count == 1);
        CHECK(topology_description_changed_count == 1);
        CHECK(topology_opening_count == 1);
    }
}

} // namespace mongocxx
