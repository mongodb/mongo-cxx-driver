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

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

enum class type : std::uint8_t;
enum class binary_sub_type : std::uint8_t;

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {
namespace types {

struct b_double;
struct b_string;
struct b_document;
struct b_array;
struct b_binary;
struct b_undefined;
struct b_oid;
struct b_bool;
struct b_date;
struct b_null;
struct b_regex;
struct b_dbpointer;
struct b_code;
struct b_symbol;
struct b_codewscope;
struct b_int32;
struct b_timestamp;
struct b_int64;
struct b_decimal128;
struct b_maxkey;
struct b_minkey;

} // namespace types
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {

using ::bsoncxx::v_noabi::binary_sub_type;
using ::bsoncxx::v_noabi::type;

} // namespace bsoncxx

namespace bsoncxx {
namespace types {

using ::bsoncxx::v_noabi::types::b_array;
using ::bsoncxx::v_noabi::types::b_binary;
using ::bsoncxx::v_noabi::types::b_bool;
using ::bsoncxx::v_noabi::types::b_code;
using ::bsoncxx::v_noabi::types::b_codewscope;
using ::bsoncxx::v_noabi::types::b_date;
using ::bsoncxx::v_noabi::types::b_dbpointer;
using ::bsoncxx::v_noabi::types::b_decimal128;
using ::bsoncxx::v_noabi::types::b_document;
using ::bsoncxx::v_noabi::types::b_double;
using ::bsoncxx::v_noabi::types::b_int32;
using ::bsoncxx::v_noabi::types::b_int64;
using ::bsoncxx::v_noabi::types::b_maxkey;
using ::bsoncxx::v_noabi::types::b_minkey;
using ::bsoncxx::v_noabi::types::b_null;
using ::bsoncxx::v_noabi::types::b_oid;
using ::bsoncxx::v_noabi::types::b_regex;
using ::bsoncxx::v_noabi::types::b_string;
using ::bsoncxx::v_noabi::types::b_symbol;
using ::bsoncxx::v_noabi::types::b_timestamp;
using ::bsoncxx::v_noabi::types::b_undefined;

} // namespace types
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities used to represent BSON types.
///
