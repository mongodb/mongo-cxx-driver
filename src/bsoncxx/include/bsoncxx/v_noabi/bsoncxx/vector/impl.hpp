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

#include <bsoncxx/types.hpp>
#include <bsoncxx/vector/elements.hpp>
#include <bsoncxx/vector/formats.hpp>
#include <bsoncxx/vector/iterators.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace impl {

/// @brief Implementation detail. A copy of the validated BSON Binary Vector header, included in each view.
struct header {
    std::uint8_t bytes[2];
};

/// @brief Implementation detail. Common data for each view type.
/// @tparam Format One of the @ref bsoncxx::v_noabi::vector::formats types.
template <typename Format>
struct view_data {
    using byte_type = typename std::conditional<std::is_const<Format>::value, std::uint8_t const, std::uint8_t>::type;
    using byte_count_type = std::uint32_t;

    byte_type* bytes;
    byte_count_type size;
    header header_copy;
};

/// @brief Implementation detail. Default format traits.
struct format_traits_base {
    using element_count_type = std::size_t;

    using byte_difference_type = std::ptrdiff_t;
    using element_difference_type = std::ptrdiff_t;

    using byte_reference = std::uint8_t&;
    using const_byte_reference = std::uint8_t const&;

    using byte_iterator = std::uint8_t*;
    using const_byte_iterator = std::uint8_t const*;
};

/// @brief Implementation detail. Format traits, specialized by format.
/// @tparam Format One of the @ref bsoncxx::v_noabi::vector::formats types.
template <typename Format>
struct format_traits;

/// @brief Implementation detail. Format traits for bsoncxx::v_noabi::vector::formats::f_int8.
template <>
struct format_traits<formats::f_int8> : format_traits_base {
    using value_type = std::int8_t;

    using reference = std::int8_t&;
    using const_reference = std::int8_t const&;
    using iterator = std::int8_t*;
    using const_iterator = std::int8_t const*;

    static BSONCXX_ABI_EXPORT_CDECL(uint32_t) length_for_append(std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(header)
    write_frame(uint8_t* binary_data, uint32_t binary_data_length, std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(view_data<formats::f_int8 const>) const_validate(types::b_binary const& binary);

    static constexpr std::size_t element_count(uint32_t binary_data_length, header) noexcept {
        return binary_data_length - sizeof(header::bytes);
    }

    static byte_iterator make_byte_iterator(iterator element, iterator) noexcept {
        return byte_iterator(static_cast<void*>(element));
    }

    static const_byte_iterator make_byte_iterator(const_iterator element, const_iterator) noexcept {
        return const_byte_iterator(static_cast<void const*>(element));
    }
};

/// @brief Implementation detail. Format traits for bsoncxx::v_noabi::vector::formats::f_float32.
template <>
struct format_traits<formats::f_float32> : format_traits_base {
    using value_type = float;

    using reference = elements::float32&;
    using const_reference = elements::float32 const&;
    using iterator = elements::float32*;
    using const_iterator = elements::float32 const*;

    static BSONCXX_ABI_EXPORT_CDECL(uint32_t) length_for_append(std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(header)
    write_frame(uint8_t* binary_data, uint32_t binary_data_length, std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(view_data<formats::f_float32 const>) const_validate(types::b_binary const& binary);

    static constexpr std::size_t element_count(uint32_t binary_data_length, header) noexcept {
        return (binary_data_length - sizeof(header::bytes)) / sizeof(float);
    }

    static byte_iterator make_byte_iterator(iterator element, iterator) noexcept {
        return byte_iterator(static_cast<void*>(element));
    }

    static const_byte_iterator make_byte_iterator(const_iterator element, const_iterator) noexcept {
        return const_byte_iterator(static_cast<void const*>(element));
    }
};

/// @brief Implementation detail. Format traits for bsoncxx::v_noabi::vector::formats::f_packed_bit.
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

    static BSONCXX_ABI_EXPORT_CDECL(uint32_t) length_for_append(std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(header)
    write_frame(uint8_t* binary_data, uint32_t binary_data_length, std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(view_data<formats::f_packed_bit const>) const_validate(
        types::b_binary const& binary);

    static constexpr std::size_t element_count(uint32_t binary_data_length, header hdr) noexcept {
        return std::size_t(binary_data_length - sizeof(hdr.bytes)) * std::size_t(8u) - std::size_t(hdr.bytes[1] & 7u);
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

} // namespace impl
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// For internal use only!
///
