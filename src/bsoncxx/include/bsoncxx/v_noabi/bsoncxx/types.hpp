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

#include <chrono>
#include <cstring>

#include <bsoncxx/types-fwd.hpp>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/decimal128.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/config/prelude.hpp>

#pragma push_macro("BSONCXX_ENUM")
#undef BSONCXX_ENUM

BSONCXX_PUSH_WARNINGS();
BSONCXX_DISABLE_WARNING(GNU("-Wfloat-equal"));

namespace bsoncxx {
namespace v_noabi {

///
/// An enumeration of each BSON type.
/// These x-macros will expand to be of the form:
///    k_double = 0x01,
///    k_string = 0x02,
///    k_document = 0x03,
///    k_array = 0x04 ...
///
enum class type : std::uint8_t {
#define BSONCXX_ENUM(name, val) k_##name = val,
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    k_utf8 = 0x02,
};

///
/// An enumeration of each BSON binary sub type.
/// These x-macros will expand to be of the form:
///   k_binary = 0x00,
///   k_function = 0x01,
///   k_binary_deprecated = 0x02,
///   k_uuid_deprecated = 0x03,
///   k_uuid = 0x04,
///   k_md5 = 0x05,
///   k_encrypted = 0x06,
///   k_column = 0x07,
///   k_user = 0x80
///
enum class binary_sub_type : std::uint8_t {
#define BSONCXX_ENUM(name, val) k_##name = val,
#include <bsoncxx/enums/binary_sub_type.hpp>
#undef BSONCXX_ENUM
};

///
/// Returns a stringification of the given type.
///
/// @param rhs
///   The type to stringify.
///
/// @return a std::string representation of the type.
///
BSONCXX_API std::string BSONCXX_CALL to_string(type rhs);

///
/// Returns a stringification of the given binary sub type.
///
/// @param rhs
///   The type to stringify.
///
/// @return a std::string representation of the type.
///
BSONCXX_API std::string BSONCXX_CALL to_string(binary_sub_type rhs);

namespace types {

///
/// A BSON double value.
///
struct b_double {
    static constexpr auto type_id = type::k_double;

    double value;

