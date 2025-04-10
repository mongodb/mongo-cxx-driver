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

namespace mongocxx {
namespace v_noabi {

enum class server_error_code : std::int32_t;

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {

using ::mongocxx::v_noabi::server_error_code;

} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<::mongocxx::v_noabi::server_error_code>;

} // namespace std

///
/// @file
/// Declares @ref mongocxx::v_noabi::server_error_code.
///
