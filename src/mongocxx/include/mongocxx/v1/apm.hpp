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

#pragma once

#include <mongocxx/v1/apm-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/events/command_failed-fwd.hpp>
#include <mongocxx/v1/events/command_started-fwd.hpp>
#include <mongocxx/v1/events/command_succeeded-fwd.hpp>
#include <mongocxx/v1/events/server_closed-fwd.hpp>
#include <mongocxx/v1/events/server_description_changed-fwd.hpp>
#include <mongocxx/v1/events/server_heartbeat_failed-fwd.hpp>
#include <mongocxx/v1/events/server_heartbeat_started-fwd.hpp>
#include <mongocxx/v1/events/server_heartbeat_succeeded-fwd.hpp>
#include <mongocxx/v1/events/server_opening-fwd.hpp>
#include <mongocxx/v1/events/topology_closed-fwd.hpp>
#include <mongocxx/v1/events/topology_description_changed-fwd.hpp>
#include <mongocxx/v1/events/topology_opening-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <functional>

namespace mongocxx {
namespace v1 {

///
/// Options related to Application Performance Monitoring (APM) configuration.
///
/// @see
/// - [Application Performance Monitoring (mongoc)](https://mongoc.org/libmongoc/current/application-performance-monitoring.html)
/// - [Command Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/command-logging-and-monitoring/command-logging-and-monitoring/)
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
class apm {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~apm();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() apm(apm&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) operator=(apm&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() apm(apm const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) operator=(apm const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All event handlers are initialized as "empty".
    ///
    MONGOCXX_ABI_EXPORT_CDECL() apm();

    ///
    /// Set the "CommandStartedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_command_started(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::command_started const&)> fn);

    ///
    /// Return the current "CommandStartedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::command_started const&)>)
    command_started() const;

    ///
    /// Set the "CommandFailedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_command_failed(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::command_failed const&)> fn);

    ///
    /// Return the current the "CommandFailedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::command_failed const&)>)
    command_failed() const;

    ///
    /// Set the "CommandSucceededEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_command_succeeded(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::command_succeeded const&)> fn);

    ///
    /// Return the current the "CommandSucceededEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::command_succeeded const&)>)
    command_succeeded() const;

    ///
    /// Set the "ServerOpeningEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_server_opening(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::server_opening const&)> fn);

    ///
    /// Return the current the "ServerOpeningEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_opening const&)>)
    server_opening() const;

    ///
    /// Set the "ServerClosedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_server_closed(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::server_closed const&)> fn);

    ///
    /// Return the current the "ServerClosedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_closed const&)>)
    server_closed() const;

    ///
    /// Set the "ServerDescriptionChangedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_server_description_changed(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::server_description_changed const&)> fn);

    ///
    /// Return the current the "ServerDescriptionChangedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_description_changed const&)>)
    server_description_changed() const;

    ///
    /// Set the "TopologyOpeningEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_topology_opening(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_opening const&)> fn);

    ///
    /// Return the current the "TopologyOpeningEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_opening const&)>)
    topology_opening() const;

    ///
    /// Set the "TopologyClosedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_topology_closed(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_closed const&)> fn);

    ///
    /// Return the current the "TopologyClosedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_closed const&)>)
    topology_closed() const;

    ///
    /// Set the "TopologyDescriptionChangedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_topology_description_changed(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_description_changed const&)> fn);

    ///
    /// Return the current the "TopologyDescriptionChangedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_description_changed const&)>)
    topology_description_changed() const;

    ///
    /// Set the "ServerHeartbeatStartedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_server_heartbeat_started(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_started const&)> fn);

    ///
    /// Return the current the "ServerHeartbeatStartedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_started const&)>)
    server_heartbeat_started() const;

    ///
    /// Set the "ServerHeartbeatFailedEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_server_heartbeat_failed(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_failed const&)> fn);

    ///
    /// Return the current the "ServerHeartbeatFailedEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_failed const&)>)
    server_heartbeat_failed() const;

    ///
    /// Set the "ServerHeartbeatSucceededEvent" handler.
    ///
    /// @warning It is undefined behavior for the handler to throw an exception.
    ///
    /// @par Preconditions:
    /// - `fn` MUST NOT throw an exception when invoked.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&) on_server_heartbeat_succeeded(
        std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_succeeded const&)> fn);

    ///
    /// Return the current the "ServerHeartbeatSucceededEvent" handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_succeeded const&)>)
    server_heartbeat_succeeded() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::apm.
///
