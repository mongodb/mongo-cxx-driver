/**
 * @file ranges.hpp
 * @brief A backport of a small amount of std::ranges from C++20
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <bsoncxx/stdx/iterator.hpp>
#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
inline namespace v_noabi {
namespace detailx {
// clang-format off
// Workarounds for MSVC 19.10 doing bad: An invocable object with name `n` should not be visible
// within its own call operator. We need to "hide" the ADL name lookup out here in a different
// namespace to prevent them from finding the invocable objects.
template <typename T>
constexpr auto adl_size(T&& t) bsoncxx_returns(size((T&&)t));
template <typename T>
constexpr auto adl_begin(T&& t) bsoncxx_returns(begin((T&&)t));
template <typename T>
constexpr auto adl_end(T&& t) bsoncxx_returns(end((T&&)t));
}  // namespace detailx
// clang-format on

namespace detail {

/**
 * XXX: These _decay_xyz functions are required for MSVC 14.0 (VS2015) compat.
 *
 * The requirements on each of the below invocable objects is that the decay-copied
 * return type meet certain requirements. It would be easier to include these
 * requirements inline directly via enable_if/requires_t, but it is difficult to
 * position them in a way that doesn't confuse MSVC 14.0. The most reliable
 * configuration is to use a SFINAE-disappearing return type, so these functions
 * serve that purpose while simultaneously performing the required decay-copy.
 *
 * It's possible that these are actually easier on the eyes than the inline
 * requirements…
 */
template <typename I>
constexpr requires_t<I, is_iterator<I>> _decay_iterator(I i) noexcept {
    return i;
}

template <typename I, typename S>
constexpr requires_t<S, is_sentinel_for<S, I>> _decay_sentinel(S s) noexcept {
    return s;
}

template <typename Sz>
constexpr requires_t<Sz, std::is_integral<Sz>> _decay_integral(Sz s) noexcept {
    return s;
}

template <typename P>
constexpr requires_t<P, std::is_pointer<P>> _decay_copy_pointer(P p) noexcept {
    return p;
}

/**
 * @brief Access the beginning iterator of a range-like object.
 *
 * Requires that the result is a valid iterator type
 *
 * Based on std::ranges::begin()
 */
static constexpr struct _begin_fn {
    // 1: Object is an array
    template <typename El, std::size_t N>
    static constexpr auto impl(El (&arr)[N], rank<5>) bsoncxx_returns(arr + 0);

    // 2: Object has member .begin() returning an iterator
    template <typename R>
    static constexpr auto impl(R& rng, rank<4>) bsoncxx_returns(_decay_iterator(rng.begin()));

    // 3: Object has an ADL-visible begin(x) returning an iterator
    template <typename R>
    static constexpr auto impl(R& rng, rank<3>)
        bsoncxx_returns(_decay_iterator(detailx::adl_begin(rng)));

    template <typename R>
    constexpr auto operator()(R&& rng) const bsoncxx_returns((impl)(rng, rank<10>{}));
} begin;

/**
 * @brief Yields the iterator result from calling begin(R&), if that expression
 * is valid
 */
template <typename R>
using iterator_t = decltype(begin(std::declval<R&>()));

/**
 * @brief Access the sentinel value for a range-like object.
 *
 * Requires that a valid begin() is also available, and that the end() returns
 * a type which is_sentinel_for the type iterator_t<R>
 *
 * Based on std::ranges::end().
 */
static constexpr struct _end_fn {
    // 1: Range is an array
    template <typename Iter, typename El, std::size_t N>
    static constexpr auto impl(El (&arr)[N], rank<5>) bsoncxx_returns(arr + N);

    // 2: Range has member .end() returning a valid sentinel
    template <typename Iter, typename R>
    static constexpr auto impl(R& rng, rank<4>) bsoncxx_returns(_decay_sentinel<Iter>(rng.end()));

    // 3: Range has ADL-found end(x) returning a valid sentinel
    template <typename Iter, typename R>
    static constexpr auto impl(R& r, rank<3>)
        bsoncxx_returns(_decay_sentinel<Iter>(detailx::adl_end(r)));

    template <typename R>
    constexpr auto operator()(R&& rng) const bsoncxx_returns((impl<iterator_t<R>>)(rng, rank<5>{}));
} end;

/**
 * @brief Yield the type resulting from end(R&), if that expression is valid
 */
template <typename R>
using sentinel_t = decltype(end(std::declval<R&>()));

/**
 * @brief Obtain the size of the given range `rng`.
 *
 * Returns the first valid of:
 * - The bounds of the array, if `rng` is an array
 * - The result of rng.size() on R, if that returns an integral type
 * - The result of size(rng), if such a name is visible via ADL and returns
 *   an integral type.
 * - The value of (end(rng) - begin(rng)) as an unsigned integer if `rng` is
 *   forward-iterable and the sentinel is a sized sentinel type.
 */
