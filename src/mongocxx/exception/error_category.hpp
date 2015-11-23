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

#include <mongocxx/exception/mongoc_error.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

const std::error_category& mongoc_error_category();

std::error_code make_error_code(mongoc_error error);

std::error_condition make_error_condition(mongoc_error error);

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

namespace std {

///
/// Allows a std::error_condition to be implicitly constructed from a mongoc_error.
///
template <>
struct is_error_condition_enum<mongocxx::exception::mongoc_error>
    : public true_type {};

}  // namespace std

#include <mongocxx/config/postlude.hpp>
