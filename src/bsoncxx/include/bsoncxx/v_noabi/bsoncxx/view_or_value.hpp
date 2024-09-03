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

#include <type_traits>

#include <bsoncxx/view_or_value-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

///
/// Class representing a view-or-value variant type.
///
template <typename View, typename Value>
class view_or_value {
   public:
    using view_type = View;
    using value_type = Value;

    ///
    /// Class View must be constructible from an instance of class Value.
    ///
    static_assert(std::is_constructible<View, Value>::value,
                  "View type must be constructible from a Value");

    ///
    /// Class View must be default constructible.
    ///
    static_assert(std::is_default_constructible<View>::value,
                  "View type must be default constructible");

    ///
    /// Destroys a view_or_value.
    ///
    ~view_or_value() = default;

    ///
    /// Default-constructs a view_or_value. This is equivalent to constructing a
    /// view_or_value with a default-constructed View.
    ///
    view_or_value() = default;

    ///
    /// Construct a view_or_value from a View. When constructed with a View,
    /// this object is non-owning. The Value underneath the given View must outlive this object.
    ///
    /// @param view
    ///   A non-owning View.
    ///
    view_or_value(View view) : _view{view} {}

    ///
    /// Constructs a view_or_value from a Value type. This object owns the passed-in Value.
    ///
    /// @param value
    ///   A Value type.
    ///
    view_or_value(Value&& value) : _value(std::move(value)), _view(*_value) {}

    ///
    /// Construct a view_or_value from a copied view_or_value.
    ///
    view_or_value(const view_or_value& other)
        : _value(other._value), _view(_value ? *_value : other._view) {}

    ///
    /// Assign to this view_or_value from a copied view_or_value.
    ///
    view_or_value& operator=(const view_or_value& other) {
        _value = other._value;
        _view = _value ? *_value : other._view;
        return *this;
    }

    ///
    /// Construct a view_or_value from a moved-in view_or_value.
    ///

    /// TODO CXX-800: Create a noexcept expression to check the conditions that must be met.
    view_or_value(view_or_value&& other) noexcept
        : _value{std::move(other._value)}, _view(_value ? *_value : std::move(other._view)) {
        other._view = View();
        other._value = stdx::nullopt;
    }

    ///
    /// Assign to this view_or_value from a moved-in view_or_value.
    ///
    /// TODO CXX-800: Create a noexcept expression to check the conditions that must be met.
    view_or_value& operator=(view_or_value&& other) noexcept {
        _value = std::move(other._value);
        _view = _value ? *_value : std::move(other._view);
        other._view = View();
        other._value = stdx::nullopt;
        return *this;
    }

    ///
    /// Return whether or not this view_or_value owns an underlying Value.
    ///
    /// @return bool Whether we are owning.
    ///
    bool is_owning() const noexcept {
        return static_cast<bool>(_value);
    }

    ///
    /// This type may be used as a View.
    ///
    /// @return a View into this view_or_value.
    ///
    operator View() const {
        return _view;
    }

    ///
    /// Get a View for the type.
    ///
    /// @return a View into this view_or_value.
    ///
    const View& view() const {
        return _view;
    }

   private:
    stdx::optional<Value> _value;
    View _view;
};

///
/// Compare view_or_value objects for (in)equality.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator==(const view_or_value<View, Value>& lhs, const view_or_value<View, Value>& rhs) {
    return lhs.view() == rhs.view();
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator!=(const view_or_value<View, Value>& lhs, const view_or_value<View, Value>& rhs) {
    return !(lhs == rhs);
}

/// @}
///

///
/// Mixed (in)equality operators for view_or_value against View and Value types
///
/// @{

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator==(const view_or_value<View, Value>& lhs, View rhs) {
    return lhs.view() == rhs;
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator==(View lhs, const view_or_value<View, Value>& rhs) {
    return rhs == lhs;
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator!=(const view_or_value<View, Value>& lhs, View rhs) {
    return !(lhs == rhs);
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator!=(View lhs, const view_or_value<View, Value>& rhs) {
    return !(rhs == lhs);
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator==(const view_or_value<View, Value>& lhs, const Value& rhs) {
    return lhs.view() == View(rhs);
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator==(const Value& lhs, const view_or_value<View, Value>& rhs) {
    return rhs == lhs;
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator!=(const view_or_value<View, Value>& lhs, const Value& rhs) {
    return !(lhs == rhs);
}

/// @relatesalso bsoncxx::v_noabi::view_or_value
template <typename View, typename Value>
bool operator!=(const Value& lhs, const view_or_value<View, Value>& rhs) {
    return !(rhs == lhs);
}

/// @}
///

}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {

using ::bsoncxx::v_noabi::operator==;
using ::bsoncxx::v_noabi::operator!=;

}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::view_or_value.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {

/// @ref bsoncxx::v_noabi::operator==(const v_noabi::view_or_value<View, Value>& lhs, const v_noabi::view_or_value<View, Value>& rhs)
template <typename View, typename Value>
bool operator==(const v_noabi::view_or_value<View, Value>& lhs,
                const v_noabi::view_or_value<View, Value>& rhs);

/// @ref bsoncxx::v_noabi::operator!=(const v_noabi::view_or_value<View, Value>& lhs, const v_noabi::view_or_value<View, Value>& rhs)
template <typename View, typename Value>
bool operator!=(const v_noabi::view_or_value<View, Value>& lhs,
                const v_noabi::view_or_value<View, Value>& rhs);

/// @ref bsoncxx::v_noabi::operator==(const v_noabi::view_or_value<View, Value>& lhs, View rhs)
template <typename View, typename Value>
bool operator==(const v_noabi::view_or_value<View, Value>& lhs, View rhs);

/// @ref bsoncxx::v_noabi::operator==(View lhs, const v_noabi::view_or_value<View, Value>& rhs)
template <typename View, typename Value>
bool operator==(View lhs, const v_noabi::view_or_value<View, Value>& rhs);

/// @ref bsoncxx::v_noabi::operator!=(const v_noabi::view_or_value<View, Value>& lhs, View rhs)
template <typename View, typename Value>
bool operator!=(const v_noabi::view_or_value<View, Value>& lhs, View rhs);

/// @ref bsoncxx::v_noabi::operator!=(View lhs, const v_noabi::view_or_value<View, Value>& rhs)
template <typename View, typename Value>
bool operator!=(View lhs, const v_noabi::view_or_value<View, Value>& rhs);

/// @ref bsoncxx::v_noabi::operator==(const v_noabi::view_or_value<View, Value>& lhs, const Value& rhs)
template <typename View, typename Value>
bool operator==(const v_noabi::view_or_value<View, Value>& lhs, const Value& rhs);

/// @ref bsoncxx::v_noabi::operator==(const Value& lhs, const v_noabi::view_or_value<View, Value>& rhs)
template <typename View, typename Value>
bool operator==(const Value& lhs, const v_noabi::view_or_value<View, Value>& rhs);

/// @ref bsoncxx::v_noabi::operator!=(const v_noabi::view_or_value<View, Value>& lhs, const Value& rhs)
template <typename View, typename Value>
bool operator!=(const v_noabi::view_or_value<View, Value>& lhs, const Value& rhs);

}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
