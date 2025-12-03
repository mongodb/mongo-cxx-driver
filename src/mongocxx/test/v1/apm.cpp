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

#include <mongocxx/v1/apm.hpp>

//

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/events/command_failed.hpp>
#include <mongocxx/v1/events/command_started.hpp>
#include <mongocxx/v1/events/command_succeeded.hpp>
#include <mongocxx/v1/events/server_closed.hpp>
#include <mongocxx/v1/events/server_description_changed.hpp>
#include <mongocxx/v1/events/server_heartbeat_failed.hpp>
#include <mongocxx/v1/events/server_heartbeat_started.hpp>
#include <mongocxx/v1/events/server_heartbeat_succeeded.hpp>
#include <mongocxx/v1/events/server_opening.hpp>
#include <mongocxx/v1/events/topology_closed.hpp>
#include <mongocxx/v1/events/topology_description_changed.hpp>
#include <mongocxx/v1/events/topology_opening.hpp>

#include <bsoncxx/test/v1/stdx/optional.hh>

#include <functional>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace {

template <typename T>
bsoncxx::v1::stdx::optional<void(MONGOCXX_ABI_CDECL*)(T const&)> get_target(
    std::function<void MONGOCXX_ABI_CDECL(T const&)> const& fn) {
    if (auto const fn_ptr = fn.template target<void(MONGOCXX_ABI_CDECL*)(T const&)>()) {
        return *fn_ptr;
    }
    return {};
}

} // namespace

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][apm]") {
    apm source;
    apm target;

    // Workaround inability to declare calling convention for lambda expressions.
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::command_started const&);

    fn_type const source_value = [](v1::events::command_started const&) {};
    fn_type const target_value = [](v1::events::command_started const&) {};

    source.on_command_started(source_value);
    target.on_command_started(target_value);

    REQUIRE(get_target(source.command_started()) == source_value);
    REQUIRE(get_target(target.command_started()) == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(get_target(move.command_started()) == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(get_target(target.command_started()) == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(get_target(source.command_started()) == source_value);
        CHECK(get_target(copy.command_started()) == source_value);

        target = copy;

        CHECK(get_target(copy.command_started()) == source_value);
        CHECK(get_target(target.command_started()) == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][apm]") {
    apm const fns;

    CHECK_FALSE(fns.command_started());
    CHECK_FALSE(fns.command_failed());
    CHECK_FALSE(fns.command_succeeded());
    CHECK_FALSE(fns.server_closed());
    CHECK_FALSE(fns.server_description_changed());
    CHECK_FALSE(fns.server_opening());
    CHECK_FALSE(fns.topology_closed());
    CHECK_FALSE(fns.topology_description_changed());
    CHECK_FALSE(fns.topology_opening());
    CHECK_FALSE(fns.server_heartbeat_started());
    CHECK_FALSE(fns.server_heartbeat_failed());
    CHECK_FALSE(fns.server_heartbeat_succeeded());
}

TEST_CASE("command_started", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::command_started const&);
    fn_type const fn = [](v1::events::command_started const&) {};
    CHECK(get_target(apm{}.on_command_started(fn).command_started()) == fn);
}

TEST_CASE("command_failed", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::command_failed const&);
    fn_type const fn = [](v1::events::command_failed const&) {};
    CHECK(get_target(apm{}.on_command_failed(fn).command_failed()) == fn);
}

TEST_CASE("command_succeeded", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::command_succeeded const&);
    fn_type const fn = [](v1::events::command_succeeded const&) {};
    CHECK(get_target(apm{}.on_command_succeeded(fn).command_succeeded()) == fn);
}

TEST_CASE("server_closed", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::server_closed const&);
    fn_type const fn = [](v1::events::server_closed const&) {};
    CHECK(get_target(apm{}.on_server_closed(fn).server_closed()) == fn);
}

TEST_CASE("server_description_changed", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::server_description_changed const&);
    fn_type const fn = [](v1::events::server_description_changed const&) {};
    CHECK(get_target(apm{}.on_server_description_changed(fn).server_description_changed()) == fn);
}

TEST_CASE("server_opening", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::server_opening const&);
    fn_type const fn = [](v1::events::server_opening const&) {};
    CHECK(get_target(apm{}.on_server_opening(fn).server_opening()) == fn);
}

TEST_CASE("topology_closed", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::topology_closed const&);
    fn_type const fn = [](v1::events::topology_closed const&) {};
    CHECK(get_target(apm{}.on_topology_closed(fn).topology_closed()) == fn);
}

TEST_CASE("topology_description_changed", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::topology_description_changed const&);
    fn_type const fn = [](v1::events::topology_description_changed const&) {};
    CHECK(get_target(apm{}.on_topology_description_changed(fn).topology_description_changed()) == fn);
}

TEST_CASE("topology_opening", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::topology_opening const&);
    fn_type const fn = [](v1::events::topology_opening const&) {};
    CHECK(get_target(apm{}.on_topology_opening(fn).topology_opening()) == fn);
}

TEST_CASE("server_heartbeat_started", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::server_heartbeat_started const&);
    fn_type const fn = [](v1::events::server_heartbeat_started const&) {};
    CHECK(get_target(apm{}.on_server_heartbeat_started(fn).server_heartbeat_started()) == fn);
}

TEST_CASE("server_heartbeat_failed", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::server_heartbeat_failed const&);
    fn_type const fn = [](v1::events::server_heartbeat_failed const&) {};
    CHECK(get_target(apm{}.on_server_heartbeat_failed(fn).server_heartbeat_failed()) == fn);
}

TEST_CASE("server_heartbeat_succeeded", "[mongocxx][v1][apm]") {
    using fn_type = void(MONGOCXX_ABI_CDECL*)(v1::events::server_heartbeat_succeeded const&);
    fn_type const fn = [](v1::events::server_heartbeat_succeeded const&) {};
    CHECK(get_target(apm{}.on_server_heartbeat_succeeded(fn).server_heartbeat_succeeded()) == fn);
}

} // namespace v1
} // namespace mongocxx
