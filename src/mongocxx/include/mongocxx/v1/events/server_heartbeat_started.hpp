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

#include <mongocxx/v1/events/server_heartbeat_started-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {
namespace events {

///
/// `ServerHeartbeatStartedEvent` from the SDAM Logging and Monitoring specification.
///
/// @see
/// - [SDAM Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class server_heartbeat_started {
   private:
    void const* _impl; // mongoc_apm_server_heartbeat_started_t const

   public:
    ///
    /// Return the hostname for the connection used by the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) host() const;

    ///
    /// Return the port number for the connection used by the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint16_t) port() const;

    ///
    /// Return true when this heartbeat event used the "streaming" protocol (instead of the "polling" protocol).
    ///
    /// @see
    /// - [Server Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-monitoring/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) awaited() const;

   private:
    /* explicit(false) */ server_heartbeat_started(void const* impl);
};

} // namespace events
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::events::server_heartbeat_started.
///
