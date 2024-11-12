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

#include <bsoncxx/config/prelude.hpp>

#if defined(BSONCXX_POLY_USE_STD)

#include <string_view>

namespace bsoncxx {
namespace v_noabi {
namespace stdx {

using ::std::basic_string_view;
using ::std::string_view;

}  // namespace stdx
}  // namespace v_noabi
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_IMPLS)

#include <algorithm>
#include <cstddef>
#include <ios>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/type_traits.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace stdx {

template <typename Char, typename Traits = std::char_traits<Char>>
class basic_string_view : bsoncxx::detail::equality_operators, bsoncxx::detail::ordering_operators {
   public:
    using pointer = Char*;
    using const_pointer = const Char*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = Char;

    // Constant sentinel value to represent an impossible/invalid string position.
    static constexpr size_type npos = static_cast<size_type>(-1);

   private:
    // Pointer to the beginning of the string being viewed.
    const_pointer _begin = nullptr;
    // The size of the array that is being viewed via `_begin`.
    size_type _size = 0;

   public:
    using traits_type = Traits;
    using reference = Char&;
    using const_reference = const Char&;
    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    constexpr basic_string_view() noexcept = default;
    constexpr basic_string_view(const basic_string_view&) noexcept = default;
    bsoncxx_cxx14_constexpr basic_string_view& operator=(const basic_string_view&) noexcept =
        default;

    constexpr basic_string_view(const_pointer s, size_type count) : _begin(s), _size(count) {}

    constexpr basic_string_view(const_pointer s) : _begin(s), _size(traits_type::length(s)) {}

    template <typename Alloc>
    constexpr basic_string_view(
        const std::basic_string<value_type, traits_type, Alloc>& str) noexcept
        : _begin(str.data()), _size(str.size()) {}

#if defined(__cpp_lib_string_view)
    constexpr basic_string_view(std::basic_string_view<value_type, traits_type> sv) noexcept
        : _begin(sv.data()), _size(sv.size()) {}
#endif

    basic_string_view(std::nullptr_t) = delete;

    constexpr const_iterator begin() const noexcept {
        return const_iterator(_begin);
    }
    constexpr const_iterator end() const noexcept {
        return begin() + size();
    }
    constexpr const_iterator cbegin() const noexcept {
        return begin();
    }
    constexpr const_iterator cend() const noexcept {
        return end();
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{cend()};
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{crbegin()};
    }

    constexpr const_reference operator[](size_type offset) const {
        return _begin[offset];
    }

