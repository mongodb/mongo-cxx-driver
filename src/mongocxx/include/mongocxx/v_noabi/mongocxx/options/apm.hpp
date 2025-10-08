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

#include <functional>

#include <mongocxx/options/apm-fwd.hpp>

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

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::client::apm_opts.
///
/// @see
/// - [Command Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/command-logging-and-monitoring/command-logging-and-monitoring/)
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/#events-api_1)
///
class apm {
   public:
    ///
    /// Set the command started monitoring callback. The callback takes a reference to a
    /// command_started_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param command_started
    ///   The command started monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_command_started(std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> command_started);

    ///
    /// Retrieves the command started monitoring callback.
    ///
    /// @return The command started monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> const&)
    command_started() const;

    ///
    /// Set the command failed monitoring callback. The callback takes a reference to a
    /// command_failed_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param command_failed
    ///   The command failed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_command_failed(std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> command_failed);

    ///
    /// Retrieves the command failed monitoring callback.
    ///
    /// @return The command failed monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> const&)
    command_failed() const;

    ///
    /// Set the command succeeded monitoring callback. The callback takes a reference to a
    /// command_succeeded_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param command_succeeded
    ///   The command succeeded monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_command_succeeded(
        std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> command_succeeded);

    ///
    /// Retrieves the command succeeded monitoring callback.
    ///
    /// @return The command succeeded monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> const&)
    command_succeeded() const;

    ///
    /// Set the server opening monitoring callback. The callback takes a reference to a
    /// server_opening_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param server_opening
    ///   The server opening monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_server_opening(std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> server_opening);

    ///
    /// Retrieves the server opening monitoring callback.
    ///
    /// @return The server opening monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> const&)
    server_opening() const;

    ///
    /// Set the server closed monitoring callback. The callback takes a reference to a
    /// server_closed_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param server_closed
    ///   The server closed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_server_closed(std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> server_closed);

    ///
    /// Retrieves the server closed monitoring callback.
    ///
    /// @return The server closed monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> const&)
    server_closed() const;

    ///
    /// Set the server description changed monitoring callback. The callback takes a reference to a
    /// server_changed_event which will only contain valid data for the duration of the
    /// callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param server_changed
    ///   The server description changed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_server_changed(std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> server_changed);

    ///
    /// Retrieves the server description changed monitoring callback.
    ///
    /// @return The server description changed monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> const&)
    server_changed() const;

    ///
    /// Set the topology_opening monitoring callback. The callback takes a reference to a
    /// topology_opening_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param topology_opening
    ///   The topology_opening monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_topology_opening(std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> topology_opening);

    ///
    /// Retrieves the topology_opening monitoring callback.
    ///
    /// @return The topology_opening monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> const&)
    topology_opening() const;

    ///
    /// Set the topology closed monitoring callback. The callback takes a reference to a
    /// topology_closed_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param topology_closed
    ///   The topology closed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_topology_closed(std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> topology_closed);

    ///
    /// Retrieves the topology closed monitoring callback.
    ///
    /// @return The topology closed monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> const&)
    topology_closed() const;

    ///
    /// Set the topology description changed monitoring callback. The callback takes a reference to
    /// a topology_changed_event which will only contain valid data for the duration of
    /// the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param topology_changed
    ///   The topology description changed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_topology_changed(std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> topology_changed);

    ///
    /// Retrieves the topology description changed monitoring callback.
    ///
    /// @return The topology description changed monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> const&)
    topology_changed() const;

    ///
    /// Set the heartbeat started monitoring callback. The callback takes a reference to a
    /// heartbeat_started_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param heartbeat_started
    ///   The heartbeat started monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_heartbeat_started(
        std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> heartbeat_started);

    ///
    /// Retrieves the heartbeat started monitoring callback.
    ///
    /// @return The heartbeat started monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> const&)
    heartbeat_started() const;

    ///
    /// Set the heartbeat failed monitoring callback. The callback takes a reference to a
    /// heartbeat_failed_event which will only contain valid data for the duration of the callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param heartbeat_failed
    ///   The heartbeat failed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_heartbeat_failed(std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> heartbeat_failed);

    ///
    /// Retrieves the heartbeat failed monitoring callback.
    ///
    /// @return The heartbeat failed monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> const&)
    heartbeat_failed() const;

    ///
    /// Set the heartbeat succeeded monitoring callback. The callback takes a reference to a
    /// heartbeat_succeeded_event which will only contain valid data for the duration of the
    /// callback.
    ///
    /// @warning
    ///   If the callback throws an exception, the behavior is undefined.
    ///
    /// @param heartbeat_succeeded
    ///   The heartbeat succeeded monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(apm&)
    on_heartbeat_succeeded(
        std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> heartbeat_succeeded);

    ///
    /// Retrieves the heartbeat succeeded monitoring callback.
    ///
    /// @return The heartbeat succeeded monitoring callback.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> const&)
    heartbeat_succeeded() const;

   private:
    std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> _command_started;
    std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> _command_failed;
    std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> _command_succeeded;
    std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> _server_closed;
    std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> _server_changed;
    std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> _server_opening;
    std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> _topology_closed;
    std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> _topology_changed;
    std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> _topology_opening;
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> _heartbeat_started;
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> _heartbeat_failed;
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> _heartbeat_succeeded;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::apm.
///
