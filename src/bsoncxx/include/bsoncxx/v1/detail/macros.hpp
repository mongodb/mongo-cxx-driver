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

// Traditional include guard is required to support v_noabi include-via-prelude.
#if !defined(BSONCXX_V1_DETAIL_MACROS_HPP)
#define BSONCXX_V1_DETAIL_MACROS_HPP

// Convert the given macro argument to a string literal, after macro expansion.
#define BSONCXX_PRIVATE_STRINGIFY(...) BSONCXX_PRIVATE_STRINGIFY_IMPL(__VA_ARGS__)
#define BSONCXX_PRIVATE_STRINGIFY_IMPL(...) #__VA_ARGS__

// Token-paste two macro arguments, after macro expansion
#define BSONCXX_PRIVATE_CONCAT(A, ...) BSONCXX_PRIVATE_CONCAT_IMPL(A, __VA_ARGS__)
#define BSONCXX_PRIVATE_CONCAT_IMPL(A, ...) A##__VA_ARGS__

// Expands to a _Pragma() preprocessor directive, after macro expansion
//
// The arguments an arbitrary "token soup", and should not be quoted like a regular
// _Pragma. This macro will stringify-them itself.
//
// Example:
//
//      BSONCXX_PRIVATE_PRAGMA(GCC diagnostic ignore "-Wconversion")
//
// will become:
//
//      _Pragma("GCC diagnostic ignore \"-Wconversion\"")
//
#define BSONCXX_PRIVATE_PRAGMA(...) BSONCXX_PRIVATE_PRAGMA_IMPL(__VA_ARGS__)
#ifdef _MSC_VER
// Old MSVC doesn't recognize C++11 _Pragma(), but it always recognized __pragma
#define BSONCXX_PRIVATE_PRAGMA_IMPL(...) __pragma(__VA_ARGS__)
#else
#define BSONCXX_PRIVATE_PRAGMA_IMPL(...) _Pragma(BSONCXX_PRIVATE_STRINGIFY(__VA_ARGS__))
#endif

// Use in a declaration position to force the appearence of a semicolon
// as the next token. Use this for statement-like or declaration-like macros to
// enforce that their call sites are followed by a semicolon
#define BSONCXX_PRIVATE_FORCE_SEMICOLON static_assert(true, "")

// Add a trailing noexcept, decltype-return, and return-body to a
// function definition. (Not compatible with lambda expressions.)
//
// Example:
//
//      template <typename T>
//      auto foo(T x, T y) BSONCXX_PRIVATE_RETURNS(x + y);
//
// Becomes:
//
//      template <typename T>
//      auto foo(T x, T y) noexcept(noexcept(x + y))
//          -> decltype(x + y)
//      { return x + y };
//
#define BSONCXX_PRIVATE_RETURNS(...)                         \
    noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) { \
        return __VA_ARGS__;                                  \
    }                                                        \
    BSONCXX_PRIVATE_FORCE_SEMICOLON

// @macro mongocxx_cxx14_constexpr
// Expands to `constexpr` if compiling as c++14 or greater, otherwise
// expands to `inline`.
//
// Use this on functions that can only be constexpr in C++14 or newer, including
// non-const member functions.
#if __cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L && _MSC_VER > 1910)
#define BSONCXX_PRIVATE_CONSTEXPR_CXX14 constexpr
#else
#define BSONCXX_PRIVATE_CONSTEXPR_CXX14 inline
#endif

#define BSONCXX_PRIVATE_IF_MSVC(...)
#define BSONCXX_PRIVATE_IF_GCC(...)
#define BSONCXX_PRIVATE_IF_CLANG(...)
#define BSONCXX_PRIVATE_IF_GNU_LIKE(...) \
    BSONCXX_PRIVATE_IF_GCC(__VA_ARGS__)  \
    BSONCXX_PRIVATE_IF_CLANG(__VA_ARGS__)

// clang-format off
#ifdef __GNUC__
    #ifdef __clang__
        #undef BSONCXX_PRIVATE_IF_CLANG
        #define BSONCXX_PRIVATE_IF_CLANG(...) __VA_ARGS__
    #else
        #undef BSONCXX_PRIVATE_IF_GCC
        #define BSONCXX_PRIVATE_IF_GCC(...) __VA_ARGS__
    #endif
#elif defined(_MSC_VER)
    #undef BSONCXX_PRIVATE_IF_MSVC
    #define BSONCXX_PRIVATE_IF_MSVC(...) __VA_ARGS__
#endif
// clang-format on

// Disable a warning for a particular compiler.
//
// The argument should be of the form:
//
// - Clang(<flag-string-literal>)
// - GCC(<flag-string-literal>)
// - GNU(<flag-string-literal>)
// - MSVC(<id-integer-literal>)
//
// The "GNU" form applies to both GCC and Clang
#define BSONCXX_PRIVATE_WARNINGS_DISABLE(Spec)                               \
    BSONCXX_PRIVATE_CONCAT(BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_, Spec) \
    BSONCXX_PRIVATE_FORCE_SEMICOLON

// Push the current compiler diagnostics settings state
#define BSONCXX_PRIVATE_WARNINGS_PUSH()                                      \
    BSONCXX_PRIVATE_IF_GNU_LIKE(BSONCXX_PRIVATE_PRAGMA(GCC diagnostic push)) \
    BSONCXX_PRIVATE_IF_MSVC(BSONCXX_PRIVATE_PRAGMA(warning(push)))           \
    BSONCXX_PRIVATE_FORCE_SEMICOLON

// Restore prior compiler diagnostics settings from before the most
// recent BSONCXX_PRIVATE_WARNINGS_PUSH()
#define BSONCXX_PRIVATE_WARNINGS_POP()                                      \
    BSONCXX_PRIVATE_IF_GNU_LIKE(BSONCXX_PRIVATE_PRAGMA(GCC diagnostic pop)) \
    BSONCXX_PRIVATE_IF_MSVC(BSONCXX_PRIVATE_PRAGMA(warning(pop)))           \
    BSONCXX_PRIVATE_FORCE_SEMICOLON

#define BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_GCC(...) \
    BSONCXX_PRIVATE_IF_GCC(BSONCXX_PRIVATE_PRAGMA(GCC diagnostic ignored __VA_ARGS__))

#define BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_Clang(...) \
    BSONCXX_PRIVATE_IF_CLANG(BSONCXX_PRIVATE_PRAGMA(GCC diagnostic ignored __VA_ARGS__))

#define BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_GNU(...)     \
    BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_GCC(__VA_ARGS__) \
    BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_Clang(__VA_ARGS__)

#define BSONCXX_PRIVATE_WARNINGS_DISABLE_IMPL_FOR_MSVC(...) \
    BSONCXX_PRIVATE_IF_MSVC(BSONCXX_PRIVATE_PRAGMA(warning(disable : __VA_ARGS__)))

#define BSONCXX_PRIVATE_FWD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

#define BSONCXX_PRIVATE_UNREACHABLE \
    if (1) {                        \
        std::abort();               \
    } else                          \
        ((void)0)

#endif // BSONCXX_V1_DETAIL_MACROS_HPP

///
/// @file
/// For internal use only!
///
/// @warning For internal use only!
///
