// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <cstring>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/string_or_literal.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

// TODO(amidvidy): figure out if enum classes need visibility attributes.
// Note: GCC complains unless visibility attributes are in this position
// when declaring enum classes.
// See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=43407
enum class type : std::uint8_t {
#define BSONCXX_ENUM(name, val) k_##name = val,
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
};

enum class binary_sub_type : std::uint8_t {
#define BSONCXX_ENUM(name, val) k_##name = val,
#include <bsoncxx/enums/binary_sub_type.hpp>
#undef BSONCXX_ENUM
};

BSONCXX_API std::string to_string(type rhs);
BSONCXX_API std::string to_string(binary_sub_type rhs);

namespace types {

struct BSONCXX_API b_double {
    static constexpr auto type_id = type::k_double;

    double value;

    operator double() { return value; }
};

inline bool operator==(const b_double& lhs, const b_double& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_utf8 {
    static constexpr auto type_id = type::k_utf8;

    template <typename T>
    explicit b_utf8(T&& value)
        : value(std::forward<T>(value)) {}

    string_or_literal value;

    operator string_or_literal() { return value; }
};

inline bool operator==(const b_utf8& lhs, const b_utf8& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_document {
    static constexpr auto type_id = type::k_document;

    document::view value;

    operator document::view() { return value; }
};

inline bool operator==(const b_document& lhs, const b_document& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_array {
    static constexpr auto type_id = type::k_array;

    array::view value;

    operator array::view() { return value; }
};

inline bool operator==(const b_array& lhs, const b_array& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_binary {
    static constexpr auto type_id = type::k_binary;

    binary_sub_type sub_type;
    uint32_t size;
    const uint8_t* bytes;
};

inline bool operator==(const b_binary& lhs, const b_binary& rhs) {
    return lhs.sub_type == rhs.sub_type && lhs.size == rhs.size && (std::memcmp(lhs.bytes, rhs.bytes, lhs.size) == 0);
}

struct BSONCXX_API b_undefined {
    static constexpr auto type_id = type::k_undefined;
};

inline bool operator==(const b_undefined&, const b_undefined&) {
    return true;
}

struct BSONCXX_API b_oid {
    static constexpr auto type_id = type::k_oid;

    oid value;
};

inline bool operator==(const b_oid& lhs, const b_oid& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_bool {
    static constexpr auto type_id = type::k_bool;

    bool value;

    operator bool() { return value; }
};

inline bool operator==(const b_bool& lhs, const b_bool& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_date {
    static constexpr auto type_id = type::k_date;

    int64_t value;

    operator int64_t() { return value; }
};

inline bool operator==(const b_date& lhs, const b_date& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_null {
    static constexpr auto type_id = type::k_null;
};

inline bool operator==(const b_null&, const b_null&) {
    return true;
}

struct BSONCXX_API b_regex {
    static constexpr auto type_id = type::k_regex;

    template <typename T, typename U>
    explicit b_regex(T&& regex, U&& options)
        : regex(std::forward<T>(regex)), options(std::forward<U>(options)) {}

    string_or_literal regex;
    string_or_literal options;
};

inline bool operator==(const b_regex& lhs, const b_regex& rhs) {
    return lhs.regex == rhs.regex && lhs.options == rhs.options;
}

struct BSONCXX_API b_dbpointer {
    static constexpr auto type_id = type::k_dbpointer;

    string_or_literal collection;
    oid value;
};

inline bool operator==(const b_dbpointer& lhs, const b_dbpointer& rhs) {
    return lhs.collection == rhs.collection && lhs.value == rhs.value;
}

struct BSONCXX_API b_code {
    static constexpr auto type_id = type::k_code;

    template <typename T>
    explicit b_code(T&& code)
        : code(std::forward<T>(code)) {}

    string_or_literal code;

    operator string_or_literal() { return code; }
};

inline bool operator==(const b_code& lhs, const b_code& rhs) {
    return lhs.code == rhs.code;
}

struct BSONCXX_API b_symbol {
    static constexpr auto type_id = type::k_symbol;

    template <typename T>
    explicit b_symbol(T&& symbol)
        : symbol(std::forward<T>(symbol)) {}

    string_or_literal symbol;

    operator string_or_literal() { return symbol; }
};

inline bool operator==(const b_symbol& lhs, const b_symbol& rhs) {
    return lhs.symbol == rhs.symbol;
}

struct BSONCXX_API b_codewscope {
    static constexpr auto type_id = type::k_codewscope;

    template <typename T, typename U>
    explicit b_codewscope(T&& code, U&& scope)
        : code(std::forward<T>(code)), scope(std::forward<U>(scope)) {}

    string_or_literal code;
    document::view scope;
};

inline bool operator==(const b_codewscope& lhs, const b_codewscope& rhs) {
    return lhs.code == rhs.code && lhs.scope == rhs.scope;
}

struct BSONCXX_API b_int32 {
    static constexpr auto type_id = type::k_int32;

    int32_t value;

    operator int32_t() { return value; }
};

inline bool operator==(const b_int32& lhs, const b_int32& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_timestamp {
    static constexpr auto type_id = type::k_timestamp;

    uint32_t increment;
    uint32_t timestamp;
};

inline bool operator==(const b_timestamp& lhs, const b_timestamp& rhs) {
    return lhs.increment == rhs.increment && lhs.timestamp == rhs.timestamp;
}

struct BSONCXX_API b_int64 {
    static constexpr auto type_id = type::k_int64;

    int64_t value;

    operator int64_t() { return value; }
};

inline bool operator==(const b_int64& lhs, const b_int64& rhs) {
    return lhs.value == rhs.value;
}

struct BSONCXX_API b_minkey {
    static constexpr auto type_id = type::k_minkey;
};

inline bool operator==(const b_minkey&, const b_minkey&) {
    return true;
}

struct BSONCXX_API b_maxkey {
    static constexpr auto type_id = type::k_maxkey;
};

inline bool operator==(const b_maxkey&, const b_maxkey&) {
    return true;
}

#define BSONCXX_ENUM(name, val) \
inline bool operator!=(const b_##name& lhs, const b_##name& rhs) { \
    return !(lhs == rhs); \
}
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
