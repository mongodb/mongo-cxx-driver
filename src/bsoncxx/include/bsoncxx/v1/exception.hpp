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

#include <bsoncxx/v1/exception-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/detail/macros.hpp>

#include <system_error>
#include <type_traits>

namespace bsoncxx {
namespace v1 {

///
/// Enumeration identifying the source of a @ref bsoncxx::v1 error.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class source_errc {
    zero,    ///< Zero.
    bsoncxx, ///< From the bsoncxx library.
    bson,    ///< From the bson library.
};

///
/// The error category for @ref bsoncxx::v1::source_errc.
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) source_error_category();

///
/// Support implicit conversion to `std::error_condition`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_condition make_error_condition(source_errc code) {
    return {static_cast<int>(code), v1::source_error_category()};
}

///
/// Enumeration identifying the type (cause) of a @ref bsoncxx::v1 error.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class type_errc {
    zero,             ///< Zero.
    invalid_argument, ///< An invalid argument passed to the throwing function.
    runtime_error,    ///< An erroneous condition was detected at runtime.
};

///
/// The error category for @ref bsoncxx::v1::type_errc.
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) type_error_category();

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
/// Base class for all exceptions thrown by @ref bsoncxx::v1.
///
/// @par Inherits:
/// - `std::system_error`
///
/// @attention This feature is experimental! It is not ready for use!
///
class exception : public std::system_error {
   public:
    using std::system_error::system_error;

   private:
    BSONCXX_ABI_NO_EXPORT virtual void key_function() const;
};

BSONCXX_PRIVATE_WARNINGS_POP();

} // namespace v1
} // namespace bsoncxx

namespace std {

template <>
struct is_error_condition_enum<bsoncxx::v1::source_errc> : true_type {};

template <>
struct is_error_condition_enum<bsoncxx::v1::type_errc> : true_type {};

} // namespace std

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides bsoncxx error-handling utilities.
///
