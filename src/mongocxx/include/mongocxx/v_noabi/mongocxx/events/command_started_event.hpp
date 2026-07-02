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

#include <mongocxx/events/command_started_event-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/events/command_started.hpp> // IWYU pragma: export

#include <cstdint>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

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
   private:
    v1::events::command_started _event;

   public:
    ///
    /// @deprecated For internal use only.
    ///
    explicit MONGOCXX_ABI_NO_EXPORT command_started_event(void const* event);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ command_started_event(v1::events::command_started const& event) : _event{event} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::events::command_started() const {
        return _event;
    }

    ///
    /// Returns the command that has been started.
    ///
    /// @return The command.
    ///
    bsoncxx::v_noabi::document::view command() const {
        return _event.command();
    }

    ///
    /// Returns the name of the database.
    ///
    /// @return The database name.
    ///
    bsoncxx::v_noabi::stdx::string_view database_name() const {
        return _event.database_name();
    }

    ///
    /// Returns the name of the command.
    ///
    /// @return The command name.
    ///
    bsoncxx::v_noabi::stdx::string_view command_name() const {
        return _event.command_name();
    }

    ///
    /// Returns the request id.
    ///
    /// @return The request id.
    ///
    std::int64_t request_id() const {
        return _event.request_id();
    }

    ///
    /// Returns the operation id.
    ///
    /// @return The operation id.
    ///
    std::int64_t operation_id() const {
        return _event.operation_id();
    }

    ///
    /// Optionally returns the service id.
    ///
    /// @return No contained value, or contains the service id if load balancing is enabled.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::oid> service_id() const {
        return _event.service_id();
    }

    ///
    /// Returns the host name.
    ///
    /// @return The host name.
    ///
    bsoncxx::v_noabi::stdx::string_view host() const {
        return _event.host();
    }

    ///
    /// Returns the port.
    ///
    /// @return The port.
    ///
    std::uint16_t port() const {
        return _event.port();
    }
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::events::command_started_event from_v1(v1::events::command_started const& v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::events::command_started to_v1(v_noabi::events::command_started_event const& v) {
    return v1::events::command_started{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::command_started_event.
///
/// @par Includes
/// - @ref mongocxx/v1/events/command_started.hpp
///
