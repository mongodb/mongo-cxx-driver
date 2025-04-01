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

#include <bsoncxx/vector/iterators-fwd.hpp>

//

#include <cstdint>
#include <iterator>

#include <bsoncxx/vector/accessor-fwd.hpp>
#include <bsoncxx/vector/detail-fwd.hpp>

#include <bsoncxx/vector/elements.hpp>
#include <bsoncxx/vector/formats.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace iterators {

/// @brief Iterator for elements within a packed_bit vector
/// @tparam Iterator Underlying byte iterator type
template <typename Iterator>
class packed_bit_element {
   public:
    /// Values pointed to by this iterator are single bits represented by bool.
    using value_type = bool;

    /// References to individual bits are each bsoncxx::v_noabi::elements::packed_bit_element.
    using reference = elements::packed_bit_element<Iterator> const;

    /// Element pointers aren't really a useful concept here, but this is defined for compatibility with standard
    /// random-access iterators.
    using pointer = elements::packed_bit_element<Iterator> const*;

    /// This is a standard random-access iterator.
    using iterator_category = std::random_access_iterator_tag;

    /// A signed bit count
    using difference_type = std::ptrdiff_t;

    /// @brief Dereference this bit iterator into a bit reference.
    /// @return An individual bit reference, as a bsoncxx::v_noabi::elements::packed_bit_element.
    constexpr reference operator*() const noexcept {
        return {byte, bit};
    }

    /// Compare two bit iterators
    constexpr bool operator==(packed_bit_element const& other) const noexcept {
        return byte == other.byte && bit == other.bit;
    }

    /// Compare two bit iterators
    constexpr bool operator!=(packed_bit_element const& other) const noexcept {
        return byte != other.byte || bit != other.bit;
    }

    /// Compare two bit iterators
    constexpr bool operator<(packed_bit_element const& other) const noexcept {
        return byte < other.byte || (byte == other.byte && bit < other.bit);
    }

    /// Compare two bit iterators
    constexpr bool operator<=(packed_bit_element const& other) const noexcept {
        return byte < other.byte || (byte == other.byte && bit <= other.bit);
    }

    /// Compare two bit iterators
    constexpr bool operator>(packed_bit_element const& other) const noexcept {
        return byte > other.byte || (byte == other.byte && bit > other.bit);
    }

    /// Compare two bit iterators
    constexpr bool operator>=(packed_bit_element const& other) const noexcept {
        return byte > other.byte || (byte == other.byte && bit >= other.bit);
    }

    /// @brief Calculate a signed addition of this iterator with a ptrdiff_t, moving it forward or backward the
    /// indicated number of bits.
    /// If the iterator goes out of range, behavior is undefined.
    constexpr packed_bit_element operator+(difference_type const& other) const noexcept {
        return {
            byte + ((difference_type{bit} + other - ((difference_type{bit} + other) & 7)) / 8),
            std::uint8_t((difference_type{bit} + other) & 7)};
    }

    /// @brief Calculate a signed subtraction of a ptrdiff_t from this iterator, moving it backward or forward the
    /// indicated number of bits.
    /// If the iterator goes out of range, behavior is undefined.
    constexpr packed_bit_element operator-(difference_type const& other) const noexcept {
        return *this + (-other);
    }

    /// @brief Calculate the difference in position between two bit iterators
    /// If the two iterators do not point into the same vector, behavior is undefined.
    constexpr difference_type operator-(packed_bit_element const& other) const noexcept {
        return {(byte - other.byte) * 8 + (difference_type{bit} - difference_type{other.bit})};
    }

    /// Advance this iterator forward by the indicated number of bits
    packed_bit_element& operator+=(difference_type const& other) noexcept {
        return *this = *this + other;
    }

    /// Move this iterator backward by the indicated number of bits
    packed_bit_element& operator-=(difference_type const& other) noexcept {
        return *this = *this - other;
    }

    /// Pre-increment
    packed_bit_element& operator++() noexcept {
        return *this += difference_type{1};
    }

    /// Pre-decrement
    packed_bit_element& operator--() noexcept {
        return *this -= difference_type{1};
    }

    /// Post-increment
    packed_bit_element operator++(int) noexcept {
        packed_bit_element prev = *this;
        ++*this;
        return prev;
    }

    /// Post-decrement
    packed_bit_element operator--(int) noexcept {
        packed_bit_element prev = *this;
        --*this;
        return prev;
    }

   private:
    friend class packed_bit_byte<Iterator>;
    friend class accessor<formats::f_packed_bit>;
    friend class accessor<formats::f_packed_bit const>;

    constexpr packed_bit_element(Iterator byte_iter, std::uint8_t bit_index = 0) noexcept
        : byte{byte_iter}, bit{bit_index} {}

