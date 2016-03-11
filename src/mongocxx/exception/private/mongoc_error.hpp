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

#include <bsoncxx/document/value.hpp>
#include <mongocxx/exception/server_error_code.hpp>
#include <mongocxx/private/libmongoc.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

inline std::error_code make_error_code(int code, int) {
    // Domain is ignored. We simply issue the code.
    return {code, server_error_category()};
}

inline std::error_code make_error_code(const ::bson_error_t &error) {
    return make_error_code(error.code, error.domain);
}

template <typename exception_type>
void throw_exception(const ::bson_error_t &error) {
    throw exception_type{make_error_code(error), error.message};
}

template <typename exception_type>
void throw_exception(bsoncxx::document::value raw_server_error, const ::bson_error_t &error) {
    throw exception_type{make_error_code(error), std::move(raw_server_error), error.message};
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hpp>
