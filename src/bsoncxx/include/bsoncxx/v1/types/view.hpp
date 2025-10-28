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

#include <bsoncxx/v1/types/view-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/view.hpp> // IWYU pragma: export
#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/decimal128.hpp> // IWYU pragma: export
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/document/view.hpp> // IWYU pragma: export
#include <bsoncxx/v1/oid.hpp>           // IWYU pragma: export
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/id.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint>
#include <cstring>
#include <system_error>
#include <type_traits>

namespace bsoncxx {
namespace v1 {
namespace types {

// BSONCXX_V1_TYPES_XMACRO: update below.

///
/// BSON type value "64-bit Binary Floating Point".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_double {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_double;

    ///
    /// The represented value.
    ///
    double value = {};

    ///
    /// Zero-initialize the represented value.
    ///
    b_double() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_double(double value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator double() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_double const& lhs, b_double const& rhs) {
        BSONCXX_PRIVATE_WARNINGS_PUSH();
        BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));
        return lhs.value == rhs.value;
        BSONCXX_PRIVATE_WARNINGS_POP();
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_double const& lhs, b_double const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "UTF-8 String".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_string {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_string;

    ///
    /// The represented value.
    ///
    v1::stdx::string_view value;

    ///
    /// Zero-initialize the represented value.
    ///
    b_string() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_string(v1::stdx::string_view value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator v1::stdx::string_view() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_string const& lhs, b_string const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_string const& lhs, b_string const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Embedded Document".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_document {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_document;

    ///
    /// The represented value.
    ///
    v1::document::view value;

    ///
    /// Zero-initialize the represented value.
    ///
    b_document() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_document(v1::document::view value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator v1::document::view() const {
        return value;
    }

    v1::document::view view() {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_document const& lhs, b_document const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_document const& lhs, b_document const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Array".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_array {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_array;

    ///
    /// The represented value.
    ///
    v1::array::view value;

    ///
    /// Zero-initialize the represented value.
    ///
    b_array() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_array(v1::array::view value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator v1::array::view() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_array const& lhs, b_array const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_array const& lhs, b_array const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Binary Data".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_binary {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_binary;

    ///
    /// The represented value's binary subtype component.
    ///
    binary_subtype subtype = {};

    ///
    /// The represented value's length of binary data.
    ///
    std::uint32_t size = {};

    ///
    /// The represented value's binary data.
    ///
    std::uint8_t const* bytes = {};

    ///
    /// Zero-initialize the represented value.
    ///
    b_binary() = default;

    ///
    /// Initialize with the given binary subtype and pointer to binary data.
    ///
    b_binary(binary_subtype subtype, std::uint32_t size, std::uint8_t const* bytes)
        : subtype{subtype}, size{size}, bytes{bytes} {}

    ///
    /// Compare equal when the binary subtype and pointed-to bytes compare equal.
    ///
    /// An "empty" range of bytes (when @ref bytes is null or @ref size is zero) only compares equal to another
    /// "empty" range of bytes. The value of @ref subtype is ignored for an empty range of bytes.
    ///
    /// @{
    friend bool operator==(b_binary const& lhs, b_binary const& rhs) {
        if (lhs.size != rhs.size) {
            return false;
        }

        if (!lhs.bytes != !rhs.bytes) {
            return false;
        }

        if (!lhs.bytes || lhs.size == 0u) {
            return true; // Empty.
        }

        return lhs.subtype == rhs.subtype && (std::memcmp(lhs.bytes, rhs.bytes, lhs.size) == 0);
    }

    friend bool operator!=(b_binary const& lhs, b_binary const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

///
/// BSON type value "Undefined (Value)".
///
/// @deprecated This BSON type is deprecated.
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_undefined {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_undefined;

    ///
    /// Return true.
    ///
    friend bool operator==(b_undefined const&, b_undefined const&) {
        return true;
    }

    ///
    /// Return false.
    ///
    friend bool operator!=(b_undefined const& lhs, b_undefined const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "ObjectID".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_oid {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_oid;

    ///
    /// The represented value.
    ///
    v1::oid value;

    ///
    /// Zero-initialize the represented value.
    ///
    b_oid() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_oid(v1::oid const& value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator v1::oid() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_oid const& lhs, b_oid const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_oid const& lhs, b_oid const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Boolean".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_bool {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_bool;

    ///
    /// The represented value.
    ///
    bool value = false;

    ///
    /// Zero-initialize the represented value.
    ///
    b_bool() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_bool(bool value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator bool() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_bool const& lhs, b_bool const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_bool const& lhs, b_bool const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "UTC Datetime".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_date {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_date;

    ///
    /// The represented value (milliseconds relative to the Unix epoch).
    ///
    std::chrono::milliseconds value = {};

    ///
    /// Zero-initialize the represented value.
    ///
    b_date() = default;

    ///
    /// Initialize with `value` (milliseconds relative to the Unix epoch).
    ///
    explicit b_date(std::chrono::milliseconds value) : value{value} {}

    ///
    /// Initialize with `tp` (assuming the epoch of `std::chrono::system_clock` is the Unix epoch).
    ///
    explicit b_date(std::chrono::system_clock::time_point tp)
        : value{std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch())} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator std::chrono::milliseconds() const {
        return value;
    }

    explicit operator std::chrono::system_clock::time_point() const {
        return std::chrono::system_clock::time_point(
            std::chrono::duration_cast<std::chrono::system_clock::duration>(value));
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_date const& lhs, b_date const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_date const& lhs, b_date const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Null Value".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_null {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_null;

    ///
    /// Return true.
    ///
    friend bool operator==(b_null const& /* lhs */, b_null const& /* rhs */) {
        return true;
    }

    ///
    /// Return false.
    ///
    friend bool operator!=(b_null const& lhs, b_null const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Regular Expression".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_regex {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_regex;

    ///
    /// The represented value's "pattern" component.
    ///
    v1::stdx::string_view regex;

    ///
    /// The represented value's "options" component.
    ///
    v1::stdx::string_view options;

    ///
    /// Zero-initialize the represented value.
    ///
    b_regex() = default;

    ///
    /// Initialize with `regex` and an empty @ref options.
    ///
    explicit b_regex(v1::stdx::string_view regex) : regex{regex}, options{} {}

    ///
    /// Initialize with `regex` and `options`.
    ///
    b_regex(v1::stdx::string_view regex, v1::stdx::string_view options) : regex{regex}, options{options} {}

    ///
    /// Equivalent to `lhs.regex == rhs.regex && lhs.options == rhs.options`.
    ///
    friend bool operator==(b_regex const& lhs, b_regex const& rhs) {
        return lhs.regex == rhs.regex && lhs.options == rhs.options;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_regex const& lhs, b_regex const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "DBPointer".
///
/// @deprecated This BSON type is deprecated.
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_dbpointer {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_dbpointer;

    ///
    /// The represented value's "$ref" (namespace) component.
    ///
    v1::stdx::string_view collection;

    ///
    /// The represented value's "$id" (ObjectID) component.
    ///
    v1::oid value;

    ///
    /// Zero-initialize the represented value.
    ///
    b_dbpointer() = default;

    ///
    /// Initialize with `collection` and `value`.
    ///
    b_dbpointer(v1::stdx::string_view collection, v1::oid value) : collection{collection}, value{value} {}

    ///
    /// Equivalent to `lhs.collection == rhs.collection && lhs.value == rhs.value`.
    ///
    friend bool operator==(b_dbpointer const& lhs, b_dbpointer const& rhs) {
        return lhs.collection == rhs.collection && lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_dbpointer const& lhs, b_dbpointer const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "JavaScript Code".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_code {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_code;

    ///
    /// The represented value.
    ///
    v1::stdx::string_view code;

    ///
    /// Zero-initialize the represented value.
    ///
    b_code() = default;

    ///
    /// Initialize with `code`.
    ///
    explicit b_code(v1::stdx::string_view code) : code{code} {}

    ///
    /// Implicitly convert to @ref code.
    ///
    /* explicit(false) */ operator v1::stdx::string_view() const {
        return code;
    }

    ///
    /// Equivalent to `lhs.code == rhs.code`.
    ///
    friend bool operator==(b_code const& lhs, b_code const& rhs) {
        return lhs.code == rhs.code;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_code const& lhs, b_code const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Symbol".
///
/// @deprecated This BSON type is deprecated.
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_symbol {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_symbol;

    ///
    /// The represented value.
    ///
    v1::stdx::string_view symbol;

    ///
    /// Zero-initialize the represented value.
    ///
    b_symbol() = default;

    ///
    /// Initialize with `symbol`.
    ///
    explicit b_symbol(v1::stdx::string_view symbol) : symbol{symbol} {}

    ///
    /// Implicitly convert to @ref symbol.
    ///
    /* explicit(false) */ operator v1::stdx::string_view() const {
        return symbol;
    }

    ///
    /// Equivalent to `lhs.symbol == rhs.symbol`.
    ///
    friend bool operator==(b_symbol const& lhs, b_symbol const& rhs) {
        return lhs.symbol == rhs.symbol;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_symbol const& lhs, b_symbol const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "JavaScript Code With Scope".
///
/// @deprecated This BSON type is deprecated.
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_codewscope {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_codewscope;

    ///
    /// The represented value's "$code" component.
    ///
    v1::stdx::string_view code;

    ///
    /// The represented value's "$scope" component.
    ///
    v1::document::view scope;

    ///
    /// Zero-initialize the represented value.
    ///
    b_codewscope() = default;

    ///
    /// Initialize with `code` and `scope`.
    ///
    b_codewscope(v1::stdx::string_view code, v1::document::view scope) : code{code}, scope{scope} {}

    ///
    /// Equivalent to `lhs.code == rhs.code && lhs.scope == rhs.scope`.
    ///
    friend bool operator==(b_codewscope const& lhs, b_codewscope const& rhs) {
        return lhs.code == rhs.code && lhs.scope == rhs.scope;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_codewscope const& lhs, b_codewscope const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "32-bit Integer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_int32 {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_int32;

    ///
    /// The represented value.
    ///
    std::int32_t value = {};

    ///
    /// Zero-initialize the represented value.
    ///
    b_int32() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_int32(std::int32_t value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator std::int32_t() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_int32 const& lhs, b_int32 const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_int32 const& lhs, b_int32 const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Timestamp".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_timestamp {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_timestamp;

    ///
    /// The represented value's "i" component.
    ///
    std::uint32_t increment = {};

    ///
    /// The represented value's "t" component.
    ///
    std::uint32_t timestamp = {};

    ///
    /// Zero-initialize the represented value.
    ///
    b_timestamp() = default;

    ///
    /// Initialize with `increment` and `timestamp`.
    ///
    b_timestamp(std::uint32_t increment, std::uint32_t timestamp) : increment{increment}, timestamp{timestamp} {}

    ///
    /// Equivalent to `lhs.timestamp == rhs.timestamp`.
    ///
    friend bool operator==(b_timestamp const& lhs, b_timestamp const& rhs) {
        return lhs.timestamp == rhs.timestamp;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_timestamp const& lhs, b_timestamp const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "64-bit Integer".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_int64 {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_int64;

    ///
    /// The represented value.
    ///
    std::int64_t value = {};

    ///
    /// Zero-initialize the represented value.
    ///
    b_int64() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_int64(std::int64_t value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator std::int64_t() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_int64 const& lhs, b_int64 const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_int64 const& lhs, b_int64 const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Decimal128".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_decimal128 {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_decimal128;

    ///
    /// The represented value.
    ///
    v1::decimal128 value;

    ///
    /// Zero-initialize the represented value.
    ///
    b_decimal128() = default;

    ///
    /// Initialize with `value`.
    ///
    explicit b_decimal128(v1::decimal128 value) : value{value} {}

    ///
    /// Implicitly convert to @ref value.
    ///
    /* explicit(false) */ operator v1::decimal128() const {
        return value;
    }

    ///
    /// Equivalent to `lhs.value == rhs.value`.
    ///
    friend bool operator==(b_decimal128 const& lhs, b_decimal128 const& rhs) {
        return lhs.value == rhs.value;
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(b_decimal128 const& lhs, b_decimal128 const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Max Key".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_maxkey {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_maxkey;

    ///
    /// Return true.
    ///
    friend bool operator==(b_maxkey const&, b_maxkey const&) {
        return true;
    }

    ///
    /// Return false.
    ///
    friend bool operator!=(b_maxkey const& lhs, b_maxkey const& rhs) {
        return !(lhs == rhs);
    }
};

///
/// BSON type value "Min Key".
///
/// @attention This feature is experimental! It is not ready for use!
///
struct b_minkey {
    ///
    /// The type represented by this BSON type value.
    ///
    static constexpr id type_id = id::k_minkey;

    ///
    /// Return true.
    ///
    friend bool operator==(b_minkey const&, b_minkey const&) {
        return true;
    }

    ///
    /// Return false.
    ///
    friend bool operator!=(b_minkey const& lhs, b_minkey const& rhs) {
        return !(lhs == rhs);
    }
};

// BSONCXX_V1_TYPES_XMACRO: update above.

#pragma push_macro("X")
#undef X
#define X(_name, _val) BSONCXX_PRIVATE_INLINE_CXX17 constexpr id b_##_name::type_id;
BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

///
/// A non-owning, read-only union of BSON type values.
///
/// @note This class only represents the **value** of a BSON element without its key.
/// @ref bsoncxx::v1::element::view represents a BSON element including its key.
///
/// @attention This feature is experimental! It is not ready for use!
///
class view {
   private:
    id _id;

#pragma push_macro("X")
#undef X
#define X(_name, _value) b_##_name _b_##_name;

    union {
        BSONCXX_V1_TYPES_XMACRO(X)
    };
#pragma pop_macro("X")

    template <typename T>
    using is_view = detail::is_alike<T, view>;

    template <typename T>
    struct is_equality_comparable_with
        : detail::conjunction<detail::negation<is_view<T>>, std::is_constructible<view, T>> {};

   public:
    /// Initialize with @ref bsoncxx::v1::types::b_null.
    view() : _id{id::k_null}, _b_null{} {}

#pragma push_macro("X")
#undef X
#define X(_name, _value) \
    /* explicit(false) */ view(b_##_name v) : _id{v.type_id}, _b_##_name{v} {}

    ///
    /// Implicitly convert `v`.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Return the type of the underlying BSON type value.
    ///
    id type_id() const {
        return _id;
    }

#pragma push_macro("X")
#undef X
#define X(_name, _value) BSONCXX_ABI_EXPORT_CDECL(b_##_name) get_##_name() const;

    ///
    /// Return the requested underlying BSON type value.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::types::view::errc::type_mismatch if the underlying
    /// BSON type value does not match the requested type.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Compare equal when the underlying BSON type values have the same type and compare equal.
    ///
    /// Equivalent to:
    /// ```cpp
    /// lhs.type_id() == rhs.type_id() && lhs.get_type() == rhs.get_type()
    /// ```
    /// where `get_type` is the correct name for the underlying BSON type values (e.g. `get_double()` when
    /// `type_id() == bsoncxx::v1::types::id::k_double`).
    ///
    /// When either `lhs.type_id()` or `rhs.type_id()` return an unsupported value (not defined by @ref
    /// bsoncxx::v1::types::id or handled by @ref BSONCXX_V1_TYPES_XMACRO), the result is unspecified.
    ///
    friend bool operator==(view const& lhs, view const& rhs) {
        if (lhs.type_id() != rhs.type_id()) {
            return false;
        }

#pragma push_macro("X")
#undef X
#define X(_name, _value)           \
    case v1::types::id::k_##_name: \
        return lhs.get_##_name() == rhs.get_##_name();

        switch (lhs.type_id()) {
            BSONCXX_V1_TYPES_XMACRO(X)

            default:
                return true;
        }
#pragma pop_macro("X")
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(view const& lhs, view const& rhs) {
        return !(lhs == rhs);
    }

    ///
    /// Equivalent to `v == e.type_view()`.
    ///
    /// @{
    friend bool operator==(view const& v, v1::element::view const& e) {
        return v == e.type_view();
    }

    friend bool operator==(v1::element::view const& e, view const& v) {
        return v == e.type_view();
    }
    /// @}
    ///

    ///
    /// Equivalent to `!(v == e)`.
    ///
    /// @{
    friend bool operator!=(view const& v, v1::element::view const& e) {
        return !(v == e);
    }

    friend bool operator!=(v1::element::view const& e, view const& v) {
        return !(v == e);
    }
    /// @}
    ///

    ///
    /// Equivalent to `v == bsoncxx::v1::types::view{value}`.
    ///
    /// @par Constraints:
    /// - `T` is not @ref bsoncxx::v1::types::view.
    /// - @ref bsoncxx::v1::types::view is constructible with `T`.
    ///
    /// @{
    template <typename T, detail::enable_if_t<is_equality_comparable_with<T>::value>* = nullptr>
    friend bool operator==(view const& v, T const& value) {
        return v == view{value};
    }

    template <typename T, detail::enable_if_t<is_equality_comparable_with<T>::value>* = nullptr>
    friend bool operator==(T const& value, view const& v) {
        return v == view{value};
    }
    /// @}
    ///

    ///
    /// Equivalent to `!(v == value)`.
    ///
    /// @{
    template <typename T, detail::enable_if_t<is_equality_comparable_with<T>::value>* = nullptr>
    friend bool operator!=(view const& v, T const& value) {
        return !(v == value);
    }

    template <typename T, detail::enable_if_t<is_equality_comparable_with<T>::value>* = nullptr>
    friend bool operator!=(T const& value, view const& v) {
        return !(v == value);
    }
    /// @}
    ///

    ///
    /// Errors codes which may be returned by @ref bsoncxx::v1::types::view.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,          ///< Zero.
        type_mismatch, ///< Requested type does not match the underlying type.
    };

    ///
    /// The error category for @ref bsoncxx::v1::types::view::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }

    class internal;
};

} // namespace types
} // namespace v1
} // namespace bsoncxx

template <>
struct std::is_error_code_enum<bsoncxx::v1::types::view::errc> : true_type {};

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides non-owning, read-only entities representing a BSON type value.
///
/// @note A "BSON type value" refers to the value of a BSON element without its key.
///
/// @par Includes
/// - @ref bsoncxx/v1/array/view.hpp
/// - @ref bsoncxx/v1/decimal128.hpp
/// - @ref bsoncxx/v1/document/view.hpp
/// - @ref bsoncxx/v1/oid.hpp
/// - @ref bsoncxx/v1/types/id.hpp
///
