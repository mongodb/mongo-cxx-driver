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

#include <bsoncxx/v1/types/id-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>

#include <string>

namespace bsoncxx {
namespace v1 {
namespace types {

///
/// Enumeration identifying a BSON type.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @showenumvalues
///
enum class id : std::uint8_t {
    k_double = 0x01,     ///< 64-bit binary floating point.
    k_string = 0x02,     ///< UTF-8 string.
    k_document = 0x03,   ///< Embedded document.
    k_array = 0x04,      ///< Array.
    k_binary = 0x05,     ///< Binary data.
    k_undefined = 0x06,  ///< Undefined value. @deprecated
    k_oid = 0x07,        ///< ObjectId.
    k_bool = 0x08,       ///< Boolean.
    k_date = 0x09,       ///< UTC datetime.
    k_null = 0x0A,       ///< Null value.
    k_regex = 0x0B,      ///< Regular expression.
    k_dbpointer = 0x0C,  ///< DBPointer. @deprecated
    k_code = 0x0D,       ///< JavaScript code.
    k_symbol = 0x0E,     ///< Symbol. @deprecated
    k_codewscope = 0x0F, ///< JavaScript code with scope. @deprecated
    k_int32 = 0x10,      ///< 32-bit integer.
    k_timestamp = 0x11,  ///< Timestamp.
    k_int64 = 0x12,      ///< 64-bit integer.
    k_decimal128 = 0x13, ///< 128-bit decimal floating point.
    k_maxkey = 0x7F,     ///< Max key.
    k_minkey = 0xFF,     ///< Min key.
};

///
/// Return the name of the enumerator (e.g. `"double"` given `k_double`).
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::string) to_string(id rhs);

///
/// Enumeration identifying a BSON binary subtype.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @showenumvalues
///
enum class binary_subtype : std::uint8_t {
    k_binary = 0x00,            ///< Generic binary subtype.
    k_function = 0x01,          ///< Function.
    k_binary_deprecated = 0x02, ///< Binary (Old). @deprecated
    k_uuid_deprecated = 0x03,   ///< UUID (Old). @deprecated
    k_uuid = 0x04,              ///< UUID.
    k_md5 = 0x05,               ///< MD5.
    k_encrypted = 0x06,         ///< Encrypted BSON value.
    k_column = 0x07,            ///< Compressed BSON column.
    k_sensitive = 0x08,         ///< Sensitive.
    k_vector = 0x09,            ///< Vector.
    k_user = 0x80,              ///< User defined.
};

///
/// Return the name of the enumerator (e.g. `"binary"` given `k_binary`).
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::string) to_string(binary_subtype rhs);

} // namespace types
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides enumerations identifying the type of a BSON element.
///