    bsoncxx_cxx14_constexpr const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range{"bsoncxx::stdx::basic_string_view::at()"};
        }
        return _begin[pos];
    }

    constexpr const_reference front() const {
        return (*this)[0];
    }

    constexpr const_reference back() const {
        return (*this)[size() - 1];
    }

    constexpr const_pointer data() const noexcept {
        return _begin;
    }

    constexpr size_type size() const noexcept {
        return _size;
    }

    constexpr size_type length() const noexcept {
        return size();
    }

    constexpr bool empty() const noexcept {
        return size() == 0;
    }

    constexpr size_type max_size() const noexcept {
        return static_cast<size_type>(std::numeric_limits<difference_type>::max());
    }

    bsoncxx_cxx14_constexpr void remove_prefix(size_type n) {
        _begin += n;
        _size -= n;
    }

    bsoncxx_cxx14_constexpr void remove_suffix(size_type n) {
        _size -= n;
    }

    bsoncxx_cxx14_constexpr void swap(basic_string_view& other) {
        std::swap(_begin, other._begin);
        std::swap(_size, other._size);
    }

    size_type copy(pointer dest, size_type count, size_type pos = 0) const {
        if (pos > size()) {
            throw std::out_of_range{"bsoncxx::stdx::basic_string_view::substr()"};
        }
        count = (std::min)(count, size() - pos);
        Traits::copy(dest, data() + pos, count);
        return count;
    }

    bsoncxx_cxx14_constexpr basic_string_view substr(size_type pos = 0,
                                                     size_type count = npos) const {
        if (pos > size()) {
            throw std::out_of_range{"bsoncxx::stdx::basic_string_view::substr()"};
        }
        return basic_string_view(_begin + pos, (std::min)(count, size() - pos));
    }

    constexpr int compare(basic_string_view other) const noexcept {
        // Another level of indirection to support restricted C++11 constexpr.
        return _compare2(Traits::compare(data(), other.data(), (std::min)(size(), other.size())),
                         other);
    }

    constexpr int compare(const_pointer cstr) const {
        return compare(basic_string_view(cstr));
    }

    constexpr int compare(size_type pos1, size_type count1, basic_string_view other) const {
        return substr(pos1, count1).compare(other);
    }

    constexpr int compare(size_type pos1, size_type count1, const_pointer cstr) const {
        return compare(pos1, count1, basic_string_view(cstr));
    }

    constexpr int compare(size_type pos1,
                          size_type count1,
                          basic_string_view other,
                          size_type pos2,
                          size_type count2) const {
        return substr(pos1, count1).compare(other.substr(pos2, count2));
    }

    constexpr int compare(size_type pos1,
                          size_type count1,
                          const_pointer str,
                          size_type count2) const {
        return substr(pos1, count1).compare(basic_string_view(str, count2));
    }

    bsoncxx_cxx14_constexpr size_type find(basic_string_view infix, size_type pos = 0) const
        noexcept {
        if (pos > size()) {
            return npos;
        }
        basic_string_view sub = this->substr(pos);
        if (infix.empty()) {
            // The empty string is always "present" at the beginning of any string.
            return pos;
        }
        const_iterator found = std::search(sub.begin(), sub.end(), infix.begin(), infix.end());
        if (found == sub.end()) {
            return npos;
        }
        return static_cast<size_type>(found - begin());
    }

    bsoncxx_cxx14_constexpr size_type rfind(basic_string_view infix, size_type pos = npos) const
        noexcept {
        // Calc the endpos where searching should begin, which includes the infix size.
        const size_type substr_size = pos != npos ? pos + infix.size() : pos;
        if (infix.empty()) {
            return (std::min)(pos, size());
        }
        basic_string_view searched = this->substr(0, substr_size);
        auto f = std::search(searched.rbegin(), searched.rend(), infix.rbegin(), infix.rend());
        if (f == searched.rend()) {
            return npos;
        }
        return static_cast<size_type>(rend() - f) - infix.size();
    }

    constexpr size_type find_first_of(basic_string_view set, size_type pos = 0) const noexcept {
        return _find_if(pos, [&](value_type chr) { return set.find(chr) != npos; });
    }

    constexpr size_type find_last_of(basic_string_view set, size_type pos = npos) const noexcept {
        return _rfind_if(pos, [&](value_type chr) { return set.find(chr) != npos; });
    }

    constexpr size_type find_first_not_of(basic_string_view set, size_type pos = 0) const noexcept {
        return _find_if(pos, [&](value_type chr) { return set.find(chr) == npos; });
    }

    constexpr size_type find_last_not_of(basic_string_view set, size_type pos = npos) const
        noexcept {
        return _rfind_if(pos, [&](value_type chr) { return set.find(chr) == npos; });
    }

#pragma push_macro("DECL_FINDERS")
#undef DECL_FINDERS
#define DECL_FINDERS(Name, DefaultPos)                                                    \
    constexpr size_type Name(value_type chr, size_type pos = DefaultPos) const noexcept { \
        return Name(basic_string_view(&chr, 1), pos);                                     \
    }                                                                                     \
    constexpr size_type Name(const_pointer cstr, size_type pos, size_type count) const {  \
        return Name(basic_string_view(cstr, count), pos);                                 \
    }                                                                                     \
    constexpr size_type Name(const_pointer cstr, size_type pos = DefaultPos) const {      \
        return Name(basic_string_view(cstr), pos);                                        \
    }                                                                                     \
    BSONCXX_FORCE_SEMICOLON

    DECL_FINDERS(find, 0);
    DECL_FINDERS(rfind, npos);
    DECL_FINDERS(find_first_of, 0);
    DECL_FINDERS(find_last_of, npos);
    DECL_FINDERS(find_first_not_of, 0);
    DECL_FINDERS(find_last_not_of, npos);
#pragma pop_macro("DECL_FINDERS")

    // Explicit-conversion to a std::basic_string.
    template <typename Allocator>
    explicit operator std::basic_string<Char, Traits, Allocator>() const {
        return std::basic_string<Char, Traits, Allocator>(data(), size());
    }

