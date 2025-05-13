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

//

#include <bsoncxx/v1/detail/prelude.hpp>

namespace bsoncxx {
namespace v1 {
namespace types {

///
/// BSON type value "64-bit Binary Floating Point".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_double {};

///
/// BSON type value "UTF-8 String".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_string {};

///
/// BSON type value "Embedded Document".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_document {};

///
/// BSON type value "Array".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_array {};

///
/// BSON type value "Binary Data".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_binary {};

///
/// BSON type value "Undefined (Value)".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_undefined {};

///
/// BSON type value "ObjectID".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_oid {};

///
/// BSON type value "Boolean".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_bool {};

///
/// BSON type value "UTC Datetime".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_date {};

///
/// BSON type value "Null Value".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_null {};

///
/// BSON type value "Regular Expression".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_regex {};

///
/// BSON type value "DBPointer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_dbpointer {};

///
/// BSON type value "JavaScript Code".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_code {};

///
/// BSON type value "Symbol".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_symbol {};

///
/// BSON type value "JavaScript Code With Scope".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_codewscope {};

///
/// BSON type value "32-bit Integer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_int32 {};

///
/// BSON type value "Timestamp".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_timestamp {};

///
/// BSON type value "64-bit Integer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_int64 {};

///
/// BSON type value "Decimal128".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_decimal128 {};

///
/// BSON type value "Max Key".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_maxkey {};

///
/// BSON type value "Min Key".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_minkey {};

///
/// A non-owning, read-only union of BSON type values.
///
/// @note This class only represents the **value** of a BSON element without its key.
/// @ref bsoncxx::v1::element::view represents a BSON element including its key.
///
/// @attention This feature is experimental! It is not ready for use!
///
class view {};

} // namespace types
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides non-owning, read-only entities representing a BSON type value.
///
/// @note A "BSON type value" refers to the value of a BSON element without its key.
///
