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
#include <utility>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace string {

///
/// Convert a `bsoncxx::v_noabi::stdx::string_view` to a `std::string`.
///
/// This function may be used in place of explicit conversion to `std::string`, which may not be
/// supported across all polyfill build configurations.
///
/// @par Example
/// ```cpp
/// std::string example(bsoncxx::v_noabi::stdx::string_view sv) {
///   // This may not be supported depending on the polyfill library.
///   // return std::string(sv);
///
///   // This is supported regardless of the polyfill library.
///   return bsoncxx::v_noabi::string::to_string(sv);
/// }
/// ```
///
template <class CharT,
          class Traits = std::char_traits<CharT>,
          class Allocator = std::allocator<CharT>>
std::basic_string<CharT, Traits, Allocator> to_string(
    v_noabi::stdx::basic_string_view<CharT, Traits> value, const Allocator& alloc = Allocator()) {
    return std::basic_string<CharT, Traits, Allocator>{value.data(), value.length(), alloc};
}

}  // namespace string
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace string {

using ::bsoncxx::v_noabi::string::to_string;

}  // namespace string
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::string::to_string.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace string {

/// @ref bsoncxx::v_noabi::string::to_string
template <class CharT, class Traits, class Allocator>
std::basic_string<CharT, Traits, Allocator> to_string(
    v_noabi::stdx::basic_string_view<CharT, Traits> value, const Allocator& alloc);

}  // namespace string
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
