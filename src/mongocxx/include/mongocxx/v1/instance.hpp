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

#include <mongocxx/v1/instance-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/logger-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>
#include <system_error>
#include <type_traits>

namespace mongocxx {
namespace v1 {

///
/// An instance of the MongoDB C++ Driver.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// All mongocxx API **MUST** be used within the lifetime of the instance object, with special exemptions listed below.
/// Only **ONE** instance object may exist for the lifetime of a given process. It is undefined behavior to use the
/// mongocxx API _before_ the instance object is initialized or _after_ the instance object is destroyed. It is
/// undefined behavior to create more than one instance object.
///
/// ```cpp
/// int main() {
///     // ...
///     // MUST NOT call mongocxx API before this point.
///     {
///         mongocxx::v1::instance instance;
///         // MAY call mongocxx API at this point.
///         // ...
///     }
///     // MUST NOT call mongocxx API after this point.
///     // ...
/// }
/// ```
///
/// @par Special exemptions
/// Only the following API are permitted to be used outside the lifetime of an instance object:
/// - @ref mongocxx::v1::logger
/// - @ref mongocxx::v1::default_logger
///
/// @see
/// - [Initialization and Cleanup (mongoc)](https://mongoc.org/libmongoc/current/init-cleanup.html)
///
class instance {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Cleanup the mongocxx (and mongoc) library.
    ///
    /// Calls [`mongoc_init()`](https://mongoc.org/libmongoc/current/mongoc_cleanup.html).
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~instance();

    ///
    /// This class is not moveable.
    ///
    instance(instance&&) = delete;

    ///
    /// This class is not moveable.
    ///
    instance& operator=(instance&&) = delete;

    ///
    /// This class is not copyable.
    ///
    instance(instance const&) = delete;

    ///
    /// This class is not copyable.
    ///
    instance& operator=(instance const&) = delete;

    ///
    /// Initialize the mongoc library with unstructured log messages disabled.
    ///
    /// Calls [`mongoc_init()`](https://mongoc.org/libmongoc/current/mongoc_init.html) after disabling unstructured
    /// log messages by calling `mongoc_log_set_handler(nullptr, nullptr)`.
    ///
    /// @important To use mongoc's default log message handler, construct this object with
    /// @ref instance(v1::default_logger tag) instead.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::instance::errc::multiple_instances if an `instance`
    /// object has already been created.
    ///
    /// @see
    /// - [Custom Log Handlers (mongoc)](https://mongoc.org/libmongoc/current/unstructured_log.html#custom-log-handlers)
    ///
    MONGOCXX_ABI_EXPORT_CDECL() instance();

    ///
    /// Initialize the mongoc library with the custom unstructured log message handler.
    ///
    /// Calls [`mongoc_init`](https://mongoc.org/libmongoc/current/mongoc_init.html) after registering the custom
    /// unstructured log handler by calling `mongoc_log_set_handler()`.
    ///
    /// @param handler Disable unstructured logging when null.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::instance::errc::multiple_instances if an `instance`
    /// object has already been created.
    ///
    /// @see
    /// - [Custom Log Handlers (mongoc)](https://mongoc.org/libmongoc/current/unstructured_log.html#custom-log-handlers)
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() instance(std::unique_ptr<v1::logger> handler);

    ///
    /// Initialize the mongoc library with its default unstructured log handler.
    ///
    /// Calls [`mongoc_init`](https://mongoc.org/libmongoc/current/mongoc_init.html) without registering any custom
    /// unstructured log handler.
    ///
    /// @param tag Unused: only for overload resolution.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::instance::errc::multiple_instances if an `instance`
    /// object has already been created.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() instance(v1::default_logger tag);

    ///
    /// Equivalent to @ref instance(std::unique_ptr<v1::logger> handler) when `handler == nullptr`.
    ///
    /// @see
    /// - [Custom Log Handlers (mongoc)](https://mongoc.org/libmongoc/current/unstructured_log.html#custom-log-handlers)
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() instance(std::nullptr_t);

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::instance.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,               ///< Zero.
        multiple_instances, ///< Cannot construct multiple instance objects in a given process.
    };

    ///
    /// The error category for @ref mongocxx::v1::instance::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }
};

} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::instance::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::instance.
///
