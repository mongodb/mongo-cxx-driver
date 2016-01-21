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

#include <mongocxx/config/prelude.hpp>

#include <system_error>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

enum class inherent_error : std::int32_t {
    k_invalid_client_object = 1,
    k_invalid_collection_object,
    k_invalid_database_object,
    k_invalid_parameter,
    k_ssl_not_supported,
    k_unknown_read_concern,
    k_unknown_write_concern,
};

///
/// Translate a mongocxx::inherent_error into a std::error_code.
///
/// @param error A mongocxx::inherent_error
///
/// @return A std::error_code
///
MONGOCXX_API std::error_code make_error_code(inherent_error error);

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
