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

#include <mongocxx/v1/structured_logging-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/structured_log.hpp>

#include <cstddef>

namespace mongocxx {
namespace v1 {

///
/// Options which configure structured logging for a client or pool.
///
/// Structured logging is configured per-client and per-pool (it is NOT process-global). Attach a
/// configured object via @ref mongocxx::v1::client::options::structured_logging_opts (or the
/// corresponding pool option) before constructing the client/pool.
///
/// A configuration with no handler disables structured logging.
///
/// @see
/// - [Structured Logging (mongoc)](https://mongoc.org/libmongoc/current/structured_log.html)
/// - [Logging (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/logging/logging/)
///
class structured_logging {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~structured_logging();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() structured_logging(structured_logging&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) operator=(structured_logging&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() structured_logging(structured_logging const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) operator=(structured_logging const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - The handler is empty (structured logging disabled).
    /// - No level or document-length overrides are set.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() structured_logging();

    ///
    /// Set the handler invoked for each structured log message.
    ///
    /// A copy of `handler` is stored. When empty (null), structured logging is disabled.
    ///
    /// @important When configured on a pool, `handler` MUST be thread-safe.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) handler(v1::structured_log_handler handler);

    ///
    /// Return the current handler.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::structured_log_handler) handler() const;

    ///
    /// Set the maximum (least severe) level emitted for a single component.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&)
    max_level_for_component(v1::structured_log_component component, v1::structured_log_level level);

    ///
    /// Return the maximum level configured for a single component, if any was set on this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::structured_log_level>)
    max_level_for_component(v1::structured_log_component component) const;

    ///
    /// Set the maximum (least severe) level emitted for all components.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) max_level_for_all_components(v1::structured_log_level level);

    ///
    /// Return the "all components" maximum level, if one was set on this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::structured_log_level>)
    max_level_for_all_components() const;

    ///
    /// Read the per-component maximum levels from the environment (the `MONGODB_LOG_*` variables)
    /// when this configuration is applied.
    ///
    /// Applied after any programmatic level settings, so environment values take precedence.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) max_levels_from_env();

    ///
    /// Set the maximum length (in bytes) of an embedded document before truncation.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) max_document_length(std::size_t max_document_length);

    ///
    /// Return the maximum document length, if one was set on this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::size_t>) max_document_length() const;

    ///
    /// Read the maximum document length from the environment
    /// (`MONGODB_LOG_MAX_DOCUMENT_LENGTH`) when this configuration is applied.
    ///
    /// Applied after any programmatic document-length setting, so the environment value takes
    /// precedence.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(structured_logging&) max_document_length_from_env();

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::structured_logging.
///
