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

#include <bsoncxx/v1/types/element-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

namespace bsoncxx {
namespace v1 {
namespace types {

///
/// A BSON type "64-bit Binary Floating Point".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_double {};

///
/// A BSON type "UTF-8 String".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_string {};

///
/// A BSON type "Embedded Document".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_document {};

///
/// A BSON type "Array".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_array {};

///
/// A BSON type "Binary Data".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_binary {};

///
/// A BSON type "Undefined (Value)".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_undefined {};

///
/// A BSON type "ObjectID".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_oid {};

///
/// A BSON type "Boolean".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_bool {};

///
/// A BSON type "UTC Datetime".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_date {};

///
/// A BSON type "Null Value".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_null {};

///
/// A BSON type "Regular Expression".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_regex {};

///
/// A BSON type "DBPointer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_dbpointer {};

///
/// A BSON type "JavaScript Code".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_code {};

///
/// A BSON type "Symbol".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_symbol {};

///
/// A BSON type "JavaScript Code With Scope".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_codewscope {};

///
/// A BSON type "32-bit Integer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_int32 {};

///
/// A BSON type "Timestamp".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_timestamp {};

///
/// A BSON type "64-bit Integer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_int64 {};

///
/// A BSON type "Decimal128".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_decimal128 {};

///
/// A BSON type "Max Key".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_maxkey {};

///
/// A BSON type "Min Key".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_minkey {};

} // namespace types
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides entities representing BSON types.
///
