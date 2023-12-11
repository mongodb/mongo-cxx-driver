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

#include <cstddef>
#include <ios>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include <bsoncxx/stdx/algorithm.hpp>
#include <bsoncxx/stdx/iterator.hpp>
#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/ranges.hpp>
#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
inline namespace v_noabi {
namespace stdx {

namespace detail {

template <typename S>
auto detect_string_f(...) -> std::false_type;

template <typename S>
auto detect_string_f(int,  //
                     const S& s = bsoncxx::detail::soft_declval<S>(),
                     S& mut = bsoncxx::detail::soft_declval<S&>())
    -> bsoncxx::detail::true_t<
        typename S::traits_type::char_type,
        decltype(s.length()),
        decltype(mut = s),
        decltype(s.compare(s)),
        decltype(s.substr(0, s.size())),
        bsoncxx::detail::requires_t<void,
                                    bsoncxx::detail::is_equality_comparable<S, S>,
                                    bsoncxx::detail::is_range<S>>>;

// Heuristic detection of std::string-like types. Not perfect, but should reasonably
// handle most cases.
template <typename S>
struct is_string_like : decltype(detect_string_f<bsoncxx::detail::remove_cvref_t<S>>(0)) {};

}  // namespace detail

/**
 * @brief Implementation of std::string_view-like class template
 */
template <typename Char, typename Traits = std::char_traits<Char>>
class basic_string_view : bsoncxx::detail::equality_operators, bsoncxx::detail::ordering_operators {
   public:
    // Pointer to (non-const) character type
    using pointer = Char*;
    // Pointer to const-character type
    using const_pointer = const Char*;
    // Type representing the size of a string
    using size_type = std::size_t;
    // Type representing the offset within a string
    using difference_type = std::ptrdiff_t;
    // The type of the string character
    using value_type = Char;

    // Constant sentinel value to represent an impossible/invalid string position
    static constexpr const size_type npos = static_cast<size_type>(-1);

   private:
    // Pointer to the beginning of the string being viewed
    const_pointer _begin = nullptr;
    // The size of the array that is being viewed via `_begin`
    size_type _size = 0;
    // Alias of our own type
    using self_type = basic_string_view;

    /**
     * @brief If R is a type for which we want to permit from-range construction,
     * evaluates to the type `int`. Otherwise, is a substitution failure.
     */
    template <typename R>
    using _enable_range_constructor = bsoncxx::detail::requires_t<
        int,
        // Must be a contiguous range
        bsoncxx::detail::is_contiguous_range<R>,
        // Don't eat our own copy/move constructor:
        bsoncxx::detail::negation<bsoncxx::detail::is_alike<R, self_type>>,
        // Don't handle character arrays (we use a different constructor for
        // that)
        bsoncxx::detail::negation<std::is_convertible<R, const_pointer>>,
        // The range's value must be the same as our character type
        std::is_same<bsoncxx::detail::detected_t<bsoncxx::detail::range_value_t, R>, value_type>>;

   public:
    using traits_type = Traits;
    using reference = Char&;
    using const_reference = const Char&;
    using iterator = const_pointer;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief Default constructor. Constructs to an empty/null string view
     */
    constexpr basic_string_view() = default;

    /// Default copy/move/assign/destroy

    /**
     * @brief Construct a new string view from a pointer-to-character and an
     * array length.
     */
    constexpr basic_string_view(const_pointer s, size_type count) noexcept
        : _begin(s), _size(count) {}

    /**
     * @brief Construct a new string view from a C-style null-terminated character array.
     *
     * The string size is inferred as-if by strlen()
     */
    constexpr basic_string_view(const_pointer s) noexcept
        : _begin(s), _size(traits_type::length(s)) {}

    /**
     * Iterator pair range constructor
     *
     * Requires that `Iterator` be a contiguous iterator, that `Sentinel` be a sized sentinel for
     * `Iterator`, and that the value-type of `Iterator` is the character type of the string.
     */
    template <
        typename Iterator,
        typename Sentinel,
        bsoncxx::detail::requires_t<int,
                                    bsoncxx::detail::is_sized_sentinel_for<Sentinel, Iterator>> = 0,
        // Requires: The iterator value_type be the same as our value_type
        bsoncxx::detail::
            requires_t<int, std::is_same<bsoncxx::detail::iter_value_t<Iterator>, value_type>> = 0,
        // Requires: We can get a pointer from the iterator via to_address:
        bsoncxx::detail::requires_t<int, bsoncxx::detail::is_contiguous_iterator<iterator>> = 0,
        // Requires: "Sentinel" is *not* convertible to std::size_t
        // (prevents ambiguity with the pointer+size constructor)
        bsoncxx::detail::requires_t<
            int,
            bsoncxx::detail::negation<std::is_convertible<Sentinel, std::size_t>>> = 0>
    constexpr basic_string_view(Iterator iter, Sentinel stop) noexcept
        : _begin(bsoncxx::detail::to_address(iter)), _size(static_cast<size_type>(stop - iter)) {}

    /**
     * @brief From-range constructor for non-string-like types. This is an explicit constructor.
     *
     * Requires that `Range` is a non-array contiguous range with the same value
     * type as the string view.
     */
    template <
        typename Range,
        _enable_range_constructor<Range> = 0,
        bsoncxx::detail::requires_t<int, bsoncxx::detail::negation<detail::is_string_like<Range>>>
            RequiresNotString = 0>
    constexpr explicit basic_string_view(Range&& rng)
        : _begin(bsoncxx::detail::data(rng)), _size(bsoncxx::detail::size(rng)) {}

    /**
     * @brief From-range constructor, but is an implicit conversion accepting string-like ranges.
     *
     * Requires that `Range` is a non-array contiguous range with the same value type
     * as the string view, and is a std::string-like value.
     */
    template <
        typename Range,
        _enable_range_constructor<Range> = 0,
        bsoncxx::detail::requires_t<int, detail::is_string_like<Range>> RequiresStringLike = 0>
    constexpr basic_string_view(Range&& rng) noexcept
        : _begin(bsoncxx::detail::data(rng)), _size(bsoncxx::detail::size(rng)) {}

    // Construction from a null pointer is deleted
    basic_string_view(std::nullptr_t) = delete;

    constexpr iterator begin() const noexcept {
        return iterator(_begin);
    }
    constexpr iterator end() const noexcept {
        return begin() + size();
    }
    constexpr iterator cbegin() const noexcept {
        return begin();
    }
    constexpr iterator cend() const noexcept {
        return end();
    }

    constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator{end()};
    }

    constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{cend()};
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{crbegin()};
    }

    /**
     * @brief Access the Nth element of the referred-to string
     *
     * @param offset A zero-based offset within the string to access. Must be less
     * than size()
     */
    constexpr const_reference operator[](size_type offset) const noexcept {
        return _begin[offset];
    }

    /**
     * @brief Access the Nth element of the referred-to string.
     *
     * @param pos A zero-based offset within the string to access. If not less
     * than size(), throws std::out_of_range
     */
    bsoncxx_cxx14_constexpr const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range{"bsoncxx::stdx::basic_string_view::at()"};
        }
        return _begin[pos];
    }
    /// Access the first character in the string
    constexpr const_reference front() const noexcept {
        return (*this)[0];
    }
    /// Access the last character in the string
    constexpr const_reference back() const noexcept {
        return (*this)[size() - 1];
    }

    /// Obtain a pointer to the beginning of the referred-to character array
    constexpr const_pointer data() const noexcept {
        return _begin;
    }
    /// Obtain the length of the referred-to string, in number of characters
    constexpr size_type size() const noexcept {
        return _size;
    }
    /// Obtain the length of the referred-to string, in number of characters
    constexpr size_type length() const noexcept {
        return size();
    }
    /// Return `true` if size() == 0, otherwise `false`
    constexpr bool empty() const noexcept {
        return size() == 0;
    }
    /// Return the maximum value that could be returned by size()
    constexpr size_type max_size() const noexcept {
        return static_cast<size_type>(std::numeric_limits<difference_type>::max());
    }

    /**
     * @brief In-place modify the string_view to view N fewer characters from the beginning
     *
     * @param n The number of characters to remove from the beginning. Must be less than size()
     */
    bsoncxx_cxx14_constexpr void remove_prefix(size_type n) noexcept {
        _begin += n;
        _size -= n;
    }

    /**
     * @brief In-place modify the string_view to view N fewer characters from the end
     *
     * @param n The number of characters to remove from the end. Must be less than size()
     */
    bsoncxx_cxx14_constexpr void remove_suffix(size_type n) noexcept {
        _size -= n;
    }

    /**
     * @brief Swap the reference with another string_view
     */
    bsoncxx_cxx14_constexpr void swap(basic_string_view& other) noexcept {
        std::swap(_begin, other._begin);
        std::swap(_size, other._size);
    }

    /**
     * @brief Copy the contents of the viewed string into the given output destination.
     *
     * @param dest The destination at which to write characters
     * @param count The maximum number of characters to copy.
     * @param pos The offset within the viewed string to begin copying from.
     * @returns The number of characters that were copied to `dest`. The number
     * of copied characters is always the lesser of `size()-pos` and `count`
     *
     * @throws std::out_of_range if pos > size()
     */
    bsoncxx_cxx14_constexpr size_type copy(pointer dest, size_type count, size_type pos = 0) const {
        if (pos > size()) {
            throw std::out_of_range{"bsoncxx::stdx::basic_string_view::substr()"};
        }
        count = (std::min)(count, size() - pos);
        Traits::copy(dest, data() + pos, count);
        return count;
    }

    /**
     * @brief Obtain a substring of this string
     *
     * @param pos The zero-based index at which to start the new string.
     * @param count The number of characters to include following `pos` in the new string.
     * Automatically clamped to the available size
     *
     * @throws std::out_of_range if `pos` is greater than this->size()
     */
    bsoncxx_cxx14_constexpr self_type substr(size_type pos, size_type count = npos) const {
        if (pos > size()) {
            throw std::out_of_range{"bsoncxx::stdx::basic_string_view::substr()"};
        }
        return self_type(_begin + pos, (std::min)(count, size() - pos));
    }

    /**
     * @brief Compare two strings lexicographically
     *
     * @param other The "right hand" operand of the comparison
     * @retval 0 If *this == other
     * @retval n : n < 0 if *this is "less than" other.
     * @retval n : n > 0 if *this is "greater than" other.
     */
    constexpr int compare(self_type other) const noexcept {
        // Another level of indirection to support restricted C++11 constexpr
        return _compare2(Traits::compare(data(), other.data(), (std::min)(size(), other.size())),
                         other);
    }

    /**
     * @brief Compare a substring of *this with `other`
     *
     * @returns substr(po1, count1).compare(other)
     */
    constexpr int compare(size_type pos1, size_type count1, self_type other) const noexcept {
        return substr(pos1, count1).compare(other);
    }

    /**
     * @brief Compare a substring of *this with a substring of `other`
     *
     * @returns substr(pos1, count1).compare(other.substr(pos2, count2))
     */
    constexpr int compare(size_type pos1,
                          size_type count1,
                          self_type other,
                          size_type pos2,
                          size_type count2) const noexcept {
        return substr(pos1, count1).compare(other.substr(pos2, count2));
    }

    /**
     * @brief Compare a substring of *this with a string viewed through the given pointer+size
     *
     * @returns substr(pos1, count1).compare(basic_string_view(str, count2))
     */
    constexpr int compare(size_type pos1,
                          size_type count1,
                          const_pointer str,
                          size_type count2) const noexcept {
        return substr(pos1, count1).compare(self_type(str, count2));
    }

    /**
     * @brief Find the zero-based index of the left-most occurrence of the given substring
     */
    bsoncxx_cxx14_constexpr size_type find(self_type infix, size_type pos = 0) const noexcept {
        self_type sub = this->substr((std::min)(pos, size()));
        bsoncxx::detail::subrange<iterator> found = bsoncxx::detail::search(sub, infix);
        if (bsoncxx::detail::distance(found) != static_cast<difference_type>(infix.size())) {
            return npos;
        }
        return _iter_to_pos(found.begin());
    }

    /**
     * @brief Find the zero-based index of the right-most occurrence of the given substring
     */
    bsoncxx_cxx14_constexpr size_type rfind(self_type infix, size_type pos = npos) const noexcept {
        self_type sub = this->substr(0, pos);
        bsoncxx::detail::reversed_t<self_type> found = bsoncxx::detail::search(
            bsoncxx::detail::make_reversed_view(sub), bsoncxx::detail::make_reversed_view(infix));
        if (bsoncxx::detail::distance(found) != static_cast<difference_type>(infix.size())) {
            return npos;
        }
        return _iter_to_pos(found.end());
    }

    /**
     * @brief Find the zero-based index of the left-most occurrence of any character of the given
     * set
     */
    constexpr size_type find_first_of(self_type set, size_type pos = 0) const noexcept {
        return _find_if(pos, bsoncxx::detail::equal_to_any_of(set));
    }

    /**
     * @brief Find the zero-based index of the right-most occurrence of any character of the given
     * set
     */
    constexpr size_type find_last_of(self_type set, size_type pos = npos) const noexcept {
        return _rfind_if(pos, bsoncxx::detail::equal_to_any_of(set));
    }

    /**
     * @brief Find the zero-based index of the left-most occurrence of any character that
     * is NOT a member of the given set of characters
     */
    constexpr size_type find_first_not_of(self_type set, size_type pos = 0) const noexcept {
        return _find_if(pos, bsoncxx::detail::not_fn(bsoncxx::detail::equal_to_any_of(set)));
    }

    /**
     * @brief Find the zero-based index of the right-most occurrence of any character that
     * is NOT a member of the given set of characters
     */
    constexpr size_type find_last_not_of(self_type set, size_type pos = npos) const noexcept {
        return _rfind_if(pos, bsoncxx::detail::not_fn(bsoncxx::detail::equal_to_any_of(set)));
    }

