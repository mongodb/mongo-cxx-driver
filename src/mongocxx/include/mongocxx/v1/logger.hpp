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

#include <mongocxx/v1/logger-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <functional>
#include <memory>

namespace mongocxx {
namespace v1 {

///
/// The log level for an unstructured log message.
///
enum class log_level {
    k_error,    ///< MONGOC_LOG_LEVEL_ERROR
    k_critical, ///< MONGOC_LOG_LEVEL_CRITICAL
    k_warning,  ///< MONGOC_LOG_LEVEL_WARNING
    k_message,  ///< MONGOC_LOG_LEVEL_MESSAGE
    k_info,     ///< MONGOC_LOG_LEVEL_INFO
    k_debug,    ///< MONGOC_LOG_LEVEL_DEBUG
    k_trace,    ///< MONGOC_LOG_LEVEL_TRACE
};

MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) to_string(log_level level);

///
/// The type of an unstructured log message handler.
///
/// The invocable is passed the log level, the message domain, and the message contents (in that
/// order) for each unstructured log message emitted by mongoc.
///
/// @important The invocable MUST be copyable (a requirement of `std::function`), MUST NOT throw an
/// exception, and MUST NOT itself emit an unstructured log message.
///
/// @see
/// - @ref mongocxx::v1::set_global_logger
/// - @ref mongocxx::v1::logger_guard
///
using logger_function =
    std::function<void MONGOCXX_ABI_CDECL(log_level, bsoncxx::v1::stdx::string_view, bsoncxx::v1::stdx::string_view)>;

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4251));
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4275));

///
/// The interface for an unstructured log message handler.
///
/// @important This interface does NOT fully conform to the CMAP specification!
///
class logger {
   public:
    ///
    /// Destructor.
    ///
    virtual ~logger();

    ///
    /// Move constructor.
    ///
    logger(logger&&) = default;

    ///
    /// Move assignment operator.
    ///
    logger& operator=(logger&&) = default;

    ///
    /// Copy constructor.
    ///
    logger(logger const&) = default;

    ///
    /// Copy assignment operator.
    ///
    logger& operator=(logger const&) = default;

    ///
    /// Default constructor.
    ///
    logger() = default;

    ///
    /// Handle an unstructured log message emitted by mongoc.
    ///
    /// Users may override this function to implement custom log message behavior such as outputting messages to a file
    /// or sending messages to a remote server.
    ///
    /// @param level The log level for the message being handled.
    /// @param domain The domain of the message.
    /// @param message The contents of the log message.
    ///
    /// @see
    /// - [Custom Log handlers (mongoc)](https://mongoc.org/libmongoc/current/unstructured_log.html#custom-log-handlers)
    ///
    virtual void operator()(
        log_level level,
        bsoncxx::v1::stdx::string_view domain,
        bsoncxx::v1::stdx::string_view message) noexcept = 0;
};

///
/// A tag type representing mongoc's default unstructured log handler.
///
class default_logger {};

///
/// Set the process-global unstructured log message handler to a custom handler.
///
/// Registers the custom unstructured log handler by calling
/// [`mongoc_log_set_handler()`](https://mongoc.org/libmongoc/current/logging.html). Unlike the
/// logging behavior configured via @ref mongocxx::v1::instance construction, the handler may be
/// changed any number of times over the lifetime of the instance object.
///
/// A copy of `handler` is stored via type erasure; any compatible invocable is accepted.
///
/// @param handler The handler to register. Disable unstructured logging when null (empty).
///
/// @warning This function is NOT thread-safe. The global handler is replaced without
/// synchronization, so the caller MUST ensure that no other thread concurrently configures the
/// global handler (via this API or @ref mongocxx::v1::logger_guard) or performs any operation which
/// may emit an unstructured log message for the duration of this call.
///
/// @important Must be called within the lifetime of a @ref mongocxx::v1::instance object.
///
/// @see
/// - [Custom Log Handlers (mongoc)](https://mongoc.org/libmongoc/current/unstructured_log.html#custom-log-handlers)
///
MONGOCXX_ABI_EXPORT_CDECL(void) set_global_logger(logger_function handler);

///
/// Set the process-global unstructured log message handler to mongoc's default handler.
///
/// Registers mongoc's default unstructured log handler by calling
/// [`mongoc_log_set_handler()`](https://mongoc.org/libmongoc/current/logging.html).
///
/// @param tag Unused: only for overload resolution.
///
/// @important Must be called within the lifetime of a @ref mongocxx::v1::instance object.
///
MONGOCXX_ABI_EXPORT_CDECL(void) set_global_logger(v1::default_logger tag);

///
/// A scope guard which temporarily replaces the process-global unstructured log message handler and
/// restores the prior handler on destruction.
///
/// On construction, captures the current global logging configuration and installs the requested
/// handler. On destruction, restores the captured configuration. Guards nest: destroying guards in
/// reverse order of construction restores each prior handler in turn.
///
/// ```cpp
/// mongocxx::v1::instance instance;
/// // ... mongoc's default handler is active ...
/// {
///     mongocxx::v1::logger_guard guard{[](auto level, auto domain, auto message) { ... }};
///     // ... the custom handler is active ...
/// }
/// // ... mongoc's default handler is active again ...
/// ```
///
/// @warning Construction and destruction are NOT thread-safe with respect to unstructured logging.
/// See @ref mongocxx::v1::set_global_logger(logger_function).
///
/// @important A guard's lifetime must be nested strictly within the lifetime of a
/// @ref mongocxx::v1::instance object.
///
class logger_guard {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Restore the unstructured log message handler that was active when this guard was
    /// constructed.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~logger_guard();

    ///
    /// Install a custom unstructured log message handler for the lifetime of this guard.
    ///
    /// A copy of `handler` is stored via type erasure; any compatible invocable is accepted.
    ///
    /// @param handler The handler to register. Disable unstructured logging when null (empty).
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() logger_guard(logger_function handler);

    ///
    /// Install mongoc's default unstructured log message handler for the lifetime of this guard.
    ///
    /// @param tag Unused: only for overload resolution.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() logger_guard(v1::default_logger tag);

    ///
    /// This class is not moveable.
    ///
    logger_guard(logger_guard&&) = delete;

    ///
    /// This class is not moveable.
    ///
    logger_guard& operator=(logger_guard&&) = delete;

    ///
    /// This class is not copyable.
    ///
    logger_guard(logger_guard const&) = delete;

    ///
    /// This class is not copyable.
    ///
    logger_guard& operator=(logger_guard const&) = delete;
};

BSONCXX_PRIVATE_WARNINGS_POP();

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::logger.
///
