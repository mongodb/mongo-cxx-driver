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

#include <cstddef>
#include <functional>
#include <type_traits>

#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace detail {

template <typename L, typename R>
auto is_equality_comparable_f(...) -> std::false_type;

BSONCXX_PUSH_WARNINGS();
BSONCXX_DISABLE_WARNING(GNU("-Wfloat-equal"));
template <typename L, typename R>
auto is_equality_comparable_f(int, bool b = false)
    -> true_t<decltype((std::declval<const L&>() == std::declval<const R&>()) ? 0 : 0,  //
                       (std::declval<const R&>() == std::declval<const L&>()) ? 0 : 0,
                       (std::declval<const L&>() != std::declval<const R&>()) ? 0 : 0,
                       (std::declval<const R&>() != std::declval<const L&>()) ? 0 : 0)>;
BSONCXX_POP_WARNINGS();

// Detect whether two types are equality-comparable.
//
// Requires L == R, L != R, R == L, and R != L.
template <typename L, typename R = L>
struct is_equality_comparable : decltype(is_equality_comparable_f<L, R>(0)) {};

// Callable object and tag type for equality comparison.
struct equal_to {
    template <typename L, typename R>
    constexpr requires_t<bool, is_equality_comparable<L, R>>  //
    operator()(L&& l, R&& r) const noexcept(noexcept(l == r)) {
        return l == r;
    }
};

// Derive from this class to define ADL-only operator== and operator!= on the basis of
// an ADL-only tag_invoke(equal_to, l, r).
class equality_operators {
    template <typename L, typename R>
    constexpr static auto impl(rank<1>, L& l, R& r) BSONCXX_RETURNS(tag_invoke(equal_to{}, l, r));

    template <typename L, typename R>
    constexpr static auto impl(rank<0>, L& l, R& r) BSONCXX_RETURNS(tag_invoke(equal_to{}, r, l));

    // @cond DOXYGEN_DISABLE "Found ';' while parsing initializer list!"
    template <typename Left, typename Other>
    constexpr friend auto operator==(const Left& self, const Other& other)
        BSONCXX_RETURNS(equality_operators::impl(rank<1>{}, self, other));
    // @endcond

    // @cond DOXYGEN_DISABLE "Found ';' while parsing initializer list!"
    template <typename Left, typename Other>
    constexpr friend auto operator!=(const Left& self, const Other& other)
        BSONCXX_RETURNS(!equality_operators::impl(rank<1>{}, self, other));
    // @endcond
};

// Very basic impl of C++20 std::strong_ordering.
//
// We don't need other weaker orderings yet, so this is all that we have.
class strong_ordering {
    signed char _c;
    struct _construct {};

    constexpr strong_ordering(_construct, signed char c) noexcept : _c(c) {}

   public:
    static const strong_ordering less;
    static const strong_ordering greater;
    static const strong_ordering equivalent;
    static const strong_ordering equal;

    constexpr strong_ordering(std::nullptr_t) noexcept : strong_ordering(_construct{}, 0) {}

    constexpr bool operator==(strong_ordering o) const noexcept {
        return _c == o._c;
    }
    constexpr bool operator!=(strong_ordering o) const noexcept {
        return !(*this == o);
    }
#pragma push_macro("DEFOP")
#undef DEFOP
#define DEFOP(Op)                                               \
    constexpr bool operator Op(std::nullptr_t) const noexcept { \
        return _c Op 0;                                         \
    }                                                           \
    static_assert(true, "")
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
#pragma pop_macro("DEFOP")

    // nonstd: Swap greater/less values
    constexpr strong_ordering inverted() const noexcept {
        return *this < nullptr ? greater : *this > nullptr ? less : *this;
    }
};

#pragma push_macro("INLINE_VAR")
#undef INLINE_VAR
#define INLINE_VAR BSONCXX_IF_GNU_LIKE([[gnu::weak]]) BSONCXX_IF_MSVC(__declspec(selectany))

INLINE_VAR const strong_ordering strong_ordering::less =
    strong_ordering(strong_ordering::_construct{}, -1);
