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

#include <chrono>
#include <cstring>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/string_view.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// An enumeration of each BSON type.
/// These x-macros will expand to be of the form:
///    k_double = 0x01,
///    k_utf8 = 0x02,
///    k_document = 0x03,
///    k_array = 0x04 ...
///
enum class type : std::uint8_t {
#define BSONCXX_ENUM(name, val) k_##name = val,
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
};

///
/// An enumeration of each BSON binary sub type.
/// These x-macros will expand to be of the form:
///   k_binary = 0x00,
///   k_function = 0x01,
///   k_binary_deprecated = 0x02,
///   k_uuid_deprecated = 0x03,
///   k_uuid = 0x04 ...
///
enum class binary_sub_type : std::uint8_t {
#define BSONCXX_ENUM(name, val) k_##name = val,
#include <bsoncxx/enums/binary_sub_type.hpp>
#undef BSONCXX_ENUM
};

BSONCXX_API std::string to_string(type rhs);
BSONCXX_API std::string to_string(binary_sub_type rhs);

namespace types {

///
/// A BSON double value.
///
struct BSONCXX_API b_double {
    static constexpr auto type_id = type::k_double;

    double value;

    BSONCXX_INLINE operator double() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_double& lhs, const b_double& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON UTF-8 encoded string value.
///
struct BSONCXX_API b_utf8 {
    static constexpr auto type_id = type::k_utf8;

    template <typename T>
    BSONCXX_INLINE explicit b_utf8(T&& value)
        : value(std::forward<T>(value)) {
    }

    stdx::string_view value;

    BSONCXX_INLINE operator stdx::string_view() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_utf8& lhs, const b_utf8& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON document value.
///
struct BSONCXX_API b_document {
    static constexpr auto type_id = type::k_document;

    document::view value;

    BSONCXX_INLINE operator document::view() {
        return value;
    }

    BSONCXX_INLINE document::view view() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_document& lhs, const b_document& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON array value.
///
struct BSONCXX_API b_array {
    static constexpr auto type_id = type::k_array;

    array::view value;

    BSONCXX_INLINE operator array::view() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_array& lhs, const b_array& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON binary data value.
///
struct BSONCXX_API b_binary {
    static constexpr auto type_id = type::k_binary;

    binary_sub_type sub_type;
    uint32_t size;
    const uint8_t* bytes;
};

BSONCXX_INLINE bool operator==(const b_binary& lhs, const b_binary& rhs) {
    return lhs.sub_type == rhs.sub_type && lhs.size == rhs.size &&
           (std::memcmp(lhs.bytes, rhs.bytes, lhs.size) == 0);
}

///
/// A BSON undefined value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_undefined {
    static constexpr auto type_id = type::k_undefined;
};

BSONCXX_INLINE bool operator==(const b_undefined&, const b_undefined&) {
    return true;
}

///
/// A BSON ObjectId value.
///
struct BSONCXX_API b_oid {
    static constexpr auto type_id = type::k_oid;

    oid value;
};

BSONCXX_INLINE bool operator==(const b_oid& lhs, const b_oid& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON boolean value.
///
struct BSONCXX_API b_bool {
    static constexpr auto type_id = type::k_bool;

    bool value;

    BSONCXX_INLINE operator bool() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_bool& lhs, const b_bool& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON date value.
///
struct BSONCXX_API b_date {
    static constexpr auto type_id = type::k_date;

    BSONCXX_INLINE
    explicit b_date(int64_t value) : value(value) {
    }

    BSONCXX_INLINE
    explicit b_date(const std::chrono::system_clock::time_point& tp)
        : value(std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch())
                    .count()) {
    }

    int64_t value;

    BSONCXX_INLINE operator int64_t() {
        return value;
    }

    BSONCXX_INLINE operator std::chrono::system_clock::time_point() {
        return std::chrono::system_clock::time_point(
            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                std::chrono::milliseconds{value}));
    }
};

BSONCXX_INLINE bool operator==(const b_date& lhs, const b_date& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON null value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_null {
    static constexpr auto type_id = type::k_null;
};

BSONCXX_INLINE bool operator==(const b_null&, const b_null&) {
    return true;
}

///
/// A BSON regex value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_regex {
    static constexpr auto type_id = type::k_regex;

    template <typename T, typename U>
    BSONCXX_INLINE explicit b_regex(T&& regex, U&& options)
        : regex(std::forward<T>(regex)), options(std::forward<U>(options)) {
    }

    stdx::string_view regex;
    stdx::string_view options;
};

BSONCXX_INLINE bool operator==(const b_regex& lhs, const b_regex& rhs) {
    return lhs.regex == rhs.regex && lhs.options == rhs.options;
}

///
/// A BSON DBPointer value.
///
/// @deprecated
///   A BSON DBPointer (aka DBRef) is still supported but deprecated.
///
struct BSONCXX_API b_dbpointer {
    static constexpr auto type_id = type::k_dbpointer;

