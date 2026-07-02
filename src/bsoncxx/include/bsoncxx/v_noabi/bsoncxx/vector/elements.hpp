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

#include <bsoncxx/vector/elements-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/detail/bit.hpp>

#include <cstdint>
#include <cstring>

#include <bsoncxx/vector/iterators-fwd.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace elements {

/// @brief A 32-bit float value in packed little-endian format
class float32 {
    template <bsoncxx::detail::endian = bsoncxx::detail::endian::native>
    void construct(float value);

    template <bsoncxx::detail::endian = bsoncxx::detail::endian::native>
    float convert() const;

   public:
    /// @brief Construct a packed little-endian value from a float input in the local byte order.
    /// @param value Floating point value to convert
    float32(float value) noexcept;

    /// Convert a packed little-endian floating point value back to the local byte order.
    operator float() const noexcept;

    /// Operator +=, emulating normal float behavior
    float32& operator+=(float const& other) noexcept {
        return *this = *this + other;
    }

    /// Operator -=, emulating normal float behavior
    float32& operator-=(float const& other) noexcept {
        return *this = *this - other;
    }

    /// Operator *=, emulating normal float behavior
    float32& operator*=(float const& other) noexcept {
        return *this = *this * other;
    }

    /// Operator /=, emulating normal float behavior
    float32& operator/=(float const& other) noexcept {
        return *this = *this / other;
    }

   private:
    std::uint8_t bytes[4];
};

template <>
inline void float32::construct<bsoncxx::detail::endian::little>(float value) {
    std::memcpy(bytes, &value, sizeof value);
}

template <>
inline void float32::construct<bsoncxx::detail::endian::big>(float value) {
    auto const ptr = reinterpret_cast<unsigned char const*>(&value);
    bytes[0] = ptr[3];
    bytes[1] = ptr[2];
    bytes[2] = ptr[1];
    bytes[3] = ptr[0];
}

template <>
inline float float32::convert<bsoncxx::detail::endian::little>() const {
    float value;
    std::memcpy(&value, bytes, sizeof value);
    return value;
}

template <>
inline float float32::convert<bsoncxx::detail::endian::big>() const {
    float value;
    auto const ptr = reinterpret_cast<unsigned char*>(&value);
    ptr[0] = bytes[3];
    ptr[1] = bytes[2];
    ptr[2] = bytes[1];
    ptr[3] = bytes[0];
    return value;
}

inline float32::float32(float value) noexcept {
    this->construct(value);
}

inline float32::operator float() const noexcept {
    return this->convert();
}

/// @brief Reference to a single element in a packed_bit vector.
/// @tparam Iterator Underlying byte iterator type, optionally const.
template <typename Iterator>
class packed_bit_element {
   public:
    /// Value type chosen to represent a single-bit element
    using value_type = bool;

    /// Obtain the referenced element's current value
    constexpr operator value_type() const {
        return ((*byte & mask) == UINT8_C(0)) ? value_type(0) : value_type(1);
    }

    /// Set the value of the element referenced
    packed_bit_element const& operator=(value_type const& v) const {
        if (v == 0) {
            *byte &= std::uint8_t(~mask);
        } else {
            *byte |= mask;
        }
        return *this;
    }

    /// Copy the referenced value from another reference of the same type
    packed_bit_element const& operator=(packed_bit_element const& v) const noexcept {
        return *this = value_type{v};
    }

    /// Operator ^=, emulating bool reference behavior
    packed_bit_element const& operator^=(value_type const& other) const noexcept {
        return *this = *this ^ other;
    }

    /// Operator &=, emulating bool reference behavior
    packed_bit_element const& operator&=(value_type const& other) const noexcept {
        return *this = *this & other;
    }

    /// Operator |=, emulating bool reference behavior
    packed_bit_element const& operator|=(value_type const& other) const noexcept {
        return *this = *this | other;
    }

    constexpr packed_bit_element(packed_bit_element const& other) : byte(other.byte), mask(other.mask) {}

   private:
    friend class iterators::packed_bit_element<Iterator>;

