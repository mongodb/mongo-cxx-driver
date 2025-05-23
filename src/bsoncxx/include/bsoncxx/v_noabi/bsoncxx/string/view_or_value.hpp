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

#include <string>

#include <bsoncxx/string/view_or_value-fwd.hpp>

#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/view_or_value.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace string {

///
/// A view-or-value variant type for strings.
///
/// @par Derived From
/// - @ref bsoncxx::v_noabi::view_or_value<stdx::string_view, std::string>
///
/// This class adds several string-specific methods to the bsoncxx::v_noabi::view_or_value template:
/// - a constructor overload for char const*
/// - a constructor overload for std::string by l-value reference
/// - a safe c_str() operation to return null-terminated c-style strings.
///
class view_or_value : public bsoncxx::v_noabi::view_or_value<stdx::string_view, std::string> {
   public:
    ///
    /// Forward all bsoncxx::v_noabi::view_or_value constructors.
    ///
    using bsoncxx::v_noabi::view_or_value<stdx::string_view, std::string>::view_or_value;

    ///
    /// Default constructor, equivalent to using an empty string.
    ///
    BSONCXX_ABI_EXPORT_CDECL() view_or_value() = default;

    ///
    /// Construct a string::view_or_value using a null-terminated const char *.
    /// The resulting view_or_value will keep a string_view of 'str', so it is
    /// important that the passed-in string outlive this object.
    ///
    /// @param str A null-terminated string
    ///
    view_or_value(char const* str)
        : bsoncxx::v_noabi::view_or_value<stdx::string_view, std::string>(stdx::string_view(str)) {}

    ///
    /// Allow construction with an l-value reference to a std::string. The resulting
    /// view_or_value will keep a string_view of 'str', so it is important that the
    /// passed-in string outlive this object.
    ///
    /// Construction calls passing a std::string by r-value reference will use the
    /// constructor defined in the parent view_or_value class.
    ///
    /// @param str A std::string l-value reference.
    ///
    view_or_value(std::string const& str)
        : bsoncxx::v_noabi::view_or_value<stdx::string_view, std::string>(stdx::string_view(str)) {}

    ///
    /// Return a string_view_or_value that is guaranteed to hold a null-terminated
    /// string. The lifetime of the returned object must be a subset of this object's
    /// lifetime, because the new view_or_value might hold a view into this one.
    ///
    /// It is recommended that this method be used before calling .data() on a
    /// view_or_value, as that method may return a non-null-terminated string.
    ///
    /// @return A new view_or_value object.
    ///
    BSONCXX_ABI_EXPORT_CDECL(view_or_value) terminated() const;

    ///
    /// Call data() on this view_or_value's string_view. This method is not
    /// guaranteed to return a null-terminated string unless it is used in
    /// combination with terminated().
    ///
    /// @return A char const* of this string.
    ///
    BSONCXX_ABI_EXPORT_CDECL(char const*) data() const;
};

///
/// Comparison operators for comparing string::view_or_value directly with `char const*`.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::string::view_or_value
inline bool operator==(view_or_value const& lhs, char const* rhs) {
    return lhs.view() == stdx::string_view(rhs);
}

/// @relatesalso bsoncxx::v_noabi::string::view_or_value
inline bool operator!=(view_or_value const& lhs, char const* rhs) {
    return !(lhs == rhs);
}

/// @relatesalso bsoncxx::v_noabi::string::view_or_value
inline bool operator==(char const* lhs, view_or_value const& rhs) {
    return rhs == lhs;
}

/// @relatesalso bsoncxx::v_noabi::string::view_or_value
inline bool operator!=(char const* lhs, view_or_value const& rhs) {
    return !(rhs == lhs);
}

/// @}
///

} // namespace string
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace string {

using ::bsoncxx::v_noabi::string::operator==;
using ::bsoncxx::v_noabi::string::operator!=;

} // namespace string
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::string::view_or_value.
///
