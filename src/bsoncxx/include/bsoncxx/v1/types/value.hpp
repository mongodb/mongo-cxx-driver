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

#include <bsoncxx/v1/types/value-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/view-fwd.hpp>
#include <bsoncxx/v1/decimal128-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/oid-fwd.hpp>
#include <bsoncxx/v1/types/id-fwd.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>
#include <vector>

namespace bsoncxx {
namespace v1 {
namespace error {
namespace category {

///
/// Declares error categories for error codes declared in @ref bsoncxx::v1::error::types.
///
namespace types {

///
/// The error category for @ref bsoncxx::v1::error::types::value.
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) value();

} // namespace types
} // namespace category
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace bsoncxx {
namespace v1 {
namespace error {

///
/// Declares error codes returned by @ref bsoncxx::v1::types interfaces.
///
namespace types {

///
/// Errors codes which may be returned by @ref bsoncxx::v1::types::value.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class value {
    zero,               ///< Zero.
    invalid_type,       ///< Requested BSON type is not supported.
    invalid_length_u32, ///< Length is too long (exceeds `UINT32_MAX`).
};

///
/// Support implicit conversion to `std::error_code`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_code make_error_code(value v) {
    return {static_cast<int>(v), v1::error::category::types::value()};
}

} // namespace types
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace std {

template <>
struct is_error_code_enum<bsoncxx::v1::error::types::value> : true_type {};

} // namespace std

namespace bsoncxx {
namespace v1 {
namespace types {

///
/// A union of BSON type values.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @note A "BSON type value" refers to the value of a BSON element without its key.
///
class value {
   private:
    class impl;

    alignas(BSONCXX_PRIVATE_MAX_ALIGN_T) std::array<unsigned char, 32> _storage;

   public:
    ///
    /// Destroy this object.
    ///
    BSONCXX_ABI_EXPORT_CDECL() ~value();

    ///
    /// Move construction.
    ///
    /// @par Postconditions:
    /// - `other` is equivalent to a default-initialized value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(value&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is equivalent to a default-initialized value.
    ///
    BSONCXX_ABI_EXPORT_CDECL(value&) operator=(value&& other) noexcept;

    ///
    /// Copy construction.
    ///
    /// The copied BSON type value is allocated (when necessary) using
    /// [`bson_value_copy`](https://mongoc.org/libbson/current/bson_value_copy.html).
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(value const& other);

    ///
    /// Copy assignment.
    ///
    /// The copied value is allocated (when necessary) using
    /// [`bson_value_copy`](https://mongoc.org/libbson/current/bson_value_copy.html).
    ///
    BSONCXX_ABI_EXPORT_CDECL(value&) operator=(value const& other);

    ///
    /// Initialize with @ref bsoncxx::v1::types::b_null.
    ///
    value() : value{v1::types::b_null{}} {}

    ///
    /// Initialize with a deep copy of the underlying BSON type value.
    ///
    /// The copied value is allocated (when necessary) using
    /// [`bson_malloc`](https://mongoc.org/libbson/current/bson_malloc.html).
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::types::value::invalid_type if `v.type_id()` is
    /// not a supported value (not defined by @ref BSONCXX_V1_TYPES_XMACRO).
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::types::value::invalid_length_u32 if the length
    /// of any underlying BSON type value component is not representable as an `std::uint32_t`.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() value(v1::types::view const& v);

#pragma push_macro("X")
#undef X
#define X(_name, _value) explicit BSONCXX_ABI_EXPORT_CDECL() value(v1::types::b_##_name v);

    ///
    /// Convert `v` as a deep copy.
    ///
    /// The copied value is allocated (when necessary) using
    /// [`bson_malloc`](https://mongoc.org/libbson/current/bson_malloc.html).
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::types::value::invalid_length_u32 if the length
    /// of any underlying BSON type value component is not representable as an `std::uint32_t` as required by the
    /// corresponding `bsoncxx::v1::types::b_<type>`.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_null.
    ///
    explicit value(std::nullptr_t) : value{v1::types::b_null{}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_string.
    ///
    explicit value(v1::stdx::string_view v) : value{v1::types::b_string{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_int32.
    ///
    explicit value(std::int32_t v) : value{v1::types::b_int32{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_int64.
    ///
    explicit value(std::int64_t v) : value{v1::types::b_int64{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_double.
    ///
    explicit value(double v) : value{v1::types::b_double{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_bool.
    ///
    explicit value(bool v) : value{v1::types::b_bool{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_oid.
    ///
    explicit value(v1::oid v) : value{v1::types::b_oid{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_decimal128.
    ///
    explicit value(v1::decimal128 v) : value{v1::types::b_decimal128{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_date.
    ///
    explicit value(std::chrono::milliseconds v) : value{v1::types::b_date{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_document.
    ///
    explicit value(v1::document::view v) : value{v1::types::b_document{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_array.
    ///
    explicit value(v1::array::view v) : value{v1::types::b_array{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_binary with @ref
    /// bsoncxx::v1::types::binary_subtype::k_binary.
    ///
    explicit value(std::vector<std::uint8_t> const& v)
        : value{v.data(), v.size(), v1::types::binary_subtype::k_binary} {}

    ///
    /// Initialize with `v` as a @ref bsoncxx::v1::types::b_binary with `subtype`.
    ///
    explicit value(std::vector<std::uint8_t> const& v, v1::types::binary_subtype subtype)
        : value{v.data(), v.size(), subtype} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_dbpointer.
    ///
    explicit value(v1::stdx::string_view c, v1::oid v) : value{v1::types::b_dbpointer{c, v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_codewscope.
    ///
    explicit value(v1::stdx::string_view code, v1::document::view scope)
        : value{v1::types::b_codewscope{code, scope}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_regex.
    ///
    explicit value(v1::stdx::string_view regex, v1::stdx::string_view options)
        : value{v1::types::b_regex{regex, options}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_string.
    ///
    explicit value(char const* v) : value{v1::stdx::string_view{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_string.
    ///
    explicit value(std::string const& v) : value{v1::stdx::string_view{v}} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_binary with @ref
    /// bsoncxx::v1::types::binary_subtype::k_binary.
    ///
    explicit value(std::uint8_t const* data, std::size_t size)
        : value{data, size, v1::types::binary_subtype::k_binary} {}

    ///
    /// Initialize as a @ref bsoncxx::v1::types::b_binary with `subtype`.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL()
    value(std::uint8_t const* data, std::size_t size, v1::types::binary_subtype const sub_type);

    ///
    /// Return the type of the underlying BSON type value.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::types::id) type_id() const;

    ///
    /// Return a view of the underlying BSON type value.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::types::view) view() const;

    ///
    /// Implicitly convert to `this->view()`.
    ///
    /* explicit(false) */ operator v1::types::view() const {
        return this->view();
    }

#pragma push_macro("X")
#undef X
#define X(_name, _value)                       \
    v1::types::b_##_name get_##_name() const { \
        return this->view().get_##_name();     \
    }

    ///
    /// Equivalent to `this->view().get_type()` where `get_type` is the correct name for the requested BSON type value
    /// (e.g. `this->view().get_double()` given `this->get_double()`).
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::types::view::type_mismatch if the underlying
    /// BSON type value does not match the requested type.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Equivalent to `lhs.view() == rhs.view()`.
    ///
    friend bool operator==(value const& lhs, value const& rhs) {
        return lhs.view() == rhs.view();
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(value const& lhs, value const& rhs) {
        return !(lhs == rhs);
    }

    class internal;
};

} // namespace types
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
///
/// @file
/// Provides @ref bsoncxx::v1::types::value.
///
/// @par Includes
/// - @ref bsoncxx/v1/types/view.hpp
///
