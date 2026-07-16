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

#include <mongocxx/v1/structured_log-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <functional>

namespace mongocxx {
namespace v1 {

///
/// The severity level of a structured log message.
///
/// Levels follow the syslog severities. A lower value is more severe. Filtering selects a
/// per-component "maximum level": a message is emitted when its level is at or below the
/// configured maximum for its component.
///
/// @see
/// - @ref mongocxx::v1::structured_log_entry::level
/// - @ref mongocxx::v1::options::structured_logging
///
enum class structured_log_level {
    k_emergency, ///< MONGOC_STRUCTURED_LOG_LEVEL_EMERGENCY
    k_alert,     ///< MONGOC_STRUCTURED_LOG_LEVEL_ALERT
    k_critical,  ///< MONGOC_STRUCTURED_LOG_LEVEL_CRITICAL
    k_error,     ///< MONGOC_STRUCTURED_LOG_LEVEL_ERROR
    k_warning,   ///< MONGOC_STRUCTURED_LOG_LEVEL_WARNING
    k_notice,    ///< MONGOC_STRUCTURED_LOG_LEVEL_NOTICE
    k_info,      ///< MONGOC_STRUCTURED_LOG_LEVEL_INFO
    k_debug,     ///< MONGOC_STRUCTURED_LOG_LEVEL_DEBUG
    k_trace,     ///< MONGOC_STRUCTURED_LOG_LEVEL_TRACE
};

///
/// The component (category) of a structured log message.
///
/// @important Applications should not assume this list is exhaustive: additional components may
/// be introduced by newer versions of the underlying C driver.
///
/// @see
/// - @ref mongocxx::v1::structured_log_entry::component
/// - @ref mongocxx::v1::options::structured_logging
///
enum class structured_log_component {
    k_command,          ///< MONGOC_STRUCTURED_LOG_COMPONENT_COMMAND
    k_topology,         ///< MONGOC_STRUCTURED_LOG_COMPONENT_TOPOLOGY
    k_server_selection, ///< MONGOC_STRUCTURED_LOG_COMPONENT_SERVER_SELECTION
    k_connection,       ///< MONGOC_STRUCTURED_LOG_COMPONENT_CONNECTION
};

///
/// Return the canonical name of a structured log level (e.g. `"debug"`).
///
/// The returned names are the lowercase severity names used by the standardized logging
/// specification. Returns `"unknown"` for an unrecognized value.
///
MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) to_string(structured_log_level level);

///
/// Return the canonical name of a structured log component (e.g. `"serverSelection"`).
///
/// Backed by `mongoc_structured_log_get_component_name`. The returned names match those used by
/// the standardized logging specification. Returns `"unknown"` for an unrecognized value.
///
MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) to_string(structured_log_component component);

///
/// Parse a structured log level from its canonical name (case-insensitive).
///
/// Backed by `mongoc_structured_log_get_named_level`. Returns an empty optional when `name` does
/// not name a level.
///
MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<structured_log_level>)
structured_log_level_from_string(bsoncxx::v1::stdx::string_view name);

///
/// Parse a structured log component from its canonical name (case-insensitive).
///
/// Backed by `mongoc_structured_log_get_named_component`. Returns an empty optional when `name`
/// does not name a component.
///
MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<structured_log_component>)
structured_log_component_from_string(bsoncxx::v1::stdx::string_view name);

///
/// A non-owning view of a single structured log message passed to a handler.
///
/// @warning A `structured_log_entry` (and any `string_view` it returns) is valid ONLY for the
/// duration of the handler invocation that received it. It MUST NOT be retained or accessed after
/// the handler returns. Use @ref message_as_bson to obtain an owned copy of the message document.
///
/// @see
/// - @ref mongocxx::v1::structured_log_handler
///
class structured_log_entry {
   private:
    void const* _impl; // mongoc_structured_log_entry_t const

   public:
    ///
    /// Return the severity level of this message.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::structured_log_level) level() const;

    ///
    /// Return the component (category) of this message.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::structured_log_component) component() const;

    ///
    /// Return the standardized message identifier (e.g. `"Command started"`).
    ///
    /// @warning The returned view is valid only for the duration of the handler invocation.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) message() const;

    ///
    /// Return an owned copy of the message as a BSON document.
    ///
    /// Backed by `mongoc_structured_log_entry_message_as_bson`. Unlike the entry itself, the
    /// returned value may outlive the handler invocation.
    ///
    /// @note Constructing the document has a cost; avoid this call for messages that will be
    /// discarded.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) message_as_bson() const;

    class internal;

   private:
    /* explicit(false) */ structured_log_entry(void const* impl);
};

///
/// The type of a structured log message handler.
///
/// The invocable is passed a @ref mongocxx::v1::structured_log_entry for each structured log
/// message emitted by a client or pool it is configured on.
///
/// @important The invocable MUST be copyable (a requirement of `std::function`), MUST NOT throw an
/// exception, and MUST NOT itself perform an operation on a client/pool that would emit a
/// structured log message routed back to it. When configured on a pool, it MUST be thread-safe:
/// it may be invoked concurrently by multiple pooled clients.
///
/// @see
/// - @ref mongocxx::v1::options::structured_logging
///
using structured_log_handler = std::function<void(structured_log_entry const&)>;

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides entities related to structured logging.
///
