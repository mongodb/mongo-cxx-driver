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

#include <cstdint>

#include <mongocxx/events/server_opening_event-fwd.hpp>

#include <bsoncxx/oid.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// An event notification sent when the driver adds a MongoDB server to the topology description
/// and begins monitoring it.
///
/// @see
/// - "ServerOpeningEvent" in https://github.com/mongodb/specifications/blob/master/source/server-discovery-and-monitoring/server-discovery-and-monitoring.md
///
class server_opening_event {
   public:
    explicit server_opening_event(const void* event);

    ///
    /// Destroys a server_opening_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~server_opening_event();

    server_opening_event(server_opening_event&&) = default;
    server_opening_event& operator=(server_opening_event&&) = default;

    server_opening_event(const server_opening_event&) = default;
    server_opening_event& operator=(const server_opening_event&) = default;

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
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::oid) topology_id() const;

   private:
    const void* _event;
};

}  // namespace events
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::server_opening_event.
///
