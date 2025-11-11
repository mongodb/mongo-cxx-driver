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

#include <mongocxx/v1/events/command_failed-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/oid-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {
namespace events {

///
/// `CommandFailedEvent` from the Command Logging and Monitoring specification.
///
/// @see
/// - [Command Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/command-logging-and-monitoring/command-logging-and-monitoring/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class command_failed {
   private:
    void const* _impl; // mongoc_apm_command_failed_t const

   public:
    ///
    /// Return the failure document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) failure() const;

    ///
    /// Return the command name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) command_name() const;

    ///
    /// Return the execution time of the event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) duration() const;

    ///
    /// Return the client-generated request ID.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) request_id() const;

    ///
    /// Return the client-generated operation ID.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) operation_id() const;

    ///
    /// Return the service ID for the command (when in load balancer mode).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::oid>) service_id() const;

    ///
    /// Return the hostname for the connection used by the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) host() const;

    ///
    /// Return the port number for the connection used by the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint16_t) port() const;

   private:
    /* explicit(false) */ command_failed(void const* impl);
};

} // namespace events
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::events::command_failed.
///
