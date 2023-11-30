#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>

#include "./type_traits.hpp"

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
inline namespace v_noabi {
namespace detail {

template <typename L, typename R>
auto is_equality_comparable_f(...) -> std::false_type;

template <typename L, typename R>
auto is_equality_comparable_f(int,
                              bool b = false,
                              const_reference_t<L> l = soft_declval<L&>(),
                              const_reference_t<R> r = soft_declval<R&>())
    -> true_t<decltype((l == r) ? 0 : 0,  //
                       (r == l) ? 0 : 0,
                       (l != r) ? 0 : 0,
                       (r != l) ? 0 : 0)>;

/**
 * @brief Detect whether two types are equality-comparable.
 *
 * Requires L == R, L != R, R == L, and R != L
 */
template <typename L, typename R, typename = void>
struct is_equality_comparable : decltype(is_equality_comparable_f<L, R>(0)) {};

/**
 * @brief Callable object and tag type for equality comparison
 *
 */
struct equal_to {
    template <typename L, typename R>
    constexpr requires_t<bool, is_equality_comparable<L, R>>  //
    operator()(L&& l, R&& r) const noexcept(noexcept(l == r)) {
        return l == r;
    }
};

/**
 * @brief Derive from this class to define ADL-only operator== and operator!= on the basis of
 * an ADL-only tag_invoke(equal_to, l, r)
 */
class equality_operators {
    template <typename L, typename R>
    constexpr static auto impl(rank<1>, L& l, R& r) bsoncxx_returns(tag_invoke(equal_to{}, l, r));

    template <typename L, typename R>
    constexpr static auto impl(rank<0>, L& l, R& r) bsoncxx_returns(tag_invoke(equal_to{}, r, l));

    template <typename Left, typename Other>
    constexpr friend auto operator==(const Left& self, const Other& other)
        bsoncxx_returns(equality_operators::impl(rank<1>{}, self, other));

    template <typename Left, typename Other>
    constexpr friend auto operator!=(const Left& self, const Other& other)
        bsoncxx_returns(!equality_operators::impl(rank<1>{}, self, other));
};

/**
 * @brief Very basic impl of C++20 std::strong_ordering
 *
 * We don't need other weaker orderings yet, so this is all that we have
 */
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

#define DEFOP(Op)                                               \
    constexpr bool operator Op(std::nullptr_t) const noexcept { \
        return _c Op 0;                                         \
    }                                                           \
    static_assert(true, "")
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
#undef DEFOP
};

#define psuedo_inline bsoncxx_if_gnu_like([[gnu::weak]]) bsoncxx_if_msvc(__declspec(selectany))

psuedo_inline const strong_ordering strong_ordering::less =
    strong_ordering(strong_ordering::_construct{}, -1);
psuedo_inline const strong_ordering strong_ordering::greater =
    strong_ordering(strong_ordering::_construct{}, 1);
psuedo_inline const strong_ordering strong_ordering::equivalent =
    strong_ordering(strong_ordering::_construct{}, 0);
psuedo_inline const strong_ordering strong_ordering::equal =
    strong_ordering(strong_ordering::_construct{}, 0);

#undef psuedo_inline

/**
 * @brief Implements a three-way comparison between two objects. That is, in
 * a single operation, determine whether the left operand is less-than, greater-than,
 * or equal-to the right-hand operand.
 */
struct compare_three_way {
    template <typename L,
              typename R,
              typename = decltype(std::declval<L>() < std::declval<R>()),
              typename = decltype(std::declval<L>() == std::declval<R>())>
    constexpr static strong_ordering impl(L const& l, R const& r, rank<1>) {
        return (l < r) ? strong_ordering::less
                       : (l == r ? strong_ordering::equal  //
                                 : strong_ordering::greater);
    }

    template <typename L,
              typename R,
              typename = decltype(tag_invoke(
                  std::declval<compare_three_way>(), std::declval<L>(), std::declval<R>()))>
    constexpr strong_ordering impl(L const& l, R const& r, rank<2>) const {
        return tag_invoke(*this, l, r);
    }

    template <typename L, typename R>
    constexpr auto operator()(L const& l, R const& r) const
        bsoncxx_returns((impl)(l, r, rank<2>{}));
};

/**
 * @brief Inherit to define ADL-visible ordering operators based on an ADL-visible
 * implementation of tag_invoke(compare_three_way, l, r)
 */
struct ordering_operators {
#define DEFOP(Oper)                                             \
    template <typename L, typename R>                           \
    constexpr friend auto operator Oper(const L& l, const R& r) \
        bsoncxx_returns(tag_invoke(compare_three_way{}, l, r) Oper 0)
    DEFOP(<);
    DEFOP(>);
    DEFOP(<=);
    DEFOP(>=);
#undef DEFOP
};

}  // namespace detail
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
