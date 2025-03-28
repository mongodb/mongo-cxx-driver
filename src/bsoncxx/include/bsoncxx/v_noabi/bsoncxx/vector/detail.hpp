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

#include <bsoncxx/vector/detail-fwd.hpp>

//

#include <array>
#include <cstdint>
#include <cstring>

#include <bsoncxx/types.hpp>
#include <bsoncxx/vector/elements.hpp>
#include <bsoncxx/vector/formats.hpp>
#include <bsoncxx/vector/iterators.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace detail {

// Implementation detail. Size of the BSON Binary Vector header, in bytes.
constexpr std::size_t header_size = 2;

// Implementation detail. Type for local copies of the vector header.
typedef std::array<std::uint8_t, header_size> header;

// @brief Implementation detail. Common data for each accessor type.
// @tparam Format One of the @ref bsoncxx::v_noabi::vector::formats types, optionally const.
template <typename Format>
struct accessor_data {
    using byte_type = typename std::conditional<std::is_const<Format>::value, std::uint8_t const, std::uint8_t>::type;
    using byte_count_type = std::uint32_t;

    byte_type* bytes;
    byte_count_type size;
    header header_copy;

    // Construct accessor_data around a b_binary that has already had its subtype and size validated.
    accessor_data(types::b_binary const& binary) : accessor_data{binary.bytes, binary.size} {}

    // Construct accessor_data with an existing header copy
    accessor_data(byte_type* bytes, byte_count_type size, header header_copy)
        : bytes{bytes}, size{size}, header_copy{header_copy} {}

    // Construct accessor_data around binary data that has already been validated, and capture a new header copy.
    accessor_data(byte_type* bytes, byte_count_type size) : bytes{bytes}, size{size} {
        std::memcpy(header_copy.data(), bytes, header_size);
    }
};

// @brief Implementation detail. Default format traits.
struct format_traits_base {
    using element_count_type = std::size_t;

    using byte_difference_type = std::ptrdiff_t;
    using element_difference_type = std::ptrdiff_t;

    using byte_reference = std::uint8_t&;
    using const_byte_reference = std::uint8_t const&;

    using byte_iterator = std::uint8_t*;
    using const_byte_iterator = std::uint8_t const*;
};

// @brief Implementation detail. Format traits, specialized by format.
// @tparam Format One of the @ref bsoncxx::v_noabi::vector::formats types, without qualifiers.
template <typename Format>
struct format_traits;

// @brief Implementation detail. Format traits for bsoncxx::v_noabi::vector::formats::f_int8.
template <>
struct format_traits<formats::f_int8> : format_traits_base {
    using value_type = std::int8_t;

    using reference = std::int8_t&;
    using const_reference = std::int8_t const&;
    using iterator = std::int8_t*;
    using const_iterator = std::int8_t const*;

    static constexpr std::size_t element_count(std::uint32_t binary_data_length, header) noexcept {
        return binary_data_length - header_size;
    }

    static byte_iterator make_byte_iterator(iterator element, iterator) noexcept {
        return byte_iterator(static_cast<void*>(element));
    }

    static const_byte_iterator make_byte_iterator(const_iterator element, const_iterator) noexcept {
        return const_byte_iterator(static_cast<void const*>(element));
    }
};

// @brief Implementation detail. Format traits for bsoncxx::v_noabi::vector::formats::f_float32.
template <>
struct format_traits<formats::f_float32> : format_traits_base {
    using value_type = float;

    using reference = elements::float32&;
    using const_reference = elements::float32 const&;
    using iterator = elements::float32*;
    using const_iterator = elements::float32 const*;

    static constexpr std::size_t element_count(std::uint32_t binary_data_length, header) noexcept {
        return (binary_data_length - header_size) / sizeof(float);
    }

    static byte_iterator make_byte_iterator(iterator element, iterator) noexcept {
        return byte_iterator(static_cast<void*>(element));
    }

    static const_byte_iterator make_byte_iterator(const_iterator element, const_iterator) noexcept {
        return const_byte_iterator(static_cast<void const*>(element));
    }
};

// @brief Implementation detail. Format traits for bsoncxx::v_noabi::vector::formats::f_packed_bit.
template <>
struct format_traits<formats::f_packed_bit> : format_traits_base {
    using value_type = bool;

    using iterator = iterators::packed_bit_element<std::uint8_t*>;
    using const_iterator = iterators::packed_bit_element<std::uint8_t const*>;
    using reference = iterator::reference;
    using const_reference = const_iterator::reference;

    using byte_iterator = iterators::packed_bit_byte<std::uint8_t*>;
    using const_byte_iterator = iterators::packed_bit_byte<std::uint8_t const*>;
    using byte_reference = byte_iterator::reference;
    using const_byte_reference = const_byte_iterator::reference;

    using byte_difference_type = byte_iterator::difference_type;
    using element_difference_type = iterator::difference_type;

    static std::size_t element_count(std::uint32_t binary_data_length, header hdr) noexcept {
        return std::size_t{binary_data_length - header_size} * std::size_t{8u} - std::size_t{hdr[1] & 7u};
    }

    static byte_iterator make_byte_iterator(iterator element, iterator element_end) noexcept {
        return {element, element_end};
    }

    static constexpr const_byte_iterator make_byte_iterator(
        const_iterator element,
        const_iterator element_end) noexcept {
        return {element, element_end};
    }
};

} // namespace detail
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// For internal use only!
///
