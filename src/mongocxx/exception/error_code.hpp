// Copyright 2015 MongoDB Inc.
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

#include <system_error>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// @todo document this enum and the values
///
enum class error_code : std::int32_t {
    k_instance_already_exists = 1,
    k_invalid_client_object,
    k_invalid_collection_object,
    k_invalid_database_object,
    k_invalid_parameter,
    k_ssl_not_supported,
    k_unknown_read_concern,
    k_unknown_write_concern,
};

///
/// Get the error_category for mongocxx library exceptions.
///
/// @return The mongocxx error_category
///
MONGOCXX_API const std::error_category& MONGOCXX_CALL error_category();

///
/// Translate a mongocxx::error_code into a std::error_code.
///
/// @param error A mongocxx::error_code
///
/// @return A std::error_code
///
MONGOCXX_INLINE std::error_code make_error_code(error_code error) {
    return {static_cast<int>(error), error_category()};
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

namespace std {
// Specialize is_error_code_enum so we get simpler std::error_code construction
template <>
struct is_error_code_enum<mongocxx::error_code> : public true_type {};
}  // namespace std
