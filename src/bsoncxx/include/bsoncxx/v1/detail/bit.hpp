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

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/config.hpp>

#if defined(__has_include)
#if __has_include(<bit>) && (!defined(_MSVC_LANG) || _MSVC_LANG >= 202002L)
// Prioritize using std::endian from C++20.
#include <bit>
#endif // __has_include(<bit>) && (!defined(_MSVC_LANG) || _MSVC_LANG >= 202002L)
#elif defined(_WIN32)
// Forward-compatibility with STL: https://github.com/microsoft/STL/blob/vs-2019-16.5/stl/inc/bit#L26
#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
// Predefined compiler macros: https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#elif defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L
// Fallback to POSIX Issue 8: https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/endian.h.html
#include <endian.h>
#elif defined(_BSD_SOURCE) || defined(_DEFAULT_SOURCE)
// Fallback to OpenBSD/FreeBSD/etc.: https://www.man7.org/linux/man-pages/man3/endian.3.html
// _BSD_SOURCE for compatibility with glibc 2.19 and older.
#include <sys/endian.h>
#endif

namespace bsoncxx {
namespace detail {

// Equivalent to `std::endian` in C++20.
enum class endian {
#if defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L
    little = static_cast<int>(std::endian::little),
    big = static_cast<int>(std::endian::big),
    native = static_cast<int>(std::endian::native)
#elif defined(_WIN32)
    // STL: https://github.com/microsoft/STL/blob/vs-2019-16.5/stl/inc/bit#L26
    little = 0,
    big = 1,
    native = little,
#elif defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__)
    // Predefined compiler macros for GCC and Clang.
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__,
#elif defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN) && defined(BYTE_ORDER)
    // Fallback to POSIX and OpenBSD/FreeBSD/etc.
    little = LITTLE_ENDIAN,
    big = BIG_ENDIAN,
    native = BYTE_ORDER,
#else
#error "Could not determine the byte order for the target compiler architecture"
#endif
};

} // namespace detail
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// For internal use only!
///
/// @warning For internal use only!
///
