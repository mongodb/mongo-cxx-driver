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

#include <bsoncxx/document/element-fwd.hpp>

//

#include <bsoncxx/v1/element/view.hpp>

#include <cstddef>
#include <cstdint>

#include <bsoncxx/array/element-fwd.hpp>
#include <bsoncxx/document/element-fwd.hpp>
#include <bsoncxx/document/view-fwd.hpp>
#include <bsoncxx/types-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace document {

///
/// A variant view type that accesses values in serialized BSON documents.
///
/// Element functions as a variant type, where the kind of the element can be
/// interrogated by calling type(), the key can be extracted by calling key() and
/// a specific value can be extracted through get_X() accessors.
///
/// @see
/// - @ref bsoncxx::v_noabi::array::element
///
class element {
   private:
    v1::element::view _view;

   public:
    ///
    /// Construct an invalid element.
    ///
    /// This is useful when mapping the end iterator of a document or array
    /// view.
    ///
    element() = default;

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ element(v1::element::view const& v) : _view{v} {}

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    explicit operator v1::element::view() const {
        return _view;
    }

    ///
    /// Conversion operator to bool which is true for valid elements
    /// and false for invalid elements.
    ///
    explicit operator bool() const {
        return _view.operator bool();
    }

    ///
    /// Getter for the raw bson bytes the element points to.
    ///
    /// @return a pointer to the raw bson bytes.
    ///
    std::uint8_t const* raw() const {
        return _view.raw();
    }

    ///
    /// Getter for length of the raw bson bytes the element points to.
    ///
    /// @return a pointer to the length of the raw bson bytes.
    ///
    std::uint32_t length() const {
        return _view.length();
    }

    ///
    /// Getter for the offset into the raw bson bytes the element points to.
    ///
    /// @return the offset into the raw bson bytes.
    ///
    std::uint32_t offset() const {
        return _view.offset();
    }

    ///
    /// Getter for the element's key length.
    ///
    /// @return the element's key length.
    ///
    std::uint32_t keylen() const {
        return _view.keylen();
    }

    ///
    /// Getter for the type of the element.
    ///
    /// @return the element's type.
    ///
    /// @throws bsoncxx::v_noabi::exception if this element is invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v_noabi::type) type() const;

    ///
    /// Getter for the element's key.
    ///
    /// @return the element's key.
    ///
    /// @throws bsoncxx::v_noabi::exception if this element is invalid.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v1::stdx::string_view) key() const;

#pragma push_macro("X")
#undef X
#define X(_name, _value) BSONCXX_ABI_EXPORT_CDECL(v_noabi::types::b_##_name) get_##_name() const;

    ///
    /// Return the BSON type value of this element.
    ///
    /// @throws bsoncxx::v_noabi::exception if this element is not the requested type.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Getter for a types::bson_value::view variant wrapper of the value portion of the
    /// element.
    ///
    /// @return the element's value.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v_noabi::types::bson_value::view) get_value() const;

    ///
    /// Getter for a types::bson_value::value variant wrapper of the value portion of
    /// the element. The returned object will make a copy of the buffer from this object.
    ///
    /// @return an owning version of the element's value.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v_noabi::types::bson_value::value) get_owning_value() const;

    ///
    /// If this element is a document, finds the first element of the document
    /// with the provided key. If there is no such element, an invalid
    /// document::element will be returned.  The runtime of operator[] is
    /// linear in the length of the document.
    ///
    /// If this element is not a document, an invalid document::element will
    /// be returned.
    ///
    /// @param key
    ///   The key to search for.
    ///
    /// @return The matching element, if found, or an invalid element.
    ///
    BSONCXX_ABI_EXPORT_CDECL(element) operator[](v1::stdx::string_view key) const;

    ///
    /// If this element is an array, indexes into this BSON array. If the
    /// index is out-of-bounds, an invalid array::element will be returned. As
    /// BSON represents arrays as documents, the runtime of operator[] is
    /// linear in the length of the array.
    ///
    /// If this element is not an array, an invalid array::element will
    /// be returned.
    ///
    /// @param i
    ///   The index of the element.
    ///
    /// @return The element if it exists, or an invalid element.
    ///
    BSONCXX_ABI_EXPORT_CDECL(v_noabi::array::element) operator[](std::uint32_t i) const;
};

///
/// Convenience methods to compare for equality against a bson_value.
///
/// Compares equal if the element contains a matching bson_value. Otherwise, compares unequal.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::document::element
BSONCXX_ABI_EXPORT_CDECL(bool) operator==(element const& lhs, v_noabi::types::bson_value::view const& rhs);

/// @relatesalso bsoncxx::v_noabi::document::element
inline bool operator==(v_noabi::types::bson_value::view const& lhs, element const& rhs) {
    return rhs == lhs;
}

/// @relatesalso bsoncxx::v_noabi::document::element
inline bool operator!=(element const& lhs, v_noabi::types::bson_value::view const& rhs) {
    return !(lhs == rhs);
}

/// @relatesalso bsoncxx::v_noabi::document::element
inline bool operator!=(v_noabi::types::bson_value::view const& lhs, element const& rhs) {
    return !(lhs == rhs);
}

/// @}
///

} // namespace document
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

// Ambiguous whether `v1::element::view` should be converted to `v_noabi::array::element` or
// `v_noabi::document::element.` Require users to explicitly cast to the expected type instead.
//
// v_noabi::document::element from_v1(v1::element::view const& v);

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::element::view to_v1(v_noabi::document::element const& v) {
    return v1::element::view{v};
}

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace document {

using v_noabi::document::operator==;
using v_noabi::document::operator!=;

} // namespace document
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::document::element.
///
/// @par Includes
/// - @ref bsoncxx/v1/element/view.hpp
///