    constexpr packed_bit_element(Iterator byte_iter, uint8_t bit_index) noexcept
        : byte{byte_iter}, mask{uint8_t(0x80u >> bit_index)} {}

    Iterator byte;
    std::uint8_t mask;
};

/// packed_bit_element is Swappable even when it's not an lvalue reference
template <typename Iterator>
void swap(packed_bit_element<Iterator> a, packed_bit_element<Iterator> b) noexcept {
    bool a_value = a;
    bool b_value = b;
    a = b_value;
    b = a_value;
}

/// @brief Reference to a byte or partial byte within a vector of packed_bit elements.
/// Allows access to each byte as a uint8_t, while masking off writes to reserved bits.
/// @tparam Iterator Underlying byte iterator type, optionally const.
template <typename Iterator>
class packed_bit_byte {
   public:
    /// @brief Read the entire byte, as a std::uint8_t.
    constexpr operator std::uint8_t() const noexcept {
        return *byte;
    }

    /// @brief Overwrite the usable portion of the byte, and set reserved bits to zero.
    /// @param v Byte to write. Reserved bits are ignored.
    /// @return *this
    packed_bit_byte const& operator=(std::uint8_t const& v) const noexcept {
        *byte = v & mask;
        return *this;
    }

    /// Copy the referenced value from another reference of the same type
    packed_bit_byte const& operator=(packed_bit_byte const& v) const noexcept {
        return *this = std::uint8_t(v);
    }

    /// Operator +=, emulating number reference behavior
    packed_bit_byte const& operator+=(std::uint8_t const& other) const noexcept {
        return *this = std::uint8_t(*this + other);
    }

    /// Operator -=, emulating number reference behavior
    packed_bit_byte const& operator-=(std::uint8_t const& other) const noexcept {
        return *this = std::uint8_t(*this - other);
    }

    /// Operator *=, emulating number reference behavior
    packed_bit_byte const& operator*=(std::uint8_t const& other) const noexcept {
        return *this = std::uint8_t(*this * other);
    }

    /// Operator /=, emulating number reference behavior
    packed_bit_byte const& operator/=(std::uint8_t const& other) const noexcept {
        return *this = *this / other;
    }

    /// Operator %=, emulating number reference behavior
    packed_bit_byte const& operator%=(std::uint8_t const& other) const noexcept {
        return *this = *this % other;
    }

    /// Operator ^=, emulating number reference behavior
    packed_bit_byte const& operator^=(std::uint8_t const& other) const noexcept {
        return *this = *this ^ other;
    }

    /// Operator &=, emulating number reference behavior
    packed_bit_byte const& operator&=(std::uint8_t const& other) const noexcept {
        return *this = *this & other;
    }

    /// Operator |=, emulating number reference behavior
    packed_bit_byte const& operator|=(std::uint8_t const& other) const noexcept {
        return *this = *this | other;
    }

    /// Operator <<=, emulating number reference behavior
    packed_bit_byte const& operator<<=(unsigned other) const noexcept {
        return *this = *this << other;
    }

    /// Operator >>=, emulating number reference behavior
    packed_bit_byte const& operator>>=(unsigned other) const noexcept {
        return *this = *this >> other;
    }

    constexpr packed_bit_byte(packed_bit_byte const& other) : byte{other.byte}, mask{other.mask} {}

   private:
    friend class iterators::packed_bit_byte<Iterator>;

    constexpr packed_bit_byte(Iterator byte_iter, uint8_t byte_mask) noexcept : byte{byte_iter}, mask{byte_mask} {}

    Iterator byte;
    std::uint8_t mask;
};

/// Swap the referenced values for `a` and `b`.
///
/// @note `packed_bit_byte<Iterator>` is a proxy reference and behaves like an lvalue reference.
template <typename Iterator>
void swap(packed_bit_byte<Iterator> a, packed_bit_byte<Iterator> b) noexcept {
    std::uint8_t a_value = a;
    std::uint8_t b_value = b;
    a = b_value;
    b = a_value;
}

} // namespace elements
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities in @ref bsoncxx::v_noabi::vector::elements.
///
