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

#include <cstdint>
#include <system_error>

#include <mongocxx/exception/server_error_code-fwd.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Errors which may be returned by the server.
///
/// This type is used to represent Int32 server error codeswithout defining the error codes
/// themselves.
///
/// @note `std::is_error_code_enum` is specialized for this type.
///
enum class server_error_code : std::int32_t {
    // Intentionally empty at this time!
};

///
/// Get the error_category for mongocxx library exceptions.
///
/// @return The mongocxx error_category
///
MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) server_error_category();

///
/// Translate a mongocxx::v_noabi::server_error_code into a std::error_code.
///
/// @param error A mongocxx::v_noabi::error_code
///
/// @return A std::error_code
///
inline std::error_code make_error_code(server_error_code error) {
    return {static_cast<int>(error), server_error_category()};
}

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {

using ::mongocxx::v_noabi::make_error_code;
using ::mongocxx::v_noabi::server_error_category;

} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

namespace std {

// @cond DOXYGEN_DISABLE
template <>
struct is_error_code_enum<::mongocxx::v_noabi::server_error_code> : std::true_type {};
// @endcond

} // namespace std

///
/// @file
/// Provides @ref mongocxx::v_noabi::server_error_code.
///
