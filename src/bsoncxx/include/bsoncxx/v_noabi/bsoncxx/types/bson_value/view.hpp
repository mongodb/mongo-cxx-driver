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

#include <bsoncxx/types/bson_value/view-fwd.hpp>

//

#include <bsoncxx/v1/types/view.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <bsoncxx/document/element-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>

#include <bsoncxx/stdx/type_traits.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace detail {

template <typename T>
using is_bson_view_compatible = detail::conjunction<
    std::is_constructible<v_noabi::types::bson_value::view, T>,
    detail::negation<detail::disjunction<
        detail::is_alike<T, v_noabi::types::bson_value::view>,
        detail::is_alike<T, v_noabi::types::bson_value::value>>>>;

} // namespace detail
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

///
/// A non-owning variant that can contain any BSON type.
///
/// @warning
///   Calling the wrong get_<type> method will cause an exception
///   to be thrown.
///
class view {
   private:
    v1::types::view _view;

   public:
    ///
    /// Default constructs a bson_value::view. The resulting view will be initialized
    /// to point at a bson_value of type k_null.
    ///
    view() noexcept : _view{} {}

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ view(v1::types::view const& v) : _view{v} {}

#pragma push_macro("X")
#undef X
#define X(_name, _value) \
    explicit view(v_noabi::types::b_##_name v) noexcept : _view{to_v1(v)} {}

    ///
    /// Construct a bson_value::view from the provided BSON type.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    explicit operator v1::types::view() const {
        return _view;
    }

    ///
    /// Returns the type of the underlying BSON value stored in this object.
    ///
    v_noabi::type type() const {
        return from_v1(_view.type_id());
    }

#pragma push_macro("X")
#undef X
#define X(_name, _value) BSONCXX_ABI_EXPORT_CDECL(v_noabi::types::b_##_name) get_##_name() const;

    ///
    /// Return the underlying BSON type value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// @relates bsoncxx::v_noabi::types::bson_value::view
    ///
    /// Compare two bson_value::views for equality
    ///
    /// @{
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator==(view const& lhs, view const& rhs);

    friend bool operator!=(view const& lhs, view const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

///
/// Compares a view with a type representable as a view.
///
/// @par Constraints
/// - @ref bsoncxx::v_noabi::types::bson_value::view is constructible from `T`.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, detail::is_bson_view_compatible<T>> operator==(view const& lhs, T&& rhs) {
    return lhs == view{std::forward<T>(rhs)};
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, detail::is_bson_view_compatible<T>> operator==(T&& lhs, view const& rhs) {
    return view{std::forward<T>(lhs)} == rhs;
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, detail::is_bson_view_compatible<T>> operator!=(view const& lhs, T&& rhs) {
    return lhs != view{std::forward<T>(rhs)};
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::view
template <typename T>
detail::requires_t<bool, detail::is_bson_view_compatible<T>> operator!=(T&& lhs, view const& rhs) {
    return view{std::forward<T>(lhs)} != rhs;
}

/// @}
///

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

///
/// Convert to the @ref bsoncxx::v_noabi equivalent of `v`.
///
inline v_noabi::types::bson_value::view from_v1(v1::types::view const& v) {
    return {v};
}

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::types::view to_v1(v_noabi::types::bson_value::view const& v) {
    return v1::types::view{v};
}

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace types {
namespace bson_value {

using v_noabi::types::bson_value::operator==;
using v_noabi::types::bson_value::operator!=;

} // namespace bson_value
} // namespace types
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::types::bson_value::view.
///
/// @par Includes
/// - @ref bsoncxx/v1/types/view.hpp
///
