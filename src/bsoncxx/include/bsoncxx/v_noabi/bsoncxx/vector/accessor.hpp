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

#include <bsoncxx/vector/accessor-fwd.hpp>

//

#include <type_traits>

#include <bsoncxx/builder/basic/sub_binary-fwd.hpp>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/vector/detail.hpp>
#include <bsoncxx/vector/formats.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {

/// @brief Accessor for the contents of a valid BSON Binary Vector
/// @tparam Format One of the @ref bsoncxx::v_noabi::vector::formats types, optionally with a const qualifier.
///
/// This accessor operates on data formatted for the bsoncxx::v_noabi::binary_sub_type::k_vector BSON binary
/// subtype. A mutable accessor may be constructed only using bsoncxx::v_noabi::builder::basic::sub_binary. A const
/// accessor may be constructed by validating any bsoncxx::v_noabi::types::b_binary.
///
/// The specific iterator and element types vary for each supported format.
///
/// bsoncxx::v_noabi::vector::formats::f_float32 uses a custom element type to support packed storage with a fixed byte
/// order.
///
/// bsoncxx::v_noabi::vector::formats::f_packed_bit uses a custom element and iterator type for single bits that unpacks
/// them as bool. It also has custom element and iterator types for byte access, which serve to mask writes to reserved
/// bits.
///
template <typename Format>
class accessor {
    using format_traits = typename detail::format_traits<typename std::remove_cv<Format>::type>;

   public:
    /// The type from bsoncxx::v_noabi::vector::formats representing this vector's layout and element type
    using format = Format;

    /// Const qualified version of @ref value_type
    /// @hideinitializer
    using const_value_type = typename format_traits::value_type const;

    /// A type suitable for holding element values.
    ///
    /// @hideinitializer
    /// For example: std::int8_t, float, bool
    using value_type = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::value_type const,
        typename format_traits::value_type>::type;

    /// Type for referencing const-qualified vector elements in-place
    /// @hideinitializer
    using const_reference = typename format_traits::const_reference;

    /// Type for referencing vector elements in-place
    ///
    /// @hideinitializer
    /// For example: std::int8_t&, bsoncxx::v_noabi::vector::elements::float32&,
    /// bsoncxx::v_noabi::vector::elements::packed_bit_element
    using reference = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_reference,
        typename format_traits::reference>::type;

    /// Iterator for const-qualified vector elements
    /// @hideinitializer
    using const_iterator = typename format_traits::const_iterator;

    /// Element iterator type
    ///
    /// @hideinitializer
    /// For example: std::int8_t*, bsoncxx::v_noabi::vector::elements::float32*,
    /// bsoncxx::v_noabi::vector::iterators::packed_bit_element
    using iterator = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_iterator,
        typename format_traits::iterator>::type;

    /// Type for the underlying byte data
    ///
    /// @hideinitializer
    /// For example: std::uint8_t, std::uint8_t const
    using byte_type = typename detail::accessor_data<format>::byte_type;

    /// Type for byte counts
    ///
    /// @hideinitializer
    /// For example: std::uint32_t, due to BSON size limits.
    using byte_count_type = typename detail::accessor_data<format>::byte_count_type;

    /// Type for element counts
    ///
    /// @hideinitializer
    /// For example: std::size_t
    using element_count_type = typename format_traits::element_count_type;

    /// Type for signed differences between byte iterators
    ///
    /// @hideinitializer
    /// For example: std::ptrdiff_t
    using byte_difference_type = typename format_traits::byte_difference_type;

    /// Type for signed differences between element iterators
    ///
    /// @hideinitializer
    /// For example: std::ptrdiff_t
    using element_difference_type = typename format_traits::element_difference_type;

    /// Type for referencing const-qualified vector bytes in-place
    /// @hideinitializer
    using const_byte_reference = typename format_traits::const_byte_reference;

    /// Type for referencing vector bytes in-place
    ///
    /// @hideinitializer
    /// For example: std::uint8_t&, std::uint8_t const&, bsoncxx::v_noabi::vector::elements::packed_bit_byte
    using byte_reference = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_byte_reference,
        typename format_traits::byte_reference>::type;

    /// Iterator for const-qualified vector bytes
    /// @hideinitializer
    using const_byte_iterator = typename format_traits::const_byte_iterator;

    /// Byte iterator type
    ///
    /// @hideinitializer
    /// For example: std::uint8_t*, std::uint8_t const*, bsoncxx::v_noabi::vector::iterators::packed_bit_byte
    using byte_iterator = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_byte_iterator,
        typename format_traits::byte_iterator>::type;

    /// @brief Construct a const vector accessor by validating a bsoncxx::v_noabi::types::b_binary reference.
    /// @param binary Non-owning reference to BSON binary data
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_invalid_vector, if validation fails.
    ///
    /// The Binary data is validated as a vector of the templated Format. On success, an accessor is created which
    /// references the same data as the bsoncxx::v_noabi::types::b_binary pointer.
    accessor(types::b_binary const& binary) : _data{(format::validate(binary), binary)} {}

