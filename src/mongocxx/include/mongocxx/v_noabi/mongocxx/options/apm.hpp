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

#include <mongocxx/options/apm-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/apm.hpp> // IWYU pragma: export

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
    /// Default initialization.
    ///
    apm() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() apm(v1::apm other);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::apm() const {
        v1::apm ret;

        ret.on_command_started(_command_started);
        ret.on_command_failed(_command_failed);
        ret.on_command_succeeded(_command_succeeded);
        ret.on_server_closed(_server_closed);
        ret.on_server_description_changed(_server_changed);
        ret.on_server_opening(_server_opening);
        ret.on_topology_closed(_topology_closed);
        ret.on_topology_description_changed(_topology_changed);
        ret.on_topology_opening(_topology_opening);
        ret.on_server_heartbeat_started(_heartbeat_started);
        ret.on_server_heartbeat_failed(_heartbeat_failed);
        ret.on_server_heartbeat_succeeded(_heartbeat_succeeded);

        return ret;
    }

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
    apm& on_command_started(
        std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> command_started) {
        _command_started = std::move(command_started);
        return *this;
    }

    ///
    /// Retrieves the command started monitoring callback.
    ///
    /// @return The command started monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::command_started_event const&)> const& command_started() const {
        return _command_started;
    }

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
    apm& on_command_failed(std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> command_failed) {
        _command_failed = std::move(command_failed);
        return *this;
    }

    ///
    /// Retrieves the command failed monitoring callback.
    ///
    /// @return The command failed monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::command_failed_event const&)> const& command_failed() const {
        return _command_failed;
    }

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
    apm& on_command_succeeded(
        std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> command_succeeded) {
        _command_succeeded = std::move(command_succeeded);
        return *this;
    }

    ///
    /// Retrieves the command succeeded monitoring callback.
    ///
    /// @return The command succeeded monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::command_succeeded_event const&)> const& command_succeeded() const {
        return _command_succeeded;
    }

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
    apm& on_server_opening(std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> server_opening) {
        _server_opening = std::move(server_opening);
        return *this;
    }

    ///
    /// Retrieves the server opening monitoring callback.
    ///
    /// @return The server opening monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::server_opening_event const&)> const& server_opening() const {
        return _server_opening;
    }

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
    apm& on_server_closed(std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> server_closed) {
        _server_closed = std::move(server_closed);
        return *this;
    }

    ///
    /// Retrieves the server closed monitoring callback.
    ///
    /// @return The server closed monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::server_closed_event const&)> const& server_closed() const {
        return _server_closed;
    }

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
    apm& on_server_changed(std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> server_changed) {
        _server_changed = std::move(server_changed);
        return *this;
    }

    ///
    /// Retrieves the server description changed monitoring callback.
    ///
    /// @return The server description changed monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::server_changed_event const&)> const& server_changed() const {
        return _server_changed;
    }

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
    apm& on_topology_opening(
        std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> topology_opening) {
        _topology_opening = std::move(topology_opening);
        return *this;
    }

    ///
    /// Retrieves the topology_opening monitoring callback.
    ///
    /// @return The topology_opening monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::topology_opening_event const&)> const& topology_opening() const {
        return _topology_opening;
    }

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
    apm& on_topology_closed(
        std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> topology_closed) {
        _topology_closed = std::move(topology_closed);
        return *this;
    }

    ///
    /// Retrieves the topology closed monitoring callback.
    ///
    /// @return The topology closed monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::topology_closed_event const&)> const& topology_closed() const {
        return _topology_closed;
    }

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
    apm& on_topology_changed(
        std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> topology_changed) {
        _topology_changed = std::move(topology_changed);
        return *this;
    }

    ///
    /// Retrieves the topology description changed monitoring callback.
    ///
    /// @return The topology description changed monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::topology_changed_event const&)> const& topology_changed() const {
        return _topology_changed;
    }

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
    apm& on_heartbeat_started(
        std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> heartbeat_started) {
        _heartbeat_started = std::move(heartbeat_started);
        return *this;
    }

    ///
    /// Retrieves the heartbeat started monitoring callback.
    ///
    /// @return The heartbeat started monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_started_event const&)> const& heartbeat_started() const {
        return _heartbeat_started;
    }

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
    apm& on_heartbeat_failed(
        std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> heartbeat_failed) {
        _heartbeat_failed = std::move(heartbeat_failed);
        return *this;
    }

    ///
    /// Retrieves the heartbeat failed monitoring callback.
    ///
    /// @return The heartbeat failed monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_failed_event const&)> const& heartbeat_failed() const {
        return _heartbeat_failed;
    }

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
    apm& on_heartbeat_succeeded(
        std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> heartbeat_succeeded) {
        _heartbeat_succeeded = std::move(heartbeat_succeeded);
        return *this;
    }

    ///
    /// Retrieves the heartbeat succeeded monitoring callback.
    ///
    /// @return The heartbeat succeeded monitoring callback.
    ///
    std::function<void MONGOCXX_ABI_CDECL(events::heartbeat_succeeded_event const&)> const& heartbeat_succeeded()
        const {
        return _heartbeat_succeeded;
    }

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

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::apm from_v1(v1::apm v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::apm to_v1(v_noabi::options::apm const& v) {
    return v1::apm{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::apm.
///
/// @par Includes
/// - @ref mongocxx/v1/apm.hpp
///