    ///
    /// Conversion operator unwrapping a double
    ///
    BSONCXX_INLINE operator double() const {
        return value;
    }
};

///
/// free function comparator for b_double
///
/// @relatesalso bsoncxx::v_noabi::types::b_double
///
BSONCXX_INLINE bool operator==(const b_double& lhs, const b_double& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON UTF-8 encoded string value.
///
struct b_string {
    static constexpr auto type_id = type::k_string;

    ///
    /// Constructor for b_string.
    ///
    /// @param t
    ///   The value to wrap.
    ///
    template <typename T, detail::requires_not_t<int, detail::is_alike<b_string, T>> = 0>
    BSONCXX_INLINE explicit b_string(T&& t) : value(std::forward<T>(t)) {}

    stdx::string_view value;

    ///
    /// Conversion operator unwrapping a string_view
    ///
    BSONCXX_INLINE operator stdx::string_view() const {
        return value;
    }
};

///
/// free function comparator for b_string
///
/// @relatesalso bsoncxx::v_noabi::types::b_string
///
BSONCXX_INLINE bool operator==(const b_string& lhs, const b_string& rhs) {
    return lhs.value == rhs.value;
}

///
/// Equivalent to @ref b_string.
///
/// @deprecated Use @ref b_string instead.
///
BSONCXX_DEPRECATED typedef b_string b_utf8;

///
/// A BSON document value.
///
struct b_document {
    static constexpr auto type_id = type::k_document;

    document::view value;

    ///
    /// Conversion operator unwrapping a document::view
    ///
    BSONCXX_INLINE operator document::view() const {
        return value;
    }

    ///
    /// Returns an unwrapped document::view
    ///
    BSONCXX_INLINE document::view view() {
        return value;
    }
};

///
/// free function comparator for b_document
///
/// @relatesalso bsoncxx::v_noabi::types::b_document
///
BSONCXX_INLINE bool operator==(const b_document& lhs, const b_document& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON array value.
///
struct b_array {
    static constexpr auto type_id = type::k_array;

    array::view value;

    ///
    /// Conversion operator unwrapping an array::view
    ///
    BSONCXX_INLINE operator array::view() const {
        return value;
    }
};

///
/// free function comparator for b_array
///
/// @relatesalso bsoncxx::v_noabi::types::b_array
///
BSONCXX_INLINE bool operator==(const b_array& lhs, const b_array& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON binary data value.
///
struct b_binary {
    static constexpr auto type_id = type::k_binary;

    binary_sub_type sub_type;
    uint32_t size;
    const uint8_t* bytes;
};

///
/// free function comparator for b_binary
///
/// @relatesalso bsoncxx::v_noabi::types::b_binary
///
BSONCXX_INLINE bool operator==(const b_binary& lhs, const b_binary& rhs) {
    return lhs.sub_type == rhs.sub_type && lhs.size == rhs.size &&
           (!lhs.size || (std::memcmp(lhs.bytes, rhs.bytes, lhs.size) == 0));
}

///
/// A BSON undefined value.
///
/// @deprecated This BSON type is deprecated. Usage is discouraged.
///
struct b_undefined {
    static constexpr auto type_id = type::k_undefined;
};

///
/// free function comparator for b_undefined
///
/// @relatesalso bsoncxx::v_noabi::types::b_undefined
///
BSONCXX_INLINE bool operator==(const b_undefined&, const b_undefined&) {
    return true;
}

///
/// A BSON ObjectId value.
///
struct b_oid {
    static constexpr auto type_id = type::k_oid;

    oid value;
};

///
/// free function comparator for b_oid
///
/// @relatesalso bsoncxx::v_noabi::types::b_oid
///
BSONCXX_INLINE bool operator==(const b_oid& lhs, const b_oid& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON boolean value.
///
struct b_bool {
    static constexpr auto type_id = type::k_bool;

    bool value;

    ///
    /// Conversion operator unwrapping a bool
    ///
    BSONCXX_INLINE operator bool() const {
        return value;
    }
};

///
/// free function comparator for b_bool
///
/// @relatesalso bsoncxx::v_noabi::types::b_bool
///
BSONCXX_INLINE bool operator==(const b_bool& lhs, const b_bool& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON date value.
///
struct b_date {
    static constexpr auto type_id = type::k_date;

    ///
    /// Constructor for b_date
    ///
    /// @param value
    ///   Milliseconds since the system_clock epoch.
    ///
    BSONCXX_INLINE
    explicit b_date(std::chrono::milliseconds value) : value(value) {}

    ///
    /// Constructor for b_date
    ///
    /// @param tp
    ///   A system_clock time_point.
    ///
    BSONCXX_INLINE
    explicit b_date(const std::chrono::system_clock::time_point& tp)
        : value(std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch())) {}

    std::chrono::milliseconds value;

    ///
    /// Conversion operator unwrapping a int64_t
    ///
    BSONCXX_INLINE operator int64_t() const {
        return value.count();
    }

    ///
    /// Manually convert this b_date to an int64_t
    ///
    BSONCXX_INLINE int64_t to_int64() const {
        return value.count();
    }

    ///
    /// Conversion operator unwrapping a time_point
    ///
    BSONCXX_INLINE operator std::chrono::system_clock::time_point() const {
        return std::chrono::system_clock::time_point(
            std::chrono::duration_cast<std::chrono::system_clock::duration>(value));
    }
};

///
/// free function comparator for b_date
///
/// @relatesalso bsoncxx::v_noabi::types::b_date
///
BSONCXX_INLINE bool operator==(const b_date& lhs, const b_date& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON null value.
///
struct b_null {
    static constexpr auto type_id = type::k_null;
};

///
/// free function comparator for b_null
///
/// @relatesalso bsoncxx::v_noabi::types::b_null
///
BSONCXX_INLINE bool operator==(const b_null&, const b_null&) {
    return true;
}

///
/// A BSON regex value.
///
struct b_regex {
    static constexpr auto type_id = type::k_regex;

    ///
    /// Constructor for b_regex
    ///
    /// @param regex
    ///   The regex pattern
    ///
    /// @param options
    ///   The regex options
    ///
    template <typename T,
              typename U = stdx::string_view,
              detail::requires_not_t<int, detail::is_alike<b_regex, T>> = 0>
    BSONCXX_INLINE explicit b_regex(T&& regex, U&& options = U{})
        : regex(std::forward<T>(regex)), options(std::forward<U>(options)) {}

    stdx::string_view regex;
    stdx::string_view options;
};

///
/// free function comparator for b_regex
///
/// @relatesalso bsoncxx::v_noabi::types::b_regex
///
BSONCXX_INLINE bool operator==(const b_regex& lhs, const b_regex& rhs) {
    return lhs.regex == rhs.regex && lhs.options == rhs.options;
}

///
/// A BSON DBPointer (aka DBRef) value.
///
/// @deprecated This BSON type is deprecated. Usage is discouraged.
///
struct b_dbpointer {
    static constexpr auto type_id = type::k_dbpointer;

    stdx::string_view collection;
    oid value;
};

///
/// free function comparator for b_dbpointer
///
/// @relatesalso bsoncxx::v_noabi::types::b_dbpointer
///
BSONCXX_INLINE bool operator==(const b_dbpointer& lhs, const b_dbpointer& rhs) {
    return lhs.collection == rhs.collection && lhs.value == rhs.value;
}

///
/// A BSON JavaScript code value.
///
struct b_code {
    static constexpr auto type_id = type::k_code;

    ///
    /// Constructor for b_code.
    ///
    /// @param t
    ///   The js code
    ///
    template <typename T, detail::requires_not_t<int, detail::is_alike<b_code, T>> = 0>
    BSONCXX_INLINE explicit b_code(T&& t) : code(std::forward<T>(t)) {}

    stdx::string_view code;

    ///
    /// Conversion operator unwrapping a string_view
    ///
    BSONCXX_INLINE operator stdx::string_view() const {
        return code;
    }
};

///
/// free function comparator for b_code
///
/// @relatesalso bsoncxx::v_noabi::types::b_code
///
BSONCXX_INLINE bool operator==(const b_code& lhs, const b_code& rhs) {
    return lhs.code == rhs.code;
}

///
/// A BSON Symbol value.
///
/// @deprecated This BSON type is deprecated. Usage is discouraged.
///
struct b_symbol {
    static constexpr auto type_id = type::k_symbol;

    ///
    /// Constructor for b_symbol.
    ///
    /// @param t
    ///   The symbol.
    ///
    template <typename T, detail::requires_not_t<int, detail::is_alike<b_symbol, T>> = 0>
    BSONCXX_INLINE explicit b_symbol(T&& t) : symbol(std::forward<T>(t)) {}

    stdx::string_view symbol;

    ///
    /// Conversion operator unwrapping a string_view
    ///
    BSONCXX_INLINE operator stdx::string_view() const {
        return symbol;
    }
};

///
/// free function comparator for b_symbol
///
/// @relatesalso bsoncxx::v_noabi::types::b_symbol
///
BSONCXX_INLINE bool operator==(const b_symbol& lhs, const b_symbol& rhs) {
    return lhs.symbol == rhs.symbol;
}

///
/// A BSON JavaScript code with scope value.
///
struct b_codewscope {
    static constexpr auto type_id = type::k_codewscope;

    ///
    /// Constructor for b_codewscope.
    ///
    /// @param code
    ///   The js code
    ///
    /// @param scope
    ///   A bson document view holding the scope environment.
    ///
    template <typename T,
              typename U,
              detail::requires_not_t<int, detail::is_alike<b_codewscope, T>> = 0>
    BSONCXX_INLINE explicit b_codewscope(T&& code, U&& scope)
        : code(std::forward<T>(code)), scope(std::forward<U>(scope)) {}

    stdx::string_view code;
    document::view scope;
};

///
/// free function comparator for b_codewscope
///
/// @relatesalso bsoncxx::v_noabi::types::b_codewscope
///
BSONCXX_INLINE bool operator==(const b_codewscope& lhs, const b_codewscope& rhs) {
    return lhs.code == rhs.code && lhs.scope == rhs.scope;
}

///
/// A BSON signed 32-bit integer value.
///
struct b_int32 {
    static constexpr auto type_id = type::k_int32;

    int32_t value;

    ///
    /// Conversion operator unwrapping a int32_t
    ///
    BSONCXX_INLINE operator int32_t() const {
        return value;
    }
};

///
/// free function comparator for b_int32
///
/// @relatesalso bsoncxx::v_noabi::types::b_int32
///
BSONCXX_INLINE bool operator==(const b_int32& lhs, const b_int32& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON replication timestamp value.
///
struct b_timestamp {
    static constexpr auto type_id = type::k_timestamp;

    uint32_t increment;
    uint32_t timestamp;
};

///
/// free function comparator for b_timestamp
///
/// @relatesalso bsoncxx::v_noabi::types::b_timestamp
///
BSONCXX_INLINE bool operator==(const b_timestamp& lhs, const b_timestamp& rhs) {
    return lhs.increment == rhs.increment && lhs.timestamp == rhs.timestamp;
}

///
/// A BSON 64-bit signed integer value.
///
struct b_int64 {
    static constexpr auto type_id = type::k_int64;

    int64_t value;

    ///
    /// Conversion operator unwrapping a int64_t
    ///
    BSONCXX_INLINE operator int64_t() const {
        return value;
    }
};

///
/// free function comparator for b_int64
///
/// @relatesalso bsoncxx::v_noabi::types::b_int64
///
BSONCXX_INLINE bool operator==(const b_int64& lhs, const b_int64& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON Decimal128 value.
///
struct b_decimal128 {
    static constexpr auto type_id = type::k_decimal128;

    decimal128 value;

    ///
    /// Constructor for b_decimal128.
    ///
    /// @param t
    ///   The value to wrap.
    ///
    template <typename T, detail::requires_not_t<int, detail::is_alike<b_decimal128, T>> = 0>
    BSONCXX_INLINE explicit b_decimal128(T&& t) : value(std::forward<T>(t)) {}
};

///
/// free function comparator for b_decimal128
///
/// @relatesalso bsoncxx::v_noabi::types::b_decimal128
///
BSONCXX_INLINE bool operator==(const b_decimal128& lhs, const b_decimal128& rhs) {
    return lhs.value == rhs.value;
}

///
/// A BSON min-key value.
///
struct b_minkey {
    static constexpr auto type_id = type::k_minkey;
};

///
/// free function comparator for b_minkey
///
/// @relatesalso bsoncxx::v_noabi::types::b_minkey
///
BSONCXX_INLINE bool operator==(const b_minkey&, const b_minkey&) {
    return true;
}

///
/// A BSON max-key value.
///
struct b_maxkey {
    static constexpr auto type_id = type::k_maxkey;
};

///
/// free function comparator for b_maxkey
///
/// @relatesalso bsoncxx::v_noabi::types::b_maxkey
///
BSONCXX_INLINE bool operator==(const b_maxkey&, const b_maxkey&) {
    return true;
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_double
///
/// free function comparator for b_double
///
inline bool operator!=(const b_double& lhs, const b_double& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_string
///
/// free function comparator for b_string
///
inline bool operator!=(const b_string& lhs, const b_string& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_document
///
/// free function comparator for b_document
///
inline bool operator!=(const b_document& lhs, const b_document& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_array
///
/// free function comparator for b_array
///
inline bool operator!=(const b_array& lhs, const b_array& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_binary
///
/// free function comparator for b_binary
///
inline bool operator!=(const b_binary& lhs, const b_binary& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_undefined
///
/// free function comparator for b_undefined
///
inline bool operator!=(const b_undefined& lhs, const b_undefined& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_oid
///
/// free function comparator for b_oid
///
inline bool operator!=(const b_oid& lhs, const b_oid& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_bool
///
/// free function comparator for b_bool
///
inline bool operator!=(const b_bool& lhs, const b_bool& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_date
///
/// free function comparator for b_date
///
inline bool operator!=(const b_date& lhs, const b_date& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_null
///
/// free function comparator for b_null
///
inline bool operator!=(const b_null& lhs, const b_null& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_regex
///
/// free function comparator for b_regex
///
inline bool operator!=(const b_regex& lhs, const b_regex& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_dbpointer
///
/// free function comparator for b_dbpointer
///
inline bool operator!=(const b_dbpointer& lhs, const b_dbpointer& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_code
///
/// free function comparator for b_code
///
inline bool operator!=(const b_code& lhs, const b_code& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_symbol
///
/// free function comparator for b_symbol
///
inline bool operator!=(const b_symbol& lhs, const b_symbol& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_codewscope
///
/// free function comparator for b_codewscope
///
inline bool operator!=(const b_codewscope& lhs, const b_codewscope& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_int32
///
/// free function comparator for b_int32
///
inline bool operator!=(const b_int32& lhs, const b_int32& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_timestamp
///
/// free function comparator for b_timestamp
///
inline bool operator!=(const b_timestamp& lhs, const b_timestamp& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_int64
///
/// free function comparator for b_int64
///
inline bool operator!=(const b_int64& lhs, const b_int64& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_decimal128
///
/// free function comparator for b_decimal128
///
inline bool operator!=(const b_decimal128& lhs, const b_decimal128& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_minkey
///
/// free function comparator for b_minkey
///
inline bool operator!=(const b_minkey& lhs, const b_minkey& rhs) {
    return !(lhs == rhs);
}

///
/// @relatesalso bsoncxx::v_noabi::types::b_maxkey
///
/// free function comparator for b_maxkey
///
inline bool operator!=(const b_maxkey& lhs, const b_maxkey& rhs) {
    return !(lhs == rhs);
}

}  // namespace types
}  // namespace v_noabi
}  // namespace bsoncxx

BSONCXX_POP_WARNINGS();

namespace bsoncxx {

using ::bsoncxx::v_noabi::to_string;

}  // namespace bsoncxx

namespace bsoncxx {
namespace types {

using ::bsoncxx::v_noabi::types::b_utf8;  // Deprecated.

using ::bsoncxx::v_noabi::types::operator==;
using ::bsoncxx::v_noabi::types::operator!=;

}  // namespace types
}  // namespace bsoncxx

#ifdef BSONCXX_ENUM
static_assert(false, "BSONCXX_ENUM must be undef'ed");
#endif
#pragma pop_macro("BSONCXX_ENUM")

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides entities used to represent BSON types.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {

/// @ref bsoncxx::v_noabi::to_string(v_noabi::type rhs)
std::string to_string(v_noabi::type rhs);

/// @ref bsoncxx::v_noabi::to_string(v_noabi::binary_sub_type rhs)
std::string to_string(v_noabi::binary_sub_type rhs);

namespace types {

/// @ref bsoncxx::v_noabi::types::b_utf8
/// @deprecated Use @ref bsoncxx::types::b_string instead.
struct b_utf8 {};

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_double& lhs, const v_noabi::types::b_double& rhs)
bool operator==(const v_noabi::types::b_double& lhs, const v_noabi::types::b_double& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_string& lhs, const v_noabi::types::b_string& rhs)
bool operator==(const v_noabi::types::b_string& lhs, const v_noabi::types::b_string& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_document& lhs, const v_noabi::types::b_document& rhs)
bool operator==(const v_noabi::types::b_document& lhs, const v_noabi::types::b_document& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_array& lhs, const v_noabi::types::b_array& rhs)
bool operator==(const v_noabi::types::b_array& lhs, const v_noabi::types::b_array& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_binary& lhs, const v_noabi::types::b_binary& rhs)
bool operator==(const v_noabi::types::b_binary& lhs, const v_noabi::types::b_binary& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_undefined&, const v_noabi::types::b_undefined&)
bool operator==(const v_noabi::types::b_undefined&, const v_noabi::types::b_undefined&);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_oid& lhs, const v_noabi::types::b_oid& rhs)
bool operator==(const v_noabi::types::b_oid& lhs, const v_noabi::types::b_oid& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_bool& lhs, const v_noabi::types::b_bool& rhs)
bool operator==(const v_noabi::types::b_bool& lhs, const v_noabi::types::b_bool& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_date& lhs, const v_noabi::types::b_date& rhs)
bool operator==(const v_noabi::types::b_date& lhs, const v_noabi::types::b_date& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_null&, const v_noabi::types::b_null&)
bool operator==(const v_noabi::types::b_null&, const v_noabi::types::b_null&);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_regex& lhs, const v_noabi::types::b_regex& rhs)
bool operator==(const v_noabi::types::b_regex& lhs, const v_noabi::types::b_regex& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_dbpointer& lhs, const v_noabi::types::b_dbpointer& rhs)
bool operator==(const v_noabi::types::b_dbpointer& lhs, const v_noabi::types::b_dbpointer& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_code& lhs, const v_noabi::types::b_code& rhs)
bool operator==(const v_noabi::types::b_code& lhs, const v_noabi::types::b_code& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_symbol& lhs, const v_noabi::types::b_symbol& rhs)
bool operator==(const v_noabi::types::b_symbol& lhs, const v_noabi::types::b_symbol& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_codewscope& lhs, const v_noabi::types::b_codewscope& rhs)
bool operator==(const v_noabi::types::b_codewscope& lhs, const v_noabi::types::b_codewscope& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_int32& lhs, const v_noabi::types::b_int32& rhs)
bool operator==(const v_noabi::types::b_int32& lhs, const v_noabi::types::b_int32& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_timestamp& lhs, const v_noabi::types::b_timestamp& rhs)
bool operator==(const v_noabi::types::b_timestamp& lhs, const v_noabi::types::b_timestamp& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_int64& lhs, const v_noabi::types::b_int64& rhs)
bool operator==(const v_noabi::types::b_int64& lhs, const v_noabi::types::b_int64& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_decimal128& lhs, const v_noabi::types::b_decimal128& rhs)
bool operator==(const v_noabi::types::b_decimal128& lhs, const v_noabi::types::b_decimal128& rhs);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_minkey&, const v_noabi::types::b_minkey&)
bool operator==(const v_noabi::types::b_minkey&, const v_noabi::types::b_minkey&);

/// @ref bsoncxx::v_noabi::types::operator==(const v_noabi::types::b_maxkey&, const v_noabi::types::b_maxkey&)
bool operator==(const v_noabi::types::b_maxkey&, const v_noabi::types::b_maxkey&);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_double& lhs, const v_noabi::types::b_double& rhs)
bool operator!=(const v_noabi::types::b_double& lhs, const v_noabi::types::b_double& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_string& lhs, const v_noabi::types::b_string& rhs)
bool operator!=(const v_noabi::types::b_string& lhs, const v_noabi::types::b_string& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_document& lhs, const v_noabi::types::b_document& rhs)
bool operator!=(const v_noabi::types::b_document& lhs, const v_noabi::types::b_document& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_array& lhs, const v_noabi::types::b_array& rhs)
bool operator!=(const v_noabi::types::b_array& lhs, const v_noabi::types::b_array& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_binary& lhs, const v_noabi::types::b_binary& rhs)
bool operator!=(const v_noabi::types::b_binary& lhs, const v_noabi::types::b_binary& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_undefined& lhs, const v_noabi::types::b_undefined& rhs)
bool operator!=(const v_noabi::types::b_undefined& lhs, const v_noabi::types::b_undefined& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_oid& lhs, const v_noabi::types::b_oid& rhs)
bool operator!=(const v_noabi::types::b_oid& lhs, const v_noabi::types::b_oid& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_bool& lhs, const v_noabi::types::b_bool& rhs)
bool operator!=(const v_noabi::types::b_bool& lhs, const v_noabi::types::b_bool& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_date& lhs, const v_noabi::types::b_date& rhs)
bool operator!=(const v_noabi::types::b_date& lhs, const v_noabi::types::b_date& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_null& lhs, const v_noabi::types::b_null& rhs)
bool operator!=(const v_noabi::types::b_null& lhs, const v_noabi::types::b_null& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_regex& lhs, const v_noabi::types::b_regex& rhs)
bool operator!=(const v_noabi::types::b_regex& lhs, const v_noabi::types::b_regex& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_dbpointer& lhs, const v_noabi::types::b_dbpointer& rhs)
bool operator!=(const v_noabi::types::b_dbpointer& lhs, const v_noabi::types::b_dbpointer& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_code& lhs, const v_noabi::types::b_code& rhs)
bool operator!=(const v_noabi::types::b_code& lhs, const v_noabi::types::b_code& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_symbol& lhs, const v_noabi::types::b_symbol& rhs)
bool operator!=(const v_noabi::types::b_symbol& lhs, const v_noabi::types::b_symbol& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_codewscope& lhs, const v_noabi::types::b_codewscope& rhs)
bool operator!=(const v_noabi::types::b_codewscope& lhs, const v_noabi::types::b_codewscope& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_int32& lhs, const v_noabi::types::b_int32& rhs)
bool operator!=(const v_noabi::types::b_int32& lhs, const v_noabi::types::b_int32& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_timestamp& lhs, const v_noabi::types::b_timestamp& rhs)
bool operator!=(const v_noabi::types::b_timestamp& lhs, const v_noabi::types::b_timestamp& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_int64& lhs, const v_noabi::types::b_int64& rhs)
bool operator!=(const v_noabi::types::b_int64& lhs, const v_noabi::types::b_int64& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_decimal128& lhs, const v_noabi::types::b_decimal128& rhs)
bool operator!=(const v_noabi::types::b_decimal128& lhs, const v_noabi::types::b_decimal128& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_minkey& lhs, const v_noabi::types::b_minkey& rhs)
bool operator!=(const v_noabi::types::b_minkey& lhs, const v_noabi::types::b_minkey& rhs);

/// @ref bsoncxx::v_noabi::types::operator!=(const v_noabi::types::b_maxkey& lhs, const v_noabi::types::b_maxkey& rhs)
bool operator!=(const v_noabi::types::b_maxkey& lhs, const v_noabi::types::b_maxkey& rhs);

}  // namespace types

}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