    /// Obtain a const version of this vector accessor, without re-validating the vector data.
    constexpr accessor<format const> as_const() const noexcept {
        // Erase the template parameter from accessor_data to allow conversion from possibly-not-const to const.
        return {{_data.bytes, _data.size, _data.header_copy}};
    }

    /// Count the bytes of element data, not including any headers
    constexpr byte_count_type byte_size() const noexcept {
        return _data.size - byte_count_type(detail::header_size);
    }

    /// Count the number of elements
    constexpr element_count_type size() const noexcept {
        return format_traits::element_count(_data.size, _data.header_copy);
    }

    /// Obtain a per-element iterator pointing to the beginning of the vector
    constexpr iterator begin() const noexcept {
        return iterator(_data.bytes + detail::header_size);
    }

    /// Obtain a per-element end iterator
    constexpr iterator end() const noexcept {
        return begin() + element_difference_type(size());
    }

    /// Obtain a const per-element iterator pointing to the beginning of the vector
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(_data.bytes + detail::header_size);
    }

    /// Obtain a const per-element end iterator
    constexpr const_iterator cend() const noexcept {
        return cbegin() + element_difference_type(size());
    }

    /// Obtain a reference to the first element.
    /// @warning Undefined behavior if the vector is empty.
    reference front() noexcept {
        return *begin();
    }

    /// Obtain a const reference to the first element
    /// @warning Undefined behavior if the vector is empty.
    constexpr const_reference front() const noexcept {
        return *begin();
    }

    /// Obtain a reference to the last element
    /// @warning Undefined behavior if the vector is empty.
    reference back() noexcept {
        return *(begin() + element_difference_type(size() - 1u));
    }

    /// Obtain a const reference to the last element
    /// @warning Undefined behavior if the vector is empty.
    constexpr const_reference back() const noexcept {
        return *(begin() + element_difference_type(size() - 1u));
    }

    /// Obtain a per-byte iterator pointing to the beginning of the vector
    constexpr byte_iterator byte_begin() const noexcept {
        return format_traits::make_byte_iterator(begin(), end());
    }

    /// Obtain a per-byte end iterator
    constexpr byte_iterator byte_end() const noexcept {
        return byte_begin() + byte_difference_type(byte_size());
    }

    /// Obtain a const per-byte iterator pointing to the beginning of the vector
    constexpr const_byte_iterator byte_cbegin() const noexcept {
        return format_traits::make_byte_iterator(cbegin(), cend());
    }

    /// Obtain a const per-byte end iterator
    constexpr const_byte_iterator byte_cend() const noexcept {
        return byte_cbegin() + byte_difference_type(byte_size());
    }

    /// Obtain a reference to the first byte
    /// @warning Undefined behavior if the vector is empty.
    byte_reference byte_front() noexcept {
        return *byte_begin();
    }

    /// Obtain a const reference to the first byte
    /// @warning Undefined behavior if the vector is empty.
    constexpr const_byte_reference byte_front() const noexcept {
        return *byte_begin();
    }

    /// Obtain a reference to the last byte
    /// @warning Undefined behavior if the vector is empty.
    byte_reference byte_back() noexcept {
        return *(byte_begin() + byte_difference_type(byte_size() - 1u));
    }

    /// Obtain a const reference to the last byte
    /// @warning Undefined behavior if the vector is empty.
    constexpr const_byte_reference byte_back() const noexcept {
        return *(byte_begin() + byte_difference_type(byte_size() - 1u));
    }

    /// Obtain a reference to a numbered byte, with bounds checking
    /// @param index Index in the range 0 to byte_size()-1 inclusive.
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    byte_reference byte_at(byte_count_type index) {
        if (index >= byte_size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(byte_begin() + byte_difference_type(index));
    }

    /// Obtain a const reference to a numbered byte, with bounds checking
    /// @param index Index in the range 0 to byte_size()-1 inclusive.
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    const_byte_reference byte_at(byte_count_type index) const {
        if (index >= byte_size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(byte_begin() + byte_difference_type(index));
    }

    /// Obtain a reference to a numbered element, with bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    reference at(element_count_type index) {
        if (index >= size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(begin() + element_difference_type(index));
    }

    /// Obtain a const reference to a numbered element, with bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    const_reference at(element_count_type index) const {
        if (index >= size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(begin() + element_difference_type(index));
    }

    /// Obtain a reference to a numbered element, without bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @warning Undefined behavior if the index is out of bounds.
    reference operator[](element_count_type index) noexcept {
        return *(begin() + element_difference_type(index));
    }

    /// Obtain a const reference to a numbered element, without bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @warning Undefined behavior if the index is out of bounds.
    constexpr const_reference operator[](element_count_type index) const noexcept {
        return *(begin() + element_difference_type(index));
    }

    /// Test whether the vector is empty
    constexpr bool empty() const noexcept {
        return size() == 0u;
    }

   private:
    friend class bsoncxx::v_noabi::builder::basic::sub_binary;
    friend class accessor<typename std::remove_const<format>::type>;

    accessor(detail::accessor_data<format> data) noexcept : _data{data} {}

    detail::accessor_data<format> _data;
};

} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares @ref bsoncxx::v_noabi::vector::accessor.
///
