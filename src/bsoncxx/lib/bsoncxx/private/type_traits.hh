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

#include <bsoncxx/v1/detail/type_traits.hpp>

namespace bsoncxx {

template <typename LHS, typename RHS>
using is_assignable_from_expr = decltype(std::declval<LHS>() = std::declval<RHS>());

// Approximates the `assignable_from` concept in C++20.
template <typename LHS, typename RHS>
struct is_assignable_from : detail::conjunction<
                                std::is_lvalue_reference<LHS>,
                                std::is_same<detail::detected_t<is_assignable_from_expr, LHS, RHS>, LHS>> {};

// Equivalent to `is_assignable_from` but also requires noexceptness.
template <typename LHS, typename RHS>
struct is_nothrow_assignable_from
    : detail::bool_constant<
          is_assignable_from<LHS, RHS>::value && noexcept(std::declval<LHS>() = std::declval<RHS>())> {};

// Approximates the `movable` concept in C++20.
template <typename T>
struct is_movable : detail::conjunction<
                        std::is_object<T>,
                        std::is_move_constructible<T>,
                        is_assignable_from<T&, T>,
                        detail::is_swappable<T>> {};

// Equivalent to `is_moveable` but additionally requires noexceptness.
template <typename T>
struct is_nothrow_moveable : detail::conjunction<
                                 std::is_object<T>,
                                 std::is_nothrow_move_constructible<T>,
                                 is_nothrow_assignable_from<T&, T>,
                                 detail::is_nothrow_swappable<T>> {};

// Approximates the `copyable` concept in C++20.
template <typename T>
struct is_copyable : detail::conjunction<
                         std::is_copy_constructible<T>,
                         is_movable<T>,
                         is_assignable_from<T&, T&>,
                         is_assignable_from<T&, T const&>,
                         is_assignable_from<T&, T const>> {};

// Approximates the `semiregular` concept in C++20.
template <typename T>
struct is_semiregular : detail::conjunction<is_copyable<T>, std::is_default_constructible<T>> {};

// Approximates the `regular` concept in C++20.
template <typename T>
struct is_regular : detail::conjunction<is_semiregular<T>, detail::is_equality_comparable<T>> {};

// Equivalent to `is_trivial` without requiring trivial default constructibility.
template <typename T>
struct is_semitrivial
// https://gcc.gnu.org/onlinedocs/gcc-4.9.0/libstdc++/manual/manual/status.html
#if defined(__GNUC__) && (__GNUC__ < 5) && !defined(__clang__)
    : std::true_type
#else
    : detail::conjunction<
          std::is_trivially_destructible<T>,
          std::is_trivially_move_constructible<T>,
          std::is_trivially_move_assignable<T>,
          std::is_trivially_copy_constructible<T>,
          std::is_trivially_copy_assignable<T>>
#endif
{
};

} // namespace bsoncxx
