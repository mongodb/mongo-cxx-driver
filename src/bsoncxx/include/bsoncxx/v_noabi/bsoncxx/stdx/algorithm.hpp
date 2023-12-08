// Copyright 2023 MongoDB Inc.
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

#include <algorithm>

#include "./algorithm.hpp"
#include "./iterator.hpp"
#include "./operators.hpp"
#include "./ranges.hpp"
#include "./type_traits.hpp"

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
inline namespace v_noabi {
namespace detail {

/**
 * @internal
 * @brief Checked iterator-advance
 *
 * Provides two signatures:
 *
 * - advance(iterator, sentinel)
 * - advance(iterator, offset, sentinel)
 *
 * The default `offset` is 1. The return value is the offset by which the
 * iterator was actually shifted, which may be different from `offset` if
 * `sentinel` was encountered before advancing the entire distance.
 */
static constexpr struct _advance_fn {
    template <typename I, typename S>
    bsoncxx_cxx14_constexpr static auto impl(I& iter,
                                             iter_difference_t<I> off,
                                             S bound)  //
        noexcept(noexcept(++iter) && noexcept(iter == bound))
            -> requires_t<iter_difference_t<I>, is_sentinel_for<S, I>> {
        iter_difference_t<I> shift = 0;
        while (off > 0 && iter != bound) {
            ++iter;
            --off;
            ++shift;
        }
        handle_negative(iter, off, bound, shift);
        return shift;
    }

    template <typename I, typename S>
    constexpr auto operator()(I& iter, S bound) const bsoncxx_returns(impl(iter, 1, bound));

    template <typename I, typename S>
    constexpr auto operator()(I& iter, iter_difference_t<I> off, S bound) const
        bsoncxx_returns(impl(iter, off, bound));

    // Handle bidirectional iterators and negative offsets
    template <typename I, typename S>
    bsoncxx_cxx14_constexpr static requires_t<void, is_bidirectional_iterator<I>>  //
    handle_negative(I& iter,
                    iter_difference_t<I> off,
                    S bound,
                    iter_difference_t<I>& shift) noexcept(noexcept(--iter)) {
        while (off < 0 && iter != bound) {
            --iter;
            ++off;
            --shift;
        }
    }

    // Fallback: Iterator is not bidir. This is less-preferred since it uses an ellipsis
    template <typename I, typename S>
    bsoncxx_cxx14_constexpr static void handle_negative(I, iter_difference_t<I>, S, ...) noexcept {}
} advance;

/**
 * @internal
 * @brief Obtain a new iterator shifted from the given iterator by some amount,
 * with bounds-checking
 *
 * Two signatures:
 *
 * - next(iterator, bound)
 * - next(iterator, offset, bound)
 *
 * Equivalent to copying `iterator` as `t`, calling `advance(t[, offset], bound)`,
 * and returning `t`.
 */
static constexpr struct _next_fn {
    template <typename I, typename S>
    bsoncxx_cxx14_constexpr auto operator()(I it, S bound) const
        -> requires_t<I, is_sentinel_for<S, I>> {
        advance(it, bound);
        return it;
    }

    template <typename I, typename S>
    bsoncxx_cxx14_constexpr auto operator()(I it, iter_difference_t<I> offset, S bound) const
        -> requires_t<I, is_sentinel_for<S, I>> {
        advance(it, offset, bound);
        return it;
    }
} next;

/**
 * @internal
 * @brief Encloses a range represented by an iterator+sentinel pair
 *
 * Provides begin(), end(), and empty(). If the iterator+sentinel pair are
 * "sized", provides a size() member as well.
 */
template <typename I, typename S = I>
struct subrange;

template <typename I, typename S, bool IsSized>
struct subrange_base {
    I _iter;
    S _stop;

    constexpr subrange_base(I i, S s) noexcept : _iter(i), _stop(s) {}

    constexpr I begin() const noexcept {
        return _iter;
    }
    constexpr S end() const noexcept {
        return _stop;
    }

    constexpr bool empty() const noexcept {
        return begin() == end();
    }
};

template <typename I, typename S>
struct subrange_base<I, S, true> : subrange_base<I, S, false> {
    using subrange_base<I, S, false>::subrange_base;

    constexpr detail::make_unsigned_t<iter_difference_t<I>> size() const noexcept {
        return static_cast<detail::make_unsigned_t<iter_difference_t<I>>>(this->_stop -
                                                                          this->_iter);
    }
};

template <typename I, typename S>
struct subrange : subrange_base<I, S, is_sized_sentinel_for<S, I>::value> {
    using subrange::subrange_base::subrange_base;
};

/**
 * @internal
 * @brief Construct a new subrange from the given iterator+sentinel pair
 */
template <typename I, typename S>
constexpr requires_t<subrange<I, S>, is_sentinel_for<S, I>> make_subrange(I i, S s) noexcept {
    return subrange<I, S>{i, s};
}

template <typename R>
using reversed_t = subrange<
    std::reverse_iterator<requires_t<iterator_t<R>, is_bidirectional_iterator<iterator_t<R>>>>>;

template <typename R>
constexpr reversed_t<R>  //
make_reversed_view(R&& rng) noexcept {
    return {detail::make_reverse_iterator(end(rng)), detail::make_reverse_iterator(begin(rng))};
}

/**
 * @internal
 * @brief Obtain the size of a range as a signed integer.
 *
 * Unlike size() or ssize(), this invocable is defined for all ranges, even if the
 * underlying operation is not O(1) or the range is an input_range. Uses ssize()
 * if available, otherwise counts iterator increments.
 */
static constexpr struct _distance_fn {
    template <typename R>
    constexpr static auto impl(rank<2>, R&& rng) bsoncxx_returns(ssize(rng));

    template <typename R>
    bsoncxx_cxx14_constexpr static auto impl(rank<1>, R&& rng)
        -> requires_t<range_difference_t<R>, is_range<R>> {
        range_difference_t<R> count = 0;
        auto i = begin(rng);
        auto s = end(rng);
        for (; i != s; ++i) {
            ++count;
        }
        return count;
    }

    template <typename I, typename S>
    constexpr auto operator()(I i, S s) const bsoncxx_returns(impl(rank<2>{}, make_subrange(i, s)));
    template <typename R>
    constexpr auto operator()(R&& rng) const bsoncxx_returns(impl(rank<2>{}, rng));
} distance;

/**
 * @brief An invocable object that simply returns its argument unchanged.
 */
struct identity {
    template <typename T>
    constexpr T&& operator()(T&& arg) const noexcept {
        return static_cast<T&&>(arg);
    }
};

/**
 * @internal
 * @brief Test whether the given ranges have equal contents
 *
 * Call as:
 *
 * - equal(left_range, right_range[, compare_fn[, left-projection[, right-projection]]])
 */
static constexpr struct _equal_fn {
    template <typename L,
              typename R,
              typename Compare = equal_to,
              typename ProjectLeft = identity,
              typename ProjectRight = identity>
    bsoncxx_cxx14_constexpr auto operator()(L&& left,
                                            R&& right,
                                            Compare&& eq = {},
                                            ProjectLeft&& projleft = {},
                                            ProjectRight&& projright = {}) const  //
        -> requires_t<bool,
                      is_invocable<Compare,
                                   invoke_result_t<ProjectLeft, range_reference_t<L>>,
                                   invoke_result_t<ProjectRight, range_reference_t<R>>>> {
        if (definitely_different_sizes(rank<1>{}, left, right)) {
            // The ranges are known to have different sizes, so cannot be equal
            return false;
        }
        auto li = begin(left);
        const auto ls = end(left);
        auto ri = begin(right);
        const auto rs = end(right);
        for (; li != ls && ri != rs; ++li, ++ri) {
            if (!eq(projleft(*li), projright(*ri))) {
                return false;
            }
        }
        // All elements compared equal, so check that we compared all elements of both ranges:
        return li == ls && ri == rs;
    }

    template <typename L, typename R>
    constexpr static auto definitely_different_sizes(rank<1>, L& l, R& r)
        bsoncxx_returns(ssize(l) != ssize(r));
    template <typename L, typename R>
    constexpr static bool definitely_different_sizes(rank<0>, L&, R&) noexcept {
        return false;
    }
} equal;

/**
 * @internal
 * @brief A default searcher implementation for use with search()
 *
 * Scans the searched range for the needle by simple element-by-element comparison.
 */
template <typename NeedleIter, typename NeedleStop, typename Compare = equal_to>
struct default_searcher;

/**
 * @internal
 * @brief Implements subrange-searching
 *
 * Callable with:
 *
 * - search(outer_range, inner_range[, compare_fn])
 * - search(outer_range, searcher)
 *
 * When given two ranges, uses default_searcher with `compare_fn` to search
 * for the first occurrence of `inner_range` within `outer_range`.
 *
 * Returns a subrange referring to the range that was found by searching, or
 * an empty subrange pointing to the end of the scanned range if no occurrence
 * was found.
 */
static constexpr struct _search_fn {
    template <typename Range, typename Searcher, typename Iter = iterator_t<Range>>
    bsoncxx_cxx14_constexpr static auto impl(Range&& rng,
                                             Searcher&& srch)  //
        noexcept(noexcept(srch(begin(rng), end(rng))))
            -> requires_t<subrange<Iter>, is_invocable<const Searcher&, Iter, sentinel_t<Range>>> {
        const std::pair<Iter, Iter> found = srch(begin(rng), end(rng));
        return {found.first, found.second};
    }

    template <typename Hay, typename Needle, typename Compare = equal_to>
    constexpr auto operator()(Hay&& hay, Needle&& ndl, Compare&& cmp = {}) const
        bsoncxx_returns((impl)(hay,
                               default_searcher<iterator_t<Needle>, sentinel_t<Needle>, Compare>{
                                   begin(ndl), end(ndl), static_cast<Compare&&>(cmp)}));

    template <typename Hay, typename Searcher>
    constexpr auto operator()(Hay&& hay, Searcher&& srch) const bsoncxx_returns(impl(hay, srch));
} search;

template <typename NeedleIter, typename NeedleStop, typename Compare>
struct default_searcher {
    NeedleIter n_first;
    NeedleStop n_last;
    Compare _compare = Compare();
    iter_difference_t<NeedleIter> n_size = distance(n_first, n_last);

    default_searcher(NeedleIter first, NeedleStop last, Compare cmp = Compare())
        : n_first(first), n_last(last), _compare(cmp) {}

    template <typename Iter, typename Stop>
    bsoncxx_cxx14_constexpr std::pair<Iter, Iter> operator()(Iter first, Stop last) const {
        auto test_begin = first;
        auto test_end = first;
        auto ndl = make_subrange(n_first, n_last);
        if (advance(test_end, n_size, last) != n_size) {
            // We did not advance the full needle size, so the test range is smaller than the
            // needle, and cannot contain the needle:
            return std::make_pair(test_end, test_end);
        }
        while (1) {
            // Create a subrange of the current part being tested:
            auto test_range = make_subrange(test_begin, test_end);
            if (equal(test_range, ndl, _compare)) {
                // This subrange is equal, so return that subrange
                return {test_range.begin(), test_range.end()};
            }
            // Step forward:
            if (test_end == last) {
                return {test_end, test_end};
            }
            ++test_begin;
            ++test_end;
        }
    }
};

template <typename T>
struct equal_to_value_t {
    T value;
    template <typename U>
    constexpr auto operator()(U&& u) const bsoncxx_returns(u == value);
};

/**
 * @internal
 * @brief Create a predicate that tests whether a value is equal to a given value
 */
template <typename T>
constexpr equal_to_value_t<T> equal_to_value(T&& t) noexcept {
    return {static_cast<T&&>(t)};
}

/**
 * @internal
 * @brief Obtain the iterator referring to the first position in a range for
 * which a given predicate returns true
 */
static constexpr struct _find_if_fn {
    template <typename R, typename Predicate, typename Project = identity>
    bsoncxx_cxx14_constexpr auto operator()(R&& rng, Predicate&& pred, Project&& proj = {}) const
        -> requires_t<iterator_t<R>> {
        auto iter = begin(rng);
        const auto stop = end(rng);
        for (; iter != stop; ++iter) {
            if (pred(proj(*iter))) {
                break;
            }
        }
        return iter;
    }
} find_if;

/**
 * @internal
 * @brief Find the given value in the given range, compared by the `==` operator
 */
static constexpr struct _find_fn {
    template <typename R, typename T, typename Project = identity>
    constexpr auto operator()(R&& rng, const T& value, Project&& proj = {}) const
        bsoncxx_returns(find_if(rng, equal_to_value(value), proj));
} find;

template <typename T>
struct equal_to_any_of_t {
    T range;

    template <typename U>
    constexpr auto operator()(U&& value) const bsoncxx_returns(find(range, value) != end(range));
};

/**
 * @internal
 * @brief Create a predicate that tests whether a value exists within the
 * given range of objects.
 */
template <typename T>
constexpr auto equal_to_any_of(T&& rng)
    -> requires_t<equal_to_any_of_t<T>, is_forward_iterator<iterator_t<T>>> {
    return {static_cast<T&&>(rng)};
}

template <typename F>
struct not_fn_t {
    F func;
    template <typename... Args>
    bsoncxx_cxx14_constexpr auto operator()(Args&&... args)
        bsoncxx_returns(!invoke(func, static_cast<Args&&>(args)...));
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const
        bsoncxx_returns(!invoke(func, static_cast<Args&&>(args)...));
};

/**
 * @internal
 * @brief Create a wrapper function that negates the result of invoking the underlying
 * invocable
 */
template <typename F>
constexpr not_fn_t<F> not_fn(F&& fn) {
    return {static_cast<F&&>(fn)};
}

}  // namespace detail
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
