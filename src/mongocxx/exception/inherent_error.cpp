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

#include <mongocxx/config/prelude.hpp>

#include <system_error>

#include <mongocxx/exception/inherent_error.hpp>

#include <mongocxx/exception/private/error_category.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Translate a mongocxx::inherent_error into a std::error_code.
///
/// @param error A mongocxx::inherent_error
///
/// @return A std::error_code
///
std::error_code make_error_code(mongocxx::inherent_error error) {
    return std::error_code(static_cast<int>(error), inherent_error_category());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