    Iterator byte;
    std::uint8_t bit;
};

/// @brief Iterator for bytes within a packed_bit vector
/// @tparam Iterator Underlying byte iterator type
template <typename Iterator>
class packed_bit_byte {
   public:
    /// Values pointed to by this iterator are unsigned bytes.
    using value_type = std::uint8_t;

    /// References to individual bytes are each bsoncxx::v_noabi::elements::packed_bit_byte, to protect the validity of
    /// bytes with reserved portions.
    using reference = elements::packed_bit_byte<Iterator> const;

    /// Element pointers aren't really a useful concept here, but this is defined for compatibility with standard
    /// random-access iterators.
    using pointer = elements::packed_bit_byte<Iterator> const*;

    /// This is a standard random-access iterator.
    using iterator_category = std::random_access_iterator_tag;

    /// A signed byte count
    using difference_type = std::ptrdiff_t;

    /// @brief Dereference the byte iterator
    /// @return A bsoncxx::v_noabi::elements::packed_bit_byte that can be used like a byte reference.
    constexpr reference operator*() const noexcept {
        return {byte, (byte + 1) == byte_end ? last_byte_mask : value_type{0xFFu}};
    }

    /// Compare two byte iterators
    constexpr bool operator==(packed_bit_byte const& other) const noexcept {
        return byte == other.byte;
    }

    /// Compare two byte iterators
    constexpr bool operator!=(packed_bit_byte const& other) const noexcept {
        return byte != other.byte;
    }

    /// Compare two byte iterators
    constexpr bool operator<(packed_bit_byte const& other) const noexcept {
        return byte < other.byte;
    }

    /// Compare two byte iterators
    constexpr bool operator<=(packed_bit_byte const& other) const noexcept {
        return byte <= other.byte;
    }

    /// Compare two byte iterators
    constexpr bool operator>(packed_bit_byte const& other) const noexcept {
        return byte > other.byte;
    }

    /// Compare two byte iterators
    constexpr bool operator>=(packed_bit_byte const& other) const noexcept {
        return byte >= other.byte;
    }

    /// @brief Calculate a signed addition of this iterator with a ptrdiff_t, moving it forward or backward the
    /// indicated number of bytes.
    /// If the iterator goes out of range, behavior is undefined.
    constexpr packed_bit_byte operator+(difference_type const& other) const noexcept {
        return {byte + other, byte_end, last_byte_mask};
    }

    /// @brief Calculate a signed subtraction of a ptrdiff_t from this iterator, moving it backward or forward the
    /// indicated number of bytes.
    /// If the iterator goes out of range, behavior is undefined.
    constexpr packed_bit_byte operator-(difference_type const& other) const noexcept {
        return *this + (-other);
    }

    /// @brief Calculate the difference in position between two byte iterators
    /// If the two iterators do not point into the same vector, behavior is undefined.
    constexpr difference_type operator-(packed_bit_byte const& other) const noexcept {
        return {byte - other.byte};
    }

    /// Advance this iterator forward by the indicated number of bytes
    packed_bit_byte& operator+=(difference_type const& other) noexcept {
        return *this = *this + other;
    }

    /// Move this iterator backward by the indicated number of bytes
    packed_bit_byte& operator-=(difference_type const& other) noexcept {
        return *this = *this - other;
    }

    /// Pre-increment
    packed_bit_byte& operator++() noexcept {
        return *this += difference_type{1};
    }

    /// Pre-decrement
    packed_bit_byte& operator--() noexcept {
        return *this -= difference_type{1};
    }

    /// Post-increment
    packed_bit_byte operator++(int) noexcept {
        packed_bit_byte prev = *this;
        ++*this;
        return prev;
    }

    /// Post-decrement
    packed_bit_byte operator--(int) noexcept {
        packed_bit_byte prev = *this;
        --*this;
        return prev;
    }

   private:
    friend struct detail::format_traits<formats::f_packed_bit>;
    friend struct detail::format_traits<formats::f_packed_bit const>;

    constexpr packed_bit_byte(packed_bit_element<Iterator> element, packed_bit_element<Iterator> element_end)
        : byte{element.byte},
          byte_end{(element_end + 7u).byte},
          last_byte_mask{value_type(0xFFu << (-element_end.bit & 7u))} {}

    constexpr packed_bit_byte(Iterator byte, Iterator byte_end, value_type last_byte_mask)
        : byte{byte}, byte_end{byte_end}, last_byte_mask{last_byte_mask} {}

    Iterator byte;
    Iterator byte_end;
    value_type last_byte_mask;
};

} // namespace iterators
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities in @ref bsoncxx::v_noabi::vector::iterators.
///