static constexpr struct _size_fn {
    // 1: Array of known bound
    template <typename Element, std::size_t N>
    static constexpr auto impl(Element (&)[N], rank<5>) bsoncxx_returns(N);

    // 2: Range with member .size()
    template <typename R>
    static constexpr auto impl(R& rng, rank<4>) bsoncxx_returns(_decay_integral(rng.size()));

    // 3: Range with ADL-found size(x)
    template <typename R>
    static constexpr auto impl(R& rng, rank<3>)
        bsoncxx_returns(_decay_integral(detailx::adl_size(rng)));

    // 4: Range is a forward-range and has a sized sentinel type
    template <typename R,
              typename Iter = iterator_t<R>,
              typename Sentinel = sentinel_t<R>,
              // Require a forward iterator:
              requires_t<int, is_forward_iterator<Iter>> = 0,
              // Require a sized sentinel:
              requires_t<int, is_sized_sentinel_for<Sentinel, Iter>> = 0,
              // We cast to an unsigned type from the difference type:
              typename Sz = make_unsigned_t<difference_t<Sentinel, Iter>>>
    static constexpr auto impl(R& rng, rank<2>)
        bsoncxx_returns(static_cast<Sz>(end(rng) - begin(rng)));

    template <typename R>
    constexpr auto operator()(R&& rng) const bsoncxx_returns((impl)(rng, rank<10>{}));
} size;

/**
 * @brief Obtain the size type of the given range
 */
template <typename R>
using range_size_t = decltype(size(std::declval<R&>()));

/**
 * @brief Obtain the size of the given range as a signed integer type
 */
static constexpr struct _ssize_fn {
    template <typename R,
              typename Unsigned = range_size_t<R>,
              typename Signed = make_signed_t<Unsigned>,
              typename RetDiff =
                  conditional_t<(sizeof(Signed) > sizeof(std::ptrdiff_t)), Signed, std::ptrdiff_t>>
    constexpr auto operator()(R&& rng) const bsoncxx_returns(static_cast<RetDiff>(size(rng)));
} ssize;

/**
 * @brief Obtain the difference type of the given range
 */
template <typename R>
using range_difference_t = iter_difference_t<iterator_t<R>>;

/**
 * @brief Obtain a pointer-to-data for the given `rng`.
 *
 * Returns the first valid of:
 * - `rng.data()` if such expression yields a pointer type
 * - `to_address(begin(rng))` if such expression is valid and iterator_t<R> is
 *   a contiguous_iterator.
 */
static constexpr struct _data_fn {
    template <typename R>
    static constexpr auto impl(R&& rng, rank<2>) bsoncxx_returns(_decay_copy_pointer(rng.data()));

    template <typename R, requires_t<int, is_contiguous_iterator<iterator_t<R>>> = 0>
    static constexpr auto impl(R&& rng, rank<1>) bsoncxx_returns(to_address(begin(rng)));

    template <typename R>
    constexpr auto operator()(R&& rng) const bsoncxx_returns((impl)(rng, rank<10>{}));
} data;

/**
 * @brief Get the type returned by data(R&), if valid
 */
template <typename R>
using range_data_t = decltype(data(std::declval<R&>()));

/**
 * @brief Get the value type of the range
 *
 * Equivalent: iter_value_t<iterator_t<R>>
 */
template <typename R>
using range_value_t = iter_value_t<iterator_t<R>>;

template <typename R>
using range_reference_t = iter_reference_t<iterator_t<R>>;

template <typename R>
using range_concept_t = iterator_concept_t<iterator_t<R>>;

/**
 * @brief Trait detects if the given type is a range
 */
template <typename R>
struct is_range : conjunction<is_detected<iterator_t, R>, is_detected<sentinel_t, R>> {};

/**
 * @brief Detect if the given range is contiguous-ish.
 *
 * This is not quite like ranges::contiguous_range, as it does not require that
 * the iterator be configuous. It only looks for a valid data(R) and size(R).
 * Without stdlib support, we cannot fully detect contiguous_iterators, but we
 * want to be able to support, basic_string<C>, vector<T>, array<T>, etc, which
 * have .data() and .size()
 */
template <typename R>
struct is_contiguous_range
    : conjunction<is_range<R>, is_detected<range_data_t, R>, is_detected<range_size_t, R>> {};

static constexpr struct unreachable_sentinel_t : equality_operators {
    template <typename I>
    constexpr friend requires_t<bool, is_iterator<I>> tag_invoke(equal_to,
                                                                 unreachable_sentinel_t,
                                                                 I) noexcept {
        return false;
    }
} unreachable_sentinel;

}  // namespace detail
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
