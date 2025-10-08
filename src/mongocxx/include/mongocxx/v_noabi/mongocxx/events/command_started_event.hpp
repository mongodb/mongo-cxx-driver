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

#include <memory>

#include <mongocxx/events/command_started_event-fwd.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// The start of the execution of a MongoDB command.
///
/// @see
/// - [Command Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/command-logging-and-monitoring/command-logging-and-monitoring/)
///
class command_started_event {
   public:
    explicit command_started_event(void const* event);

    ///
    /// Destroys a command_started_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~command_started_event();

    command_started_event(command_started_event&&) = default;
    command_started_event& operator=(command_started_event&&) = default;

    command_started_event(command_started_event const&) = default;
    command_started_event& operator=(command_started_event const&) = default;

    ///
    /// Returns the command that has been started.
    ///
    /// @return The command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) command() const;

    ///
    /// Returns the name of the database.
    ///
    /// @return The database name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) database_name() const;

    ///
    /// Returns the name of the command.
    ///
    /// @return The command name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) command_name() const;

    ///
    /// Returns the request id.
    ///
    /// @return The request id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) request_id() const;

    ///
    /// Returns the operation id.
    ///
    /// @return The operation id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) operation_id() const;

    ///
    /// Optionally returns the service id.
    ///
    /// @return No contained value, or contains the service id if load balancing is enabled.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::oid>)
    service_id() const;

    ///
    /// Returns the host name.
    ///
    /// @return The host name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) host() const;

    ///
    /// Returns the port.
    ///
    /// @return The port.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint16_t) port() const;

   private:
    void const* _started_event;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::command_started_event.
///
