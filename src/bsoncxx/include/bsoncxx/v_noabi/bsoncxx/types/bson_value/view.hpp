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

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <bsoncxx/document/element-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>

#include <bsoncxx/stdx/type_traits.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {
///
/// A view-only variant that can contain any BSON type.
///
/// @warning
///   Calling the wrong get_<type> method will cause an exception
///   to be thrown.
///
class view {
   public:
    ///
    /// Construct a bson_value::view from the provided BSON type.
    ///
    /// @{
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_double v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_string v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_document v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_array v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_binary v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_undefined v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_oid v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_bool v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_date v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_null v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_regex v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_dbpointer v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_code v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_symbol v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_codewscope v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_int32 v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_timestamp v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_int64 v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_decimal128 v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_maxkey v) noexcept;
    explicit BSONCXX_ABI_EXPORT_CDECL() view(b_minkey v) noexcept;
    /// @}
    ///

    ///
    /// Default constructs a bson_value::view. The resulting view will be initialized
    /// to point at a bson_value of type k_null.
    ///
    BSONCXX_ABI_EXPORT_CDECL() view() noexcept;

    BSONCXX_ABI_EXPORT_CDECL() view(const view&) noexcept;
    BSONCXX_ABI_EXPORT_CDECL(view&) operator=(const view&) noexcept;

    BSONCXX_ABI_EXPORT_CDECL() ~view();

    ///
    /// @relates bsoncxx::v_noabi::types::bson_value::view
    ///
    /// Compare two bson_value::views for equality
    ///
    /// @{
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator==(const bson_value::view&,
                                                     const bson_value::view&);
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator!=(const bson_value::view&,
                                                     const bson_value::view&);
    /// @}
    ///

    ///
    /// Returns the type of the underlying BSON value stored in this object.
    ///
    BSONCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::type) type() const;

    ///
    /// Returns the underlying BSON double value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_double&) get_double() const;

    ///
    /// Returns the underlying BSON UTF-8 string value.
    ///
    /// @deprecated use get_string instead.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_DEPRECATED BSONCXX_ABI_EXPORT_CDECL(const b_string&) get_utf8() const;

    ///
    /// Returns the underlying BSON UTF-8 string value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_string&) get_string() const;

    ///
    /// Returns the underlying BSON document value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_document&) get_document() const;

    ///
    /// Returns the underlying BSON array value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_array&) get_array() const;

    ///
    /// Returns the underlying BSON binary data value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_binary&) get_binary() const;

    ///
    /// Returns the underlying BSON undefined value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_undefined&) get_undefined() const;

    ///
    /// Returns the underlying BSON ObjectId value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_oid&) get_oid() const;

    ///
    /// Returns the underlying BSON boolean value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_bool&) get_bool() const;

    ///
    /// Returns the underlying BSON date value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_date&) get_date() const;

    ///
    /// Returns the underlying BSON null value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_null&) get_null() const;

    ///
    /// Returns the underlying BSON regex value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_regex&) get_regex() const;

    ///
    /// Returns the underlying BSON DBPointer value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_dbpointer&) get_dbpointer() const;

    ///
    /// Returns the underlying BSON JavaScript code value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_code&) get_code() const;

    ///
    /// Returns the underlying BSON symbol value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_symbol&) get_symbol() const;

    ///
    /// Returns the underlying BSON JavaScript code with scope value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_codewscope&) get_codewscope() const;

    ///
    /// Returns the underlying BSON 32-bit signed integer value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_int32&) get_int32() const;

    ///
    /// Returns the underlying BSON replication timestamp value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_timestamp&) get_timestamp() const;

    ///
    /// Returns the underlying BSON 64-bit signed integer value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_int64&) get_int64() const;

    ///
    /// Returns the underlying BSON Decimal128 value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_decimal128&) get_decimal128() const;

    ///
    /// Returns the underlying BSON min-key value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_minkey&) get_minkey() const;

    ///
    /// Returns the underlying BSON max-key value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const b_maxkey&) get_maxkey() const;

   private:
    friend ::bsoncxx::v_noabi::types::bson_value::value;
    friend ::bsoncxx::v_noabi::document::element;

    view(const std::uint8_t* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen);
    view(void* internal_value) noexcept;

    void _init(void* internal_value) noexcept;

    void destroy() noexcept;

    bsoncxx::v_noabi::type _type;

    union {
        struct b_double _b_double;
        struct b_string _b_string;
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

template <typename T>
using is_bson_view_compatible = detail::conjunction<
    std::is_constructible<bson_value::view, T>,
    detail::negation<detail::disjunction<detail::is_alike<T, bson_value::view>,
                                         detail::is_alike<T, bson_value::value>>>>;

template <typename T>
using not_view = is_bson_view_compatible<T>;

///
/// Compares a view with a type representable as a view.
///
/// @par Constraints
/// - @ref bsoncxx::v_noabi::types::bson_value::view is constructible from `T`.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, is_bson_view_compatible<T>>  //
operator==(const bson_value::view& lhs, T&& rhs) {
    return lhs == bson_value::view{std::forward<T>(rhs)};
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, is_bson_view_compatible<T>>  //
operator==(T&& lhs, const bson_value::view& rhs) {
    return bson_value::view{std::forward<T>(lhs)} == rhs;
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, is_bson_view_compatible<T>>  //
operator!=(const bson_value::view& lhs, T&& rhs) {
    return lhs != bson_value::view{std::forward<T>(rhs)};
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, is_bson_view_compatible<T>>  //
operator!=(T&& lhs, const bson_value::view& rhs) {
    return bson_value::view{std::forward<T>(lhs)} != rhs;
}

/// @}
///

}  // namespace bson_value
}  // namespace types
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace types {
namespace bson_value {

using ::bsoncxx::v_noabi::types::bson_value::operator==;
using ::bsoncxx::v_noabi::types::bson_value::operator!=;

}  // namespace bson_value
}  // namespace types
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::types::bson_value::view.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace types {
namespace bson_value {

/// @ref bsoncxx::v_noabi::types::bson_value::operator==(const v_noabi::types::bson_value::view& lhs, T&& rhs)
template <typename T>
bool operator==(const v_noabi::types::bson_value::view& lhs, T&& rhs);

/// @ref bsoncxx::v_noabi::types::bson_value::operator==(T&& lhs, const v_noabi::types::bson_value::view& rhs)
template <typename T>
bool operator==(T&& lhs, const v_noabi::types::bson_value::view& rhs);

/// @ref bsoncxx::v_noabi::types::bson_value::operator!=(const v_noabi::types::bson_value::view& lhs, T&& rhs)
template <typename T>
bool operator!=(const v_noabi::types::bson_value::view& lhs, T&& rhs);

/// @ref bsoncxx::v_noabi::types::bson_value::operator!=(T&& lhs, const v_noabi::types::bson_value::view& rhs)
template <typename T>
bool operator!=(T&& lhs, const v_noabi::types::bson_value::view& rhs);

}  // namespace bson_value
}  // namespace types
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