#pragma push_macro("DECL_FINDERS")
#undef DECL_FINDERS
#define DECL_FINDERS(Name, DefaultPos)                                                            \
    constexpr size_type Name(value_type chr, size_type pos = DefaultPos) const noexcept {         \
        return Name(self_type(&chr, 1), pos);                                                     \
    }                                                                                             \
    constexpr size_type Name(const_pointer cstr, size_type pos, size_type count) const noexcept { \
        return Name(self_type(cstr, count), pos);                                                 \
    }                                                                                             \
    constexpr size_type Name(const_pointer cstr, size_type pos = DefaultPos) const noexcept {     \
        return Name(self_type(cstr), pos);                                                        \
    }                                                                                             \
    BSONCXX_FORCE_SEMICOLON
    DECL_FINDERS(find, 0);
    DECL_FINDERS(rfind, npos);
    DECL_FINDERS(find_first_of, 0);
    DECL_FINDERS(find_last_of, npos);
    DECL_FINDERS(find_first_not_of, 0);
    DECL_FINDERS(find_last_not_of, npos);
#pragma pop_macro("DECL_FINDERS")

    /**
     * @brief Test whether the string starts-with the given prefix string
     */
    constexpr bool starts_with(self_type pfx) const noexcept {
        return pfx == substr(0, pfx.size());
    }

    /**
     * @brief Test whether the string ends-with the given suffix string
     */
    constexpr bool ends_with(self_type sfx) const noexcept {
        return size() >= sfx.size() && substr(size() - sfx.size()) == sfx;
    }

    /**
     * @brief Test whether the string contains any occurrence of the given substring
     */
    constexpr bool contains(self_type infix) const noexcept {
        return find(infix) != npos;
    }

    constexpr bool contains(value_type chr) const noexcept {
        return contains(string_view(&chr, 1));
    }

    constexpr bool contains(const_pointer cstr) const noexcept {
        return contains(self_type(cstr));
    }

    /**
     * @brief Explicit-conversion to a std::basic_string
     */
    template <typename Allocator>
    constexpr explicit operator std::basic_string<Char, Traits, Allocator>() const {
        return std::basic_string<Char, Traits, Allocator>(data(), size());
    }

   private:
    // Additional level-of-indirection for constexpr compare()
    constexpr int _compare2(int diff, self_type other) const noexcept {
        // "diff" is the diff according to Traits::cmp
        return diff ? diff : static_cast<int>(size() - other.size());
    }

    // Implementation of equality comparison
    constexpr friend bool tag_invoke(bsoncxx::detail::equal_to,
                                     self_type left,
                                     self_type right) noexcept {
        return left.size() == right.size() && left.compare(right) == 0;
    }

    // Implementation of a three-way-comparison
    constexpr friend bsoncxx::detail::strong_ordering tag_invoke(
        bsoncxx::detail::compare_three_way cmp, self_type left, self_type right) noexcept {
        return cmp(left.compare(right), 0);
    }

    friend std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
                                                        self_type self) {
        out.write(self.data(), static_cast<std::streamsize>(self.size()));
        return out;
    }

    // Find the first index I where the given predicate returns true for substr(I)
    template <typename F>
    bsoncxx_cxx14_constexpr size_type _find_if(size_type pos, F pred) const noexcept {
        const iterator found = bsoncxx::detail::find_if(substr(pos), pred);
        if (found == end()) {
            return npos;
        }
        return _iter_to_pos(found);
    }

    // Find the last index I where the given predicate returns true for substr(0, I)
    template <typename F>
    bsoncxx_cxx14_constexpr size_type _rfind_if(size_type pos, F pred) const noexcept {
        const const_reverse_iterator found =
            bsoncxx::detail::find_if(bsoncxx::detail::make_reversed_view(substr(0, pos)), pred);
        if (found == rend()) {
            return npos;
        }
        return _iter_to_pos(found);
    }

    // Convert an iterator to a zero-based index
    constexpr size_type _iter_to_pos(const_iterator it) const noexcept {
        return static_cast<size_type>(it - begin());
    }

    // Convert a reverse-iterator to a zero-based index
    constexpr size_type _iter_to_pos(const_reverse_iterator it) const noexcept {
        return static_cast<size_type>(rend() - it);
    }
};

// Required to define this here for C++≤14 compatibility. Can be removed in C++≥17
template <typename C, typename Tr>
const std::size_t basic_string_view<C, Tr>::npos;

using string_view = basic_string_view<char>;

}  // namespace stdx
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
