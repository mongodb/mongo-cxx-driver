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

#include <bsoncxx/v1/types/view-fwd.hpp>

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

using v_noabi::binary_sub_type;
using v_noabi::type;

} // namespace bsoncxx

namespace bsoncxx {
namespace types {

using v_noabi::types::b_array;
using v_noabi::types::b_binary;
using v_noabi::types::b_bool;
using v_noabi::types::b_code;
using v_noabi::types::b_codewscope;
using v_noabi::types::b_date;
using v_noabi::types::b_dbpointer;
using v_noabi::types::b_decimal128;
using v_noabi::types::b_document;
using v_noabi::types::b_double;
using v_noabi::types::b_int32;
using v_noabi::types::b_int64;
using v_noabi::types::b_maxkey;
using v_noabi::types::b_minkey;
using v_noabi::types::b_null;
using v_noabi::types::b_oid;
using v_noabi::types::b_regex;
using v_noabi::types::b_string;
using v_noabi::types::b_symbol;
using v_noabi::types::b_timestamp;
using v_noabi::types::b_undefined;

} // namespace types
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities used to represent BSON types.
///
/// @par Includes
/// - @ref bsoncxx/v1/types/view-fwd.hpp
///
