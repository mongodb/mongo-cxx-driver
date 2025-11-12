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

namespace mongocxx {
namespace v1 {

///
/// The log level for an unstructured log message.
///
/// @attention This feature is experimental! It is not ready for use!
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

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4251));
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4275));

///
/// The interface for an unstructured log message handler.
///
/// @important This interface does NOT fully conform to the CMAP specification!
///
/// @attention This feature is experimental! It is not ready for use!
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

BSONCXX_PRIVATE_WARNINGS_POP();

///
/// A tag type representing mongoc's default unstructured log handler.
///
/// @attention This feature is experimental! It is not ready for use!
///
class default_logger {};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::logger.
///
