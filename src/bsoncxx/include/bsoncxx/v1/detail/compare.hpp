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

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>

#include <cstddef>
#include <functional>
#include <type_traits>

namespace bsoncxx {
namespace detail {

// Callable object and tag type for equality comparison.
struct equal_to {
    template <typename L, typename R>
    constexpr requires_t<bool, is_equality_comparable<L, R>> operator()(L&& l, R&& r) const noexcept(noexcept(l == r)) {
        return l == r;
    }
};

// Derive from this class to define ADL-only operator== and operator!= on the basis of
// an ADL-only tag_invoke(equal_to, l, r).
class equality_operators {
    template <typename L, typename R>
    constexpr static auto impl(rank<1>, L& l, R& r) BSONCXX_PRIVATE_RETURNS(tag_invoke(equal_to{}, l, r));

    template <typename L, typename R>
    constexpr static auto impl(rank<0>, L& l, R& r) BSONCXX_PRIVATE_RETURNS(tag_invoke(equal_to{}, r, l));

    // @cond DOXYGEN_DISABLE "Found ';' while parsing initializer list!"
    template <typename Left, typename Other>
    constexpr friend auto operator==(Left const& self, Other const& other)
        BSONCXX_PRIVATE_RETURNS(equality_operators::impl(rank<1>{}, self, other));
    // @endcond

    // @cond DOXYGEN_DISABLE "Found ';' while parsing initializer list!"
    template <typename Left, typename Other>
    constexpr friend auto operator!=(Left const& self, Other const& other)
        BSONCXX_PRIVATE_RETURNS(!equality_operators::impl(rank<1>{}, self, other));
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
    static strong_ordering const less;
    static strong_ordering const greater;
    static strong_ordering const equivalent;
    static strong_ordering const equal;

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
#define INLINE_VAR                             \
    BSONCXX_PRIVATE_IF_GNU_LIKE([[gnu::weak]]) \
    BSONCXX_PRIVATE_IF_MSVC(__declspec(selectany))

INLINE_VAR const strong_ordering strong_ordering::less = strong_ordering(strong_ordering::_construct{}, -1);
INLINE_VAR const strong_ordering strong_ordering::greater = strong_ordering(strong_ordering::_construct{}, 1);
INLINE_VAR const strong_ordering strong_ordering::equivalent = strong_ordering(strong_ordering::_construct{}, 0);
INLINE_VAR const strong_ordering strong_ordering::equal = strong_ordering(strong_ordering::_construct{}, 0);

#pragma pop_macro("INLINE_VAR")

// Implements a three-way comparison between two objects. That is, in
// a single operation, determine whether the left operand is less-than, greater-than,
// or equal-to the right-hand operand.
struct compare_three_way {
    BSONCXX_PRIVATE_WARNINGS_PUSH();
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));
    template <
        typename L,
        typename R,
        typename = decltype(std::declval<L>() < std::declval<R>()),
        typename = decltype(std::declval<L>() == std::declval<R>())>
    constexpr static strong_ordering impl(L const& l, R const& r, rank<1>) {
        return (l < r) ? strong_ordering::less : (l == r ? strong_ordering::equal : strong_ordering::greater);
    }
    BSONCXX_PRIVATE_WARNINGS_POP();

    template <
        typename L,
        typename R,
        typename = decltype(tag_invoke(std::declval<compare_three_way>(), std::declval<L>(), std::declval<R>()))>
    constexpr static strong_ordering impl(L const& l, R const& r, rank<2>) {
        return tag_invoke(compare_three_way{}, l, r);
    }

    template <typename L, typename R>
    constexpr auto operator()(L const& l, R const& r) const BSONCXX_PRIVATE_RETURNS((impl)(l, r, rank<2>{}));
};

// Inherit to define ADL-visible ordering operators based on an ADL-visible
// implementation of tag_invoke(compare_three_way, l, r).
struct ordering_operators {
    template <typename L, typename R>
    constexpr static auto impl(L const& l, R const& r, rank<1>)
        BSONCXX_PRIVATE_RETURNS(tag_invoke(compare_three_way{}, l, r));

    template <typename L, typename R>
    constexpr static auto impl(L const& l, R const& r, rank<0>)
        BSONCXX_PRIVATE_RETURNS(tag_invoke(compare_three_way{}, r, l).inverted());

#pragma push_macro("DEFOP")
#undef DEFOP
#define DEFOP(Oper)                                             \
    template <typename L, typename R>                           \
    constexpr friend auto operator Oper(L const& l, R const& r) \
        BSONCXX_PRIVATE_RETURNS(ordering_operators::impl(l, r, rank<1>{}) Oper nullptr)
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
#pragma pop_macro("DEFOP")
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
