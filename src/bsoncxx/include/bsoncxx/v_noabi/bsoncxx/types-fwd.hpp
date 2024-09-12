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

}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {
namespace types {

#pragma push_macro("BSONCXX_ENUM")
#undef BSONCXX_ENUM
#define BSONCXX_ENUM(name, val) struct b_##name;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
#pragma pop_macro("BSONCXX_ENUM")

}  // namespace types
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {

using ::bsoncxx::v_noabi::binary_sub_type;
using ::bsoncxx::v_noabi::type;

}  // namespace bsoncxx

namespace bsoncxx {
namespace types {

#pragma push_macro("BSONCXX_ENUM")
#undef BSONCXX_ENUM
#define BSONCXX_ENUM(name, val) using ::bsoncxx::v_noabi::types::b_##name;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
#pragma pop_macro("BSONCXX_ENUM")

}  // namespace types
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities used to represent BSON types.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace types {

/// @ref bsoncxx::v_noabi::types::b_double
struct b_double {};

/// @ref bsoncxx::v_noabi::types::b_string
struct b_string {};

/// @ref bsoncxx::v_noabi::types::b_document
struct b_document {};

/// @ref bsoncxx::v_noabi::types::b_array
struct b_array {};

/// @ref bsoncxx::v_noabi::types::b_binary
struct b_binary {};

/// @ref bsoncxx::v_noabi::types::b_undefined
struct b_undefined {};

/// @ref bsoncxx::v_noabi::types::b_oid
struct b_oid {};

/// @ref bsoncxx::v_noabi::types::b_bool
struct b_bool {};

/// @ref bsoncxx::v_noabi::types::b_date
struct b_date {};

/// @ref bsoncxx::v_noabi::types::b_null
struct b_null {};

/// @ref bsoncxx::v_noabi::types::b_regex
struct b_regex {};

/// @ref bsoncxx::v_noabi::types::b_dbpointer
struct b_dbpointer {};

/// @ref bsoncxx::v_noabi::types::b_code
struct b_code {};

/// @ref bsoncxx::v_noabi::types::b_symbol
struct b_symbol {};

/// @ref bsoncxx::v_noabi::types::b_codewscope
struct b_codewscope {};

/// @ref bsoncxx::v_noabi::types::b_int32
struct b_int32 {};

/// @ref bsoncxx::v_noabi::types::b_timestamp
struct b_timestamp {};

/// @ref bsoncxx::v_noabi::types::b_int64
struct b_int64 {};

/// @ref bsoncxx::v_noabi::types::b_decimal128
struct b_decimal128 {};

/// @ref bsoncxx::v_noabi::types::b_maxkey
struct b_maxkey {};

/// @ref bsoncxx::v_noabi::types::b_minkey
struct b_minkey {};

}  // namespace types
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