    stdx::string_view collection;
    oid value;
};

BSONCXX_INLINE bool operator==(const b_dbpointer& lhs, const b_dbpointer& rhs) {
    return lhs.collection == rhs.collection && lhs.value == rhs.value;
}

///
/// A BSON JavaScript code value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_code {
    static constexpr auto type_id = type::k_code;

    template <typename T>
    BSONCXX_INLINE explicit b_code(T&& code)
        : code(std::forward<T>(code)) {
    }

    stdx::string_view code;

    BSONCXX_INLINE operator stdx::string_view() {
        return code;
    }
};

BSONCXX_INLINE bool operator==(const b_code& lhs, const b_code& rhs) {
    return lhs.code == rhs.code;
}

///
/// A BSON JavaScript code value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_symbol {
    static constexpr auto type_id = type::k_symbol;

    template <typename T>
    BSONCXX_INLINE explicit b_symbol(T&& symbol)
        : symbol(std::forward<T>(symbol)) {
    }

    stdx::string_view symbol;

    BSONCXX_INLINE operator stdx::string_view() {
        return symbol;
    }
};

BSONCXX_INLINE bool operator==(const b_symbol& lhs, const b_symbol& rhs) {
    return lhs.symbol == rhs.symbol;
}

///
/// A BSON JavaScript code with scope value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_codewscope {
    static constexpr auto type_id = type::k_codewscope;

    template <typename T, typename U>
    BSONCXX_INLINE explicit b_codewscope(T&& code, U&& scope)
        : code(std::forward<T>(code)), scope(std::forward<U>(scope)) {
    }

    stdx::string_view code;
    document::view scope;
};

BSONCXX_INLINE bool operator==(const b_codewscope& lhs, const b_codewscope& rhs) {
    return lhs.code == rhs.code && lhs.scope == rhs.scope;
}

///
/// A BSON signed 32-bit integer value.
///
struct BSONCXX_API b_int32 {
    static constexpr auto type_id = type::k_int32;

    int32_t value;

    BSONCXX_INLINE operator int32_t() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_int32& lhs, const b_int32& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON replication timestamp value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_timestamp {
    static constexpr auto type_id = type::k_timestamp;

    uint32_t increment;
    uint32_t timestamp;
};

BSONCXX_INLINE bool operator==(const b_timestamp& lhs, const b_timestamp& rhs) {
    return lhs.increment == rhs.increment && lhs.timestamp == rhs.timestamp;
}

///
/// A BSON 64-bit signed integer value.
///
struct BSONCXX_API b_int64 {
    static constexpr auto type_id = type::k_int64;

    int64_t value;

    BSONCXX_INLINE operator int64_t() {
        return value;
    }
};

BSONCXX_INLINE bool operator==(const b_int64& lhs, const b_int64& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON min-key value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_minkey {
    static constexpr auto type_id = type::k_minkey;
};

BSONCXX_INLINE bool operator==(const b_minkey&, const b_minkey&) {
    return true;
}

///
/// A BSON max-key value.
///
/// @warning
///   This BSON type is used internally by the MongoDB server - use by clients
///   is discouraged.
///
struct BSONCXX_API b_maxkey {
    static constexpr auto type_id = type::k_maxkey;
};

BSONCXX_INLINE bool operator==(const b_maxkey&, const b_maxkey&) {
    return true;
}

#define BSONCXX_ENUM(name, val)                                                \
    BSONCXX_INLINE bool operator!=(const b_##name& lhs, const b_##name& rhs) { \
        return !(lhs == rhs);                                                  \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
