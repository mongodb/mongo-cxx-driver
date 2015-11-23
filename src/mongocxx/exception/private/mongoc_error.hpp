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

#include <bsoncxx/document/value.hpp>

#include <mongocxx/exception/error_category.hpp>
#include <mongocxx/private/libmongoc.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

mongoc_error convert_mongoc_error(int code, int domain);
std::error_code error_code_from_bson_error_t(::bson_error_t error);

template <typename exception_type>
void throw_exception(bsoncxx::document::value raw_server_error) {
    throw exception_type{make_error_code(mongoc_error::k_raw_bson), std::move(raw_server_error)};
}

template <typename exception_type>
void throw_exception(::bson_error_t error) {
    throw exception_type{error_code_from_bson_error_t(error), std::move(error.message)};
}

template <typename exception_type>
void throw_exception(bsoncxx::document::value raw_server_error, ::bson_error_t error) {
    throw exception_type{error_code_from_bson_error_t(error), std::move(raw_server_error),
                         std::move(error.message)};
}

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
