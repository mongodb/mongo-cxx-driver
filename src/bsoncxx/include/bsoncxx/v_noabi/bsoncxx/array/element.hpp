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

#include <bsoncxx/array/element-fwd.hpp>

//

#include <bsoncxx/v1/element/view.hpp>

#include <cstddef>
#include <cstdint>

#include <bsoncxx/array/view-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>

#include <bsoncxx/document/element.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace array {

///
/// A variant view type that accesses values in serialized BSON arrays.
///
/// Element functions as a variant type, where the kind of the element can be
/// interrogated by calling type() and a specific value can be extracted through
/// get_X() accessors.
///
class element : private v_noabi::document::element {
   public:
    element() : v_noabi::document::element() {}

    /* explicit(false) */ element(v1::element::view const& v) : v_noabi::document::element{v} {}

    using v_noabi::document::element::operator v1::element::view;

    using v_noabi::document::element::operator bool;

    using v_noabi::document::element::raw;

    using v_noabi::document::element::length;

    using v_noabi::document::element::offset;

    using v_noabi::document::element::keylen;

    using v_noabi::document::element::type;

    using v_noabi::document::element::key;

#pragma push_macro("X")
#undef X
#define X(_name, _value) using v_noabi::document::element::get_##_name;
    BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

    using v_noabi::document::element::get_value;

    using v_noabi::document::element::get_owning_value;

    using v_noabi::document::element::type_view;

    using v_noabi::document::element::type_value;

    using v_noabi::document::element::operator[];

    friend bool operator==(element const& lhs, v_noabi::types::bson_value::view const& rhs);
};

///
/// Convenience methods to compare for equality against a bson_value.
///
/// Compares equal if this element contains a matching bson_value. Otherwise, compares unequal.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::array::element
inline bool operator==(element const& lhs, v_noabi::types::bson_value::view const& rhs) {
    return static_cast<v_noabi::document::element const&>(lhs) == rhs;
}

/// @relatesalso bsoncxx::v_noabi::array::element
inline bool operator==(v_noabi::types::bson_value::view const& lhs, element const& rhs) {
    return rhs == lhs;
}

/// @relatesalso bsoncxx::v_noabi::array::element
inline bool operator!=(element const& lhs, v_noabi::types::bson_value::view const& rhs) {
    return !(lhs == rhs);
}

/// @relatesalso bsoncxx::v_noabi::array::element
inline bool operator!=(v_noabi::types::bson_value::view const& lhs, element const& rhs) {
    return !(lhs == rhs);
}

/// @}
///

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

// Ambiguous whether `v1::element::view` should be converted to `v_noabi::array::element` or
// `v_noabi::document::element.` Require users to explicitly cast to the expected type instead.
//
// v_noabi::array::element from_v1(v1::element::view const& v);

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::element::view to_v1(v_noabi::array::element const& v) {
    return v1::element::view{v};
}

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace array {

using v_noabi::array::operator==;
using v_noabi::array::operator!=;

} // namespace array
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::array::element.
///
/// @par Includes
/// - @ref bsoncxx/document/element.hpp
/// - @ref bsoncxx/v1/element/view.hpp
///