INLINE_VAR const strong_ordering strong_ordering::greater =
    strong_ordering(strong_ordering::_construct{}, 1);
INLINE_VAR const strong_ordering strong_ordering::equivalent =
    strong_ordering(strong_ordering::_construct{}, 0);
INLINE_VAR const strong_ordering strong_ordering::equal =
    strong_ordering(strong_ordering::_construct{}, 0);

#pragma pop_macro("INLINE_VAR")

// Implements a three-way comparison between two objects. That is, in
// a single operation, determine whether the left operand is less-than, greater-than,
// or equal-to the right-hand operand.
struct compare_three_way {
    BSONCXX_PUSH_WARNINGS();
    BSONCXX_DISABLE_WARNING(GNU("-Wfloat-equal"));
    template <typename L,
              typename R,
              typename = decltype(std::declval<L>() < std::declval<R>()),
              typename = decltype(std::declval<L>() == std::declval<R>())>
    constexpr static strong_ordering impl(L const& l, R const& r, rank<1>) {
        return (l < r) ? strong_ordering::less
                       : (l == r ? strong_ordering::equal  //
                                 : strong_ordering::greater);
    }
    BSONCXX_POP_WARNINGS();

    template <typename L,
              typename R,
              typename = decltype(tag_invoke(
                  std::declval<compare_three_way>(), std::declval<L>(), std::declval<R>()))>
    constexpr static strong_ordering impl(L const& l, R const& r, rank<2>) {
        return tag_invoke(compare_three_way{}, l, r);
    }

    template <typename L, typename R>
    constexpr auto operator()(L const& l, R const& r) const
        BSONCXX_RETURNS((impl)(l, r, rank<2>{}));
};

// Inherit to define ADL-visible ordering operators based on an ADL-visible
// implementation of tag_invoke(compare_three_way, l, r).
struct ordering_operators {
    template <typename L, typename R>
    constexpr static auto impl(const L& l, const R& r, rank<1>)
        BSONCXX_RETURNS(tag_invoke(compare_three_way{}, l, r));

    template <typename L, typename R>
    constexpr static auto impl(const L& l, const R& r, rank<0>)
        BSONCXX_RETURNS(tag_invoke(compare_three_way{}, r, l).inverted());

#pragma push_macro("DEFOP")
#undef DEFOP
#define DEFOP(Oper)                                             \
    template <typename L, typename R>                           \
    constexpr friend auto operator Oper(const L& l, const R& r) \
        BSONCXX_RETURNS(ordering_operators::impl(l, r, rank<1>{}) Oper nullptr)
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
#pragma pop_macro("DEFOP")
};

template <typename L, typename R>
std::false_type is_partially_ordered_with_f(rank<0>);

template <typename L, typename R>
auto is_partially_ordered_with_f(rank<1>)
    -> true_t<decltype(std::declval<const L&>() > std::declval<const R&>()),
              decltype(std::declval<const L&>() < std::declval<const R&>()),
              decltype(std::declval<const L&>() >= std::declval<const R&>()),
              decltype(std::declval<const L&>() <= std::declval<const R&>()),
              decltype(std::declval<const R&>() < std::declval<const L&>()),
              decltype(std::declval<const R&>() > std::declval<const L&>()),
              decltype(std::declval<const R&>() <= std::declval<const L&>()),
              decltype(std::declval<const R&>() >= std::declval<const L&>())>;

template <typename T, typename U>
struct is_partially_ordered_with : decltype(is_partially_ordered_with_f<T, U>(rank<1>{})) {};

template <typename T>
struct is_totally_ordered
    : conjunction<is_equality_comparable<T>, is_partially_ordered_with<T, T>> {};

template <typename T, typename U>
struct is_totally_ordered_with : conjunction<is_totally_ordered<T>,
                                             is_totally_ordered<U>,
                                             is_equality_comparable<T, U>,
                                             is_partially_ordered_with<T, U>> {};

}  // namespace detail
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides comparison-related utilities for internal use.
///
/// @warning For internal use only!
///
