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

namespace mongocxx {
namespace v_noabi {
namespace options {

apm& apm::on_command_started(
    std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> command_started) {
    _command_started = std::move(command_started);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> const& apm::command_started() const {
    return _command_started;
}

apm& apm::on_command_failed(
    std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> command_failed) {
    _command_failed = std::move(command_failed);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> const& apm::command_failed() const {
    return _command_failed;
}

apm& apm::on_command_succeeded(
    std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> command_succeeded) {
    _command_succeeded = std::move(command_succeeded);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> const& apm::command_succeeded() const {
    return _command_succeeded;
}

apm& apm::on_server_opening(
    std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> server_opening) {
    _server_opening = server_opening;
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> const& apm::server_opening() const {
    return _server_opening;
}

apm& apm::on_server_closed(std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> server_closed) {
    _server_closed = server_closed;
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> const& apm::server_closed() const {
    return _server_closed;
}

apm& apm::on_server_changed(
    std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> server_changed) {
    _server_changed = server_changed;
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> const& apm::server_changed() const {
    return _server_changed;
}

apm& apm::on_topology_opening(
    std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> topology_opening) {
    _topology_opening = topology_opening;
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> const& apm::topology_opening() const {
    return _topology_opening;
}

apm& apm::on_topology_closed(
    std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> topology_closed) {
    _topology_closed = topology_closed;
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> const& apm::topology_closed() const {
    return _topology_closed;
}

apm& apm::on_topology_changed(
    std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> topology_changed) {
    _topology_changed = topology_changed;
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> const& apm::topology_changed() const {
    return _topology_changed;
}

apm& apm::on_heartbeat_started(
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> heartbeat_started) {
    _heartbeat_started = std::move(heartbeat_started);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> const& apm::heartbeat_started() const {
    return _heartbeat_started;
}

apm& apm::on_heartbeat_failed(
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> heartbeat_failed) {
    _heartbeat_failed = std::move(heartbeat_failed);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> const& apm::heartbeat_failed() const {
    return _heartbeat_failed;
}

apm& apm::on_heartbeat_succeeded(
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> heartbeat_succeeded) {
    _heartbeat_succeeded = std::move(heartbeat_succeeded);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> const& apm::heartbeat_succeeded()
    const {
    return _heartbeat_succeeded;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
