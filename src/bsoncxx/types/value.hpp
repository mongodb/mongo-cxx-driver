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

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <bsoncxx/types.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace types {

///
/// A variant that can contain any BSON type.
///
/// @warning
///   It is undefined behavior to call the wrong get_<type> method. Check
///   the underlying type() first.
///
class BSONCXX_API value {
   public:
    ///
    /// Construct a value from a BSON double.
    ///
    explicit value(b_double) noexcept;

    ///
    /// Construct a value from a BSON UTF-8 string.
    ///
    explicit value(b_utf8) noexcept;

    ///
    /// Construct a value from a BSON document.
    ///
    explicit value(b_document) noexcept;

    ///
    /// Construct a value from a BSON array.
    ///
    explicit value(b_array) noexcept;

    ///
    /// Construct a value from a BSON binary datum.
    ///
    explicit value(b_binary) noexcept;

    ///
    /// Construct a value from a BSON undefined.
    ///
    explicit value(b_undefined) noexcept;

    ///
    /// Construct a value from a BSON ObjectId.
    ///
    explicit value(b_oid) noexcept;

    ///
    /// Construct a value from a BSON boolean.
    ///
    explicit value(b_bool) noexcept;

    ///
    /// Construct a value from a BSON date.
    ///
    explicit value(b_date) noexcept;

    ///
    /// Construct a value from a BSON null.
    ///
    explicit value(b_null) noexcept;

    ///
    /// Construct a value from a BSON regex.
    ///
    explicit value(b_regex) noexcept;

    ///
    /// Construct a value from a BSON DBPointer.
    ///
    explicit value(b_dbpointer) noexcept;

    ///
    /// Construct a value from a BSON JavaScript code.
    ///
    explicit value(b_code) noexcept;

    ///
    /// Construct a value from a BSON symbol.
    ///
    explicit value(b_symbol) noexcept;

    ///
    /// Construct a value from a BSON JavaScript code with scope.
    ///
    explicit value(b_codewscope) noexcept;

    ///
    /// Construct a value from a BSON 32-bit signed integer.
    ///
    explicit value(b_int32) noexcept;

    ///
    /// Construct a value from a BSON replication timestamp.
    ///
    explicit value(b_timestamp) noexcept;

    ///
    /// Construct a value from a BSON 64-bit signed integer.
    ///
    explicit value(b_int64) noexcept;

    ///
    /// Construct a value from a BSON Decimal128.
    ///
    explicit value(b_decimal128) noexcept;

    ///
    /// Construct a value from a BSON min-key.
    ///
    explicit value(b_minkey) noexcept;

    ///
    /// Construct a value from a BSON max-key.
    ///
    explicit value(b_maxkey) noexcept;

    value(const value&) noexcept;
    value& operator=(const value&) noexcept;

    ~value();

    ///
    /// @{
    ///
    /// Compare two values for equality
    ///
    /// @relates value
    ///
    friend BSONCXX_API bool BSONCXX_CALL operator==(const value&, const value&);
    friend BSONCXX_API bool BSONCXX_CALL operator!=(const value&, const value&);
    ///
    /// @}
    ///

    ///
    /// @return The type of the underlying BSON value stored in this object.
    ///
    bsoncxx::type type() const;

    ///
    /// @return The underlying BSON double value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_double& get_double() const;

    ///
    /// @return The underlying BSON UTF-8 string value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_utf8& get_utf8() const;

    ///
    /// @return The underlying BSON document value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_document& get_document() const;

    ///
    /// @return The underlying BSON array value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_array& get_array() const;

    ///
    /// @return The underlying BSON binary data value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_binary& get_binary() const;

    ///
    /// @return The underlying BSON undefined value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_undefined& get_undefined() const;

    ///
    /// @return The underlying BSON ObjectId value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_oid& get_oid() const;

    ///
    /// @return The underlying BSON boolean value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_bool& get_bool() const;

    ///
    /// @return The underlying BSON date value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_date& get_date() const;

    ///
    /// @return The underlying BSON null value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_null& get_null() const;

    ///
    /// @return The underlying BSON regex value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_regex& get_regex() const;

    ///
    /// @return The underlying BSON DBPointer value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_dbpointer& get_dbpointer() const;

    ///
    /// @return The underlying BSON JavaScript code value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_code& get_code() const;

    ///
    /// @return The underlying BSON symbol value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_symbol& get_symbol() const;

    ///
    /// @return The underlying BSON JavaScript code with scope value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_codewscope& get_codewscope() const;

    ///
    /// @return The underlying BSON 32-bit signed integer value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_int32& get_int32() const;

    ///
    /// @return The underlying BSON replication timestamp value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_timestamp& get_timestamp() const;

    ///
    /// @return The underlying BSON 64-bit signed integer value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_int64& get_int64() const;

    ///
    /// @return The underlying BSON Decimal128 value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_decimal128& get_decimal128() const;

    ///
    /// @return The underlying BSON min-key value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_minkey& get_minkey() const;

    ///
    /// @return The underlying BSON max-key value.
    ///
    /// @warning
    ///   It is undefined behavior to call the wrong get_<type> method. Check
    ///   the underlying type() first.
    ///
    const b_maxkey& get_maxkey() const;

   private:
    void BSONCXX_PRIVATE destroy() noexcept;

    bsoncxx::type _type;
    union {
        struct b_double _b_double;
        struct b_utf8 _b_utf8;
        struct b_document _b_document;
        struct b_array _b_array;
        struct b_binary _b_binary;
        struct b_undefined _b_undefined;
        struct b_oid _b_oid;
        struct b_bool _b_bool;
        struct b_date _b_date;
        struct b_null _b_null;
        struct b_regex _b_regex;
        struct b_dbpointer _b_dbpointer;
        struct b_code _b_code;
        struct b_symbol _b_symbol;
        struct b_codewscope _b_codewscope;
        struct b_int32 _b_int32;
        struct b_timestamp _b_timestamp;
        struct b_int64 _b_int64;
        struct b_decimal128 _b_decimal128;
        struct b_minkey _b_minkey;
        struct b_maxkey _b_maxkey;
    };
};

// sfinae in the bool return to avoid competing with the value == value
// operators
template <typename T>
using not_value = typename std::enable_if<
    std::is_constructible<value, T>::value &&
        !std::is_same<typename std::remove_reference<T>::type, value>::value,
    bool>::type;

// these all return bool
template <typename T>
BSONCXX_INLINE not_value<T> operator==(const value& lhs, T&& rhs) {
    return lhs == value{std::forward<T>(rhs)};
}

template <typename T>
BSONCXX_INLINE not_value<T> operator==(T&& lhs, const value& rhs) {
    return value{std::forward<T>(lhs)} == rhs;
}

template <typename T>
BSONCXX_INLINE not_value<T> operator!=(const value& lhs, T&& rhs) {
    return lhs != value{std::forward<T>(rhs)};
}

template <typename T>
BSONCXX_INLINE not_value<T> operator!=(T&& lhs, const value& rhs) {
    return value{std::forward<T>(lhs)} != rhs;
}

}  // namespace types

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
