// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <string>

#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/view_or_value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace string {

///
/// Class representing a view-or-value variant type for strings.
///
/// This class adds several string-specific methods to the bsoncxx::view_or_value template:
/// - a constructor overload for const char*
/// - a constructor overload for std::string by l-value reference
/// - a safe c_str() operation to return null-terminated c-style strings.
///
class BSONCXX_API view_or_value : public bsoncxx::view_or_value<stdx::string_view, std::string> {
   public:
    ///
    /// Forward all bsoncxx::view_or_value constructors.
    ///
    using bsoncxx::view_or_value<stdx::string_view, std::string>::view_or_value;

    ///
    /// Construct a string::view_or_value using a null-terminated const char *.
    /// The resulting view_or_value will keep a string_view of 'str', so it is
    /// important that the passed-in string outlive this object.
    ///
    /// @param str A null-terminated string
    ///
    BSONCXX_INLINE view_or_value(const char* str)
        : bsoncxx::view_or_value<stdx::string_view, std::string>(stdx::string_view(str)) {
    }

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
    BSONCXX_INLINE view_or_value(const std::string& str)
        : bsoncxx::view_or_value<stdx::string_view, std::string>(stdx::string_view(str)) {
    }

    ///
    /// Return a null-terminated string from this string::view_or_value.
    ///
    /// @note if we are non-owning, we cannot assume our string_view is null-terminated.
    /// In this case we must make a new, owning view_or_value with a null-terminated
    /// copy of our view.
    ///
    /// @return A pointer to the original string, or a null-terminated copy.
    ///
    const char* c_str();
};

///
/// Compare string::view_or_value directly with const char *.
///

BSONCXX_INLINE bool operator==(const view_or_value& lhs, const char* rhs) {
    return lhs.view() == stdx::string_view(rhs);
}

BSONCXX_INLINE bool operator!=(const view_or_value& lhs, const char* rhs) {
    return !(lhs == rhs);
}

BSONCXX_INLINE bool operator==(const char* lhs, const view_or_value& rhs) {
    return rhs == lhs;
}

BSONCXX_INLINE bool operator!=(const char* lhs, const view_or_value& rhs) {
    return !(rhs == lhs);
}

}  // namespace string
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
