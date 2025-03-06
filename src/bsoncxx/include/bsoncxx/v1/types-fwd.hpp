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

#include <bsoncxx/v1/detail/prelude.hpp>

#include <cstdint>

///
/// X-macro over the name and value of BSON types.
///
/// @important The addition of new expansions to this X-macro ARE NOT considered an API breaking change.
/// The modification or removal of existing expansions to this X-macro ARE considered an API breaking change.
/// This X-macro MUST used in a manner that is compatible with these API compatibility guidelines!
///
/// @par "Example"
/// ```cpp
/// #define EXAMPLE(name, value) std::cout << (#name) << ": " << (value) << '\n';
///
/// void print_bson_types() {
///     // Expands to:
///     //     std::cout << ("double") << ": " << (0x01) << '\n';
///     //     std::cout << ("string") << ": " << (0x02) << '\n';
///     //     ...
///     BSONCXX_V1_TYPES_XMACRO(EXAMPLE)
/// }
/// ```
///
/// @param X the user-defined macro to be expanded.
///
// clang-format off
#define BSONCXX_V1_TYPES_XMACRO(X) \
    X(double,     0x01) \
    X(string,     0x02) \
    X(document,   0x03) \
    X(array,      0x04) \
    X(binary,     0x05) \
    X(undefined,  0x06) \
    X(oid,        0x07) \
    X(bool,       0x08) \
    X(date,       0x09) \
    X(null,       0x0A) \
    X(regex,      0x0B) \
    X(dbpointer,  0x0C) \
    X(code,       0x0D) \
    X(symbol,     0x0E) \
    X(codewscope, 0x0F) \
    X(int32,      0x10) \
    X(timestamp,  0x11) \
    X(int64,      0x12) \
    X(decimal128, 0x13) \
    X(maxkey,     0x7F) \
    X(minkey,     0xFF)
// clang-format on

///
/// X-macro over the name and value of BSON binary subtypes.
///
/// @important The addition of new expansions to this X-macro ARE NOT considered an API breaking change.
/// The modification or removal of existing expansions to this X-macro ARE considered an API breaking change.
/// This X-macro MUST used in a manner that is compatible with these API compatibility guidelines!
///
/// @par "Example"
/// ```cpp
/// #define EXAMPLE(name, value) std::cout << #name << ": " << value << '\n';
///
/// void print_bson_binary_subtypes() {
///     // Expands to:
///     //     std::cout << ("binary") << ": " << (0x00) << '\n';
///     //     std::cout << ("function") << ": " << (0x01) << '\n';
///     //     ...
///     BSONCXX_V1_BINARY_SUBTYPES_XMACRO(EXAMPLE)
/// }
/// ```
///
/// @param X the user-defined macro to be expanded.
///
// clang-format off
#define BSONCXX_V1_BINARY_SUBTYPES_XMACRO(X) \
    X(binary,            0x00) \
    X(function,          0x01) \
    X(binary_deprecated, 0x02) \
    X(uuid_deprecated,   0x03) \
    X(uuid,              0x04) \
    X(md5,               0x05) \
    X(encrypted,         0x06) \
    X(column,            0x07) \
    X(sensitive,         0x08) \
    X(user,              0x80)
// clang-format on

namespace bsoncxx {
namespace v1 {

enum class type : std::uint8_t;
enum class binary_subtype : std::uint8_t;

} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Declares entities describing BSON types.
///
