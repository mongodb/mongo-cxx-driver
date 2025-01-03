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

#include <mongocxx/events/server_description-fwd.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// The description of a connected MongoDB server.
///
/// @see
/// - @ref mongocxx::v_noabi::events::topology_description
///
class server_description {
   public:
    explicit server_description(void const* event);

    ///
    /// Destroys a server_description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~server_description();

    server_description(server_description&&) = default;
    server_description& operator=(server_description&&) = default;

    server_description(server_description const&) = default;
    server_description& operator=(server_description const&) = default;

    ///
    /// An opaque id, unique to this server for this mongocxx::v_noabi::client or
    /// mongocxx::v_noabi::pool.
    ///
    /// @return The id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint32_t) id() const;

    ///
    /// The duration of the last hello call, indicating network latency.
    ///
    /// @return The duration in microseconds.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) round_trip_time() const;

    ///
    /// The server type: "Unknown", "Standalone", "Mongos", "PossiblePrimary", "RSPrimary",
    /// "RSSecondary", "RSArbiter", "RSOther", or "RSGhost".
    ///
    /// @return The type as a short-lived string view.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) type() const;

    ///
    /// @return The response as a short-lived document view.
    ///
    /// @deprecated use hello instead.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) is_master() const;

    ///
    /// The server's last response to the "hello" command, or an empty document if the driver
    /// has not yet reached the server or there was an error.
    ///
    /// @return The response as a short-lived document view.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) hello() const;

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

   private:
    void const* _sd;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::server_description.
///
