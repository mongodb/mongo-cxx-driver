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

#include <mongocxx/exception/private/mongoc_error.hpp>

#include <mongocxx/exception/private/error_category.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Translate an error code and domain from libmongoc into a std::error_code.
///
/// @param code A libmongoc error code
/// @param domain A libmongoc error domain
///
/// @return A std::error_code
///
std::error_code make_error_code(int code, int domain) {
    return {code, mongoc_error_category(domain)};
}

///
/// Translate a bson_error_t from libmongoc into a std::error_code.
///
/// @param error A libmongoc bson_error_t
///
/// @return A std::error_code
///
std::error_code make_error_code(::bson_error_t error) {
    return make_error_code(error.code, error.domain);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
