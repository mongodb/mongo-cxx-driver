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

#include <mongocxx/v1/exception-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>
#include <system_error>
#include <type_traits>

namespace mongocxx {
namespace v1 {

///
/// Enumeration identifying the source of a @ref mongocxx::v1 error.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class source_errc {
    zero,       ///< Zero.
    mongocxx,   ///< From the mongocxx library.
    mongoc,     ///< From the mongoc library.
    mongocrypt, ///< From the mongocrypt library.
    server,     ///< From the MongoDB server.
};

///
/// The error category for @ref mongocxx::v1::source_errc.
///
/// @attention This feature is experimental! It is not ready for use!
///
MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) source_error_category();

///
/// Support implicit conversion to `std::error_condition`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_condition make_error_condition(source_errc code) {
    return {static_cast<int>(code), v1::source_error_category()};
}

///
/// Enumeration identifying the type (cause) of a @ref mongocxx::v1 error.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class type_errc {
    zero,             ///< Zero.
    invalid_argument, ///< An invalid argument passed to the throwing function.
    runtime_error,    ///< An erroneous condition was detected at runtime.
};

///
/// The error category for @ref mongocxx::v1::type_errc.
///
/// @attention This feature is experimental! It is not ready for use!
///
MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) type_error_category();

///
/// Support implicit conversion to `std::error_condition`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_condition make_error_condition(type_errc code) {
    return {static_cast<int>(code), v1::type_error_category()};
}

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4251));
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4275));

///
/// Base class for all exceptions thrown by @ref mongocxx::v1.
///
/// @par Inherits:
/// - `std::system_error`
///
/// @attention This feature is experimental! It is not ready for use!
///
class exception : public std::system_error {
   private:
    class impl;
    std::shared_ptr<impl> _impl;

   public:
    ///
    /// Return true if this exception contains the specified error label.
    ///
    /// @important The set of error labels may vary depending on the operation and error.
    ///
    bool MONGOCXX_ABI_CDECL has_error_label(bsoncxx::v1::stdx::string_view label) const;

    class internal;

   private:
    MONGOCXX_ABI_NO_EXPORT /* explicit(false) */
    exception(std::error_code ec, char const* message, std::unique_ptr<impl> impl);

    MONGOCXX_ABI_NO_EXPORT /* explicit(false) */
    exception(std::error_code ec, std::unique_ptr<impl> impl);

    MONGOCXX_ABI_NO_EXPORT virtual void key_function() const;
};

BSONCXX_PRIVATE_WARNINGS_POP();

} // namespace v1
} // namespace mongocxx

template <>
struct std::is_error_condition_enum<mongocxx::v1::source_errc> : true_type {};

template <>
struct std::is_error_condition_enum<mongocxx::v1::type_errc> : true_type {};

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides mongocxx error-handling utilities.
///