#if defined(__cpp_lib_string_view)
    explicit operator std::basic_string_view<value_type, traits_type>() const noexcept {
        return std::basic_string_view<value_type, traits_type>(data(), size());
    }
#endif

   private:
    // Additional level-of-indirection for constexpr compare().
    constexpr int _compare2(int diff, basic_string_view other) const noexcept {
        // "diff" is the diff according to Traits::cmp
        return diff ? diff : static_cast<int>(size() - other.size());
    }

    // Implementation of equality comparison.
    constexpr friend bool tag_invoke(bsoncxx::detail::equal_to,
                                     basic_string_view left,
                                     basic_string_view right) noexcept {
        return left.size() == right.size() && left.compare(right) == 0;
    }

    // Implementation of a three-way-comparison.
    constexpr friend bsoncxx::detail::strong_ordering tag_invoke(
        bsoncxx::detail::compare_three_way cmp,
        basic_string_view left,
        basic_string_view right) noexcept {
        return cmp(left.compare(right), 0);
    }

    friend std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
                                                        basic_string_view self) {
        out << std::basic_string<Char, Traits>(self);
        return out;
    }

    // Find the first in-bounds index I in [pos, size()) where the given predicate
    // returns true for substr(I). If no index exists, returns npos.
    template <typename F>
    bsoncxx_cxx14_constexpr size_type _find_if(size_type pos, F pred) const noexcept {
        const auto sub = substr(pos);
        const iterator found = std::find_if(sub.begin(), sub.end(), pred);
        if (found == end()) {
            return npos;
        }
        return static_cast<size_type>(found - begin());
    }

    // Find the LAST index I in [0, pos] where the given predicate returns true for
    // substr(0, I). If no such index exists, returns npos.
    template <typename F>
    bsoncxx_cxx14_constexpr size_type _rfind_if(size_type pos, F pred) const noexcept {
        // Adjust 'pos' for an inclusive range in substr()
        const auto rpos = pos == npos ? npos : pos + 1;
        // The substring that will be searched:
        const auto prefix = substr(0, rpos);
        const const_reverse_iterator found = std::find_if(prefix.rbegin(), prefix.rend(), pred);
        if (found == rend()) {
            return npos;
        }
        // Adjust by 1 to account for reversed-ness
        return static_cast<size_type>(rend() - found) - 1u;
    }
};

// Required to define this here for compatibility with C++14 and older. Can be removed in C++17 or
// newer.
template <typename C, typename Tr>
constexpr std::size_t basic_string_view<C, Tr>::npos;

using string_view = basic_string_view<char>;

}  // namespace stdx
}  // namespace v_noabi
}  // namespace bsoncxx

namespace std {

template <typename CharT, typename Traits>
struct hash<bsoncxx::v_noabi::stdx::basic_string_view<CharT, Traits>>
    : private std::hash<std::basic_string<CharT, Traits>> {
    std::size_t operator()(
        const bsoncxx::v_noabi::stdx::basic_string_view<CharT, Traits>& str) const {
        return std::hash<std::basic_string<CharT, Traits>>::operator()(
            std::basic_string<CharT, Traits>(str.data(), str.size()));
    }
};

}  // namespace std

#else
#error "Cannot find a valid polyfill for string_view"
#endif

#include <bsoncxx/config/postlude.hpp>

namespace bsoncxx {
namespace stdx {

using ::bsoncxx::v_noabi::stdx::basic_string_view;
using ::bsoncxx::v_noabi::stdx::string_view;

}  // namespace stdx
}  // namespace bsoncxx

///
/// @file
/// Provides `std::string_view`-related polyfills for library API usage.
///
/// @note The API and ABI compatibility of this polyfill is determined by polyfill build
/// configuration variables and the `BSONCXX_POLY_USE_*` macros provided by @ref
/// bsoncxx-v_noabi-bsoncxx-config-config-hpp.
///
/// @see
/// - [Choosing a C++17 Polyfill](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/polyfill-selection/)
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace v_noabi {
namespace stdx {

///
/// A polyfill for `std::string_view`.
///
/// @note The API and ABI compatibility of this polyfill is determined by polyfill build
/// configuration variables and the `BSONCXX_POLY_USE_*` macros provided by @ref
/// bsoncxx-v_noabi-bsoncxx-config-config-hpp.
///
class string_view {};

}  // namespace stdx
}  // namespace v_noabi
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
