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

#include <mongocxx/v1/events/server_description-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {
namespace events {

///
/// `ServerDescription` from the Server Discovery and Monitoring specification.
///
/// @see
/// - [Server Discovery and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class server_description {
   private:
    void* _impl; // mongoc_server_description_t

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~server_description();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() server_description(server_description&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_description&) operator=(server_description&& other) noexcept;

    ///
    /// Copy constructor.
    ///
    server_description(server_description const& other);

    ///
    /// Copy assignment.
    ///
    server_description& operator=(server_description const& other);

    ///
    /// Return the client-generated unique server ID.
    ///
    /// @note The server ID is unique only for the associated client or client pool.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint32_t) id() const;

    ///
    /// Return the client-measured execution time of the "hello" command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) round_trip_time() const;

    ///
    /// Return the topology type.
    ///
    /// @returns One of:
    /// - "LoadBalancer"
    /// - "Mongos"
    /// - "PossiblePrimary"
    /// - "RSArbiter"
    /// - "RSGhost"
    /// - "RSOther"
    /// - "RSPrimary"
    /// - "RSSecondary"
    /// - "Standalone"
    /// - "Unknown"
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) type() const;

    ///
    /// Return the raw server response to the "hello" command.
    ///
    /// @returns Empty when connection was unsuccessful or a client-side error was encountered.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) hello() const;

    ///
    /// Return the hostname for the connection used by the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) host() const;

    ///
    /// Return the port number for the connection used by the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint16_t) port() const;
};

} // namespace events
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::events::server_description.
///
