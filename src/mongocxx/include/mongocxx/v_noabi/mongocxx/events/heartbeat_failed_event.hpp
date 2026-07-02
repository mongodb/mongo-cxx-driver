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

#include <mongocxx/events/heartbeat_failed_event-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/events/server_heartbeat_failed.hpp> // IWYU pragma: export

#include <cstdint>
#include <string>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// The failed execution of a heartbeat ("hello") command.
///
/// @see
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
class heartbeat_failed_event {
   private:
    v1::events::server_heartbeat_failed _event;

   public:
    ///
    /// @deprecated For internal use only.
    ///
    explicit MONGOCXX_ABI_NO_EXPORT heartbeat_failed_event(void const* event);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ heartbeat_failed_event(v1::events::server_heartbeat_failed const& event) : _event{event} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::events::server_heartbeat_failed() const {
        return _event;
    }

    ///
    /// Returns the failed operation's error message.
    ///
    /// @return The message.
    ///
    std::string message() const {
        return _event.message();
    }

    ///
    /// Returns the duration of the failed operation.
    ///
    /// @return The duration in microseconds.
    ///
    std::int64_t duration() const {
        return _event.duration();
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

    ///
    /// Returns a boolean indicating whether this heartbeat event is from an awaitable hello.
    ///
    /// @return A boolean.
    ///
    bool awaited() const {
        return _event.awaited();
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
inline v_noabi::events::heartbeat_failed_event from_v1(v1::events::server_heartbeat_failed const& v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::events::server_heartbeat_failed to_v1(v_noabi::events::heartbeat_failed_event const& v) {
    return v1::events::server_heartbeat_failed{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::heartbeat_failed_event.
///
/// @par Includes
/// - @ref mongocxx/v1/events/server_heartbeat_failed.hpp
///
