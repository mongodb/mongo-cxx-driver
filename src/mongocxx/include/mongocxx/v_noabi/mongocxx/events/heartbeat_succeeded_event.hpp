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

#include <mongocxx/events/heartbeat_succeeded_event-fwd.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// The successful execution of a heartbeat ("hello") command.
///
/// @see
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
class heartbeat_succeeded_event {
   public:
    explicit heartbeat_succeeded_event(void const* event);

    ///
    /// Destroys a heartbeat_succeeded_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~heartbeat_succeeded_event();

    heartbeat_succeeded_event(heartbeat_succeeded_event&&) = default;
    heartbeat_succeeded_event& operator=(heartbeat_succeeded_event&&) = default;

    heartbeat_succeeded_event(heartbeat_succeeded_event const&) = default;
    heartbeat_succeeded_event& operator=(heartbeat_succeeded_event const&) = default;

    ///
    /// Returns the server reply for the succeeded operation.
    ///
    /// @return The reply.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) reply() const;

    ///
    /// Returns the duration of the successful operation.
    ///
    /// @return The duration in microseconds.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) duration() const;

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

    ///
    /// Returns a boolean indicating whether this heartbeat event is from an awaitable hello.
    ///
    /// @return A boolean.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) awaited() const;

   private:
    void const* _succeeded_event;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::heartbeat_succeeded_event.
///
