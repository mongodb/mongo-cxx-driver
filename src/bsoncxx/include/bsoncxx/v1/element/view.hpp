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

#include <bsoncxx/v1/element/view-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/id-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <system_error>
#include <type_traits>

namespace bsoncxx {
namespace v1 {
namespace error {
namespace category {

///
/// Declares error categories for error codes declared in @ref bsoncxx::v1::error::element.
///
namespace element {

///
/// The error category for @ref bsoncxx::v1::error::element::view.
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) view();

} // namespace element
} // namespace category
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace bsoncxx {
namespace v1 {
namespace error {

///
/// Declares error codes returned by @ref bsoncxx::v1::element interfaces.
///
namespace element {

///
/// Errors codes which may be returned by @ref bsoncxx::v1::element::view.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class view {
    zero,         ///< Zero.
    invalid_view, ///< View is invalid.
    invalid_data, ///< Data is invalid.
};

///
/// Support implicit conversion to `std::error_code`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_code make_error_code(view v) {
    return {static_cast<int>(v), v1::error::category::element::view()};
}

} // namespace element
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace std {

template <>
struct is_error_code_enum<bsoncxx::v1::error::element::view> : true_type {};

} // namespace std

namespace bsoncxx {
namespace v1 {
namespace element {

///
/// A non-owning, read-only BSON element.
///
/// An "invalid" element, as indicated by @ref operator bool() const, does not represent a BSON element and does not
/// have an underlying key or value. Attempting to query the key or value of an invalid element will throw an exception.
/// However, subscripting will not throw an exception when the requested field does not exist; instead it will return an
/// invalid element to facilitate chaining.
///
/// ```cpp
/// a = doc["a"]; // If "a" exists, `a` is valid. Otherwise, `a` is invalid.
/// b = a["b"];   // If "a" is a document and "a.b" exists, `b` is valid. Otherwise, `b` is invalid.
/// c = b[3];     // If "a.b" is an array and "a.b[3]" exists, `c` is valid. Otherwise, `c` is invalid.
///
/// a.key();       // Throws an exception if invalid.
/// b.type_id();   // Throws an exception if invalid.
/// c.get_int32(); // Throws an exception if invalid.
/// ```
///
/// Do not chain subscripts (e.g. `doc["a"]["b"]`) or immediately query element properties (e.g. `doc["x"].key()`)
/// if you need to identify which field may be missing.
///
/// ```cpp
/// // Unclear whether "x" or "x.y" was missing when an exception is thrown.
/// try { doc["x"]["y"].type_id(); } catch (...) {}
///
/// // Check validity prior to further access to identify the missing field.
/// if (x = doc["x"]) {
///     if (y = x["y"]) {} // Field "x.y" is present.
///     else            {} // Field "x.y" is missing.
/// } else              {} // Field "x" is missing.
///
/// // A less efficient but more concise approach.
/// if (doc["x"]["y"]) {} // Field "x.y" is present.
/// else if (doc["x"]) {} // Field "x.y" is missing.
/// else               {} // Field "x" is missing.
/// ```
///
/// The BSON bytes being represented is only validated as minimally required to satisfy a requested operation.
/// When an operation is not satisfiable due to invalid data, the operation will throw an @ref bsoncxx::v1::exception
/// with @ref bsoncxx::v1::error::document::view::invalid_data.
///
/// @attention This feature is experimental! It is not ready for use!
///
class view {
   private:
    class impl;

    alignas(BSONCXX_PRIVATE_MAX_ALIGN_T) unsigned char _storage[32];

   public:
    ///
    /// Destroy this object.
    ///
    BSONCXX_ABI_EXPORT_CDECL() ~view();

    // Handled by the nothrow copy constructor.
    // view(view&& other) noexcept;

    // Handled by the nothrow copy assignment operator.
    // view& operator=(view&& other) noexcept;

    ///
    /// Copy (or move) construction.
    ///
    BSONCXX_ABI_EXPORT_CDECL() view(view const& other) noexcept;

    ///
    /// Copy (or move) assignment.
    ///
    BSONCXX_ABI_EXPORT_CDECL(view&) operator=(view const& other) noexcept;

    ///
    /// Initialize as an invalid element without any associated BSON bytes.
    ///
    BSONCXX_ABI_EXPORT_CDECL() view();

    ///
    /// Return true when this is a valid element.
    ///
    /// @note This does not validate the BSON bytes being represented.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Return the "raw" component of the underlying BSON bytes.
    ///
    /// The value is unspecified when this element is invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::uint8_t const*) raw() const;

    ///
    /// Return the "length" component of the underlying BSON bytes.
    ///
    /// The value is unspecified when this element is invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::uint32_t) length() const;

    ///
    /// Return the "offset" component of the underlying BSON bytes.
    ///
    /// The value is unspecified when this element is invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::uint32_t) offset() const;

    ///
    /// Return the "keylen" component of the underlying BSON bytes.
    ///
    /// The value is unspecified when this element is invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::uint32_t) keylen() const;

    ///
    /// Return the type.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::element::view::invalid_view if this element is
    /// invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::types::id) type_id() const;

    ///
    /// Return the key.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::element::view::invalid_view if this element is
    /// invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::stdx::string_view) key() const;

#pragma push_macro("X")
#undef X
#define X(_name, _value) BSONCXX_ABI_EXPORT_CDECL(v1::types::b_##_name) get_##_name() const;

    ///
    /// Return the underlying BSON type value.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::element::view::invalid_view if this element is
    /// invalid.
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::types::view::type_mismatch if the BSON type
    /// value does not match the requested type.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Return a view of the underlying BSON type value.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::element::view::invalid_view if this element is
    /// invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::types::view) type_view() const;

    ///
    /// Return a deep copy of the underlying BSON type value.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::element::view::invalid_view if this element is
    /// invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::types::value) type_value() const;

    ///
    /// Return an element representing the requested field.
    ///
    /// @returns An invalid element if the requested field does not exist.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::element::view::invalid_data if this operation
    /// failed due to invalid BSON bytes.
    ///
    /// @{
    BSONCXX_ABI_EXPORT_CDECL(v1::element::view) operator[](v1::stdx::string_view key) const;
    BSONCXX_ABI_EXPORT_CDECL(v1::element::view) operator[](std::uint32_t idx) const;
    /// @}
    ///

    class internal;

   private:
    explicit view(impl i);
};

} // namespace element
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::element::view.
///
