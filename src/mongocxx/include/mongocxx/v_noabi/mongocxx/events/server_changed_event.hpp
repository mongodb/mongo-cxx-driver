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

#include <mongocxx/events/server_changed_event-fwd.hpp>

#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/events/server_description.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// A change in the description of a connected MongoDB server.
///
/// @see
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
class server_changed_event {
   public:
    explicit server_changed_event(void const* event);

    ///
    /// Destroys a server_changed_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~server_changed_event();

    server_changed_event(server_changed_event&&) = default;
    server_changed_event& operator=(server_changed_event&&) = default;

    server_changed_event(server_changed_event const&) = default;
    server_changed_event& operator=(server_changed_event const&) = default;

    ///
    /// Returns the server host name.
    ///
    /// @return The host name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) host() const;

    ///
    /// Returns the server port.
    ///
    /// @return The port.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint16_t) port() const;

    ///
    /// An opaque id, unique to this topology for this mongocxx::v_noabi::client or
    /// mongocxx::v_noabi::pool.
    ///
    /// @return The id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::oid const) topology_id() const;

    ///
    /// The server's description before it changed.
    ///
    /// @return The server_description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_description const) previous_description() const;

    ///
    /// The server's description after it changed.
    ///
    /// @return The server_description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_description const) new_description() const;

   private:
    void const* _event;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::server_changed_event.
///
