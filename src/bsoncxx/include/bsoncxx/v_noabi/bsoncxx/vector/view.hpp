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

#include <type_traits>

#include <bsoncxx/vector/view-fwd.hpp>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/vector/formats.hpp>
#include <bsoncxx/vector/impl.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

namespace builder {
namespace basic {
class sub_binary;
} // namespace basic
} // namespace builder

namespace vector {

/// @brief Accessor for the contents of a valid BSON Binary Vector
/// @tparam Format One of the @ref bsoncxx::v_noabi::vector::formats types, optionally with a const qualifier.
///
/// This accessor operates on data formatted for the bsoncxx::v_noabi::binary_sub_type::k_vector BSON binary
/// subtype. A mutable view may be constructed only using bsoncxx::v_noabi::builder::basic::sub_binary. A const
/// view may be constructed by validating a bsoncxx::v_noabi::types::b_binary.
///
/// The specific iterator and element types vary for each supported format. When possible,
/// iterators are raw pointers.
///
/// bsoncxx::v_noabi::vector::formats::f_float32 uses a custom element type to support packed storage with a fixed byte
/// order.
///
/// bsoncxx::v_noabi::vector::formats::f_packed_bit uses a custom element and iterator type for single bits that unpacks
/// them as bool. It also has custom element and iterator types for byte access, which serve to mask writes to reserved
/// bits.
///
template <typename Format>
class view {
    using format_traits = typename impl::format_traits<typename std::remove_cv<Format>::type>;

   public:
    using format = Format;

    using const_value_type = typename format_traits::value_type const;
    using value_type = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::value_type const,
        typename format_traits::value_type>::type;

    using const_reference = typename format_traits::const_reference;
    using reference = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_reference,
        typename format_traits::reference>::type;

    using const_iterator = typename format_traits::const_iterator;
    using iterator = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_iterator,
        typename format_traits::iterator>::type;

    using byte_type = typename impl::view_data<format>::byte_type;
    using byte_count_type = typename impl::view_data<format>::byte_count_type;
    using element_count_type = typename format_traits::element_count_type;
    using byte_difference_type = typename format_traits::byte_difference_type;
    using element_difference_type = typename format_traits::element_difference_type;

    using const_byte_reference = typename format_traits::const_byte_reference;
    using byte_reference = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_byte_reference,
        typename format_traits::byte_reference>::type;

    using const_byte_iterator = typename format_traits::const_byte_iterator;
    using byte_iterator = typename std::conditional<
        std::is_const<Format>::value,
        typename format_traits::const_byte_iterator,
        typename format_traits::byte_iterator>::type;

    /// @brief Construct a const Vector view by validating a bsoncxx::v_noabi::types::b_binary reference.
    /// @param binary Non-owning reference to BSON binary data
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_invalid_vector, if validation fails.
    ///
    /// The Binary data is validated as a Vector of the templated Format. On success, a view is created which
    /// references the same data as the bsoncxx::v_noabi::types::b_binary pointer.
    view(types::b_binary const& binary) : impl_data(format_traits::const_validate(binary)) {}

    /// @brief Count the bytes of element data
    /// @return Size of the vector data, not including any headers
    constexpr byte_count_type byte_size() const noexcept {
        return impl_data.size - byte_count_type(sizeof(impl::header::bytes));
    }

    /// @brief Count the number of elements
    /// @return Number of elements
    constexpr element_count_type size() const noexcept {
        return format_traits::element_count(impl_data.size, impl_data.header_copy);
    }

    /// @brief Obtain an iterator pointing to the beginning of the vector
    /// @return A per-element random access iterator, pointing at the first element if one exists
    constexpr iterator begin() const noexcept {
        return iterator(impl_data.bytes + sizeof(impl::header::bytes));
    }

    /// @brief Obtain an iterator pointing just past the end of the vector
    /// @return A per-element random access iterator, pointing just past the end of the vector
    constexpr iterator end() const noexcept {
        return begin() + element_difference_type(size());
    }

    /// @brief Obtain a reference to the first element
    /// @return An element reference
    /// Undefined behavior if the vector is empty.
    reference front() noexcept {
        return *begin();
    }

    /// @brief Obtain a const reference to the first element
    /// @return An element reference
    /// Undefined behavior if the vector is empty.
    constexpr const_reference front() const noexcept {
        return *begin();
    }

    /// @brief Obtain a reference to the last element
    /// @return An element reference
    /// Undefined behavior if the vector is empty.
    reference back() noexcept {
        return *(begin() + element_difference_type(size() - 1u));
    }

    /// @brief Obtain a const reference to the last element
    /// @return An element reference
    /// Undefined behavior if the vector is empty.
    constexpr const_reference back() const noexcept {
        return *(begin() + element_difference_type(size() - 1u));
    }

    /// @brief Obtain a byte iterator pointing to the beginning of the vector
    /// @return A per-byte random access iterator, pointing at the first byte if one exists
    constexpr byte_iterator byte_begin() const noexcept {
        return format_traits::make_byte_iterator(begin(), end());
    }

    /// @brief Obtain a byte iterator pointing just past the end of the vector
    /// @return A per-byte random access iterator, pointing just past the end of the vector
    constexpr byte_iterator byte_end() const noexcept {
        return byte_begin() + byte_difference_type(byte_size());
    }

    /// @brief Obtain a reference to the first byte
    /// @return A byte reference
    /// Undefined behavior if the vector is empty.
    byte_reference byte_front() noexcept {
        return *byte_begin();
    }

    /// @brief Obtain a const reference to the first byte
    /// @return A byte reference
    /// Undefined behavior if the vector is empty.
    constexpr const_byte_reference byte_front() const noexcept {
        return *byte_begin();
    }

    /// @brief Obtain a reference to the last byte
    /// @return A byte reference
    /// Undefined behavior if the vector is empty.
    byte_reference byte_back() noexcept {
        return *(byte_begin() + byte_difference_type(byte_size() - 1u));
    }

    /// @brief Obtain a const reference to the last byte
    /// @return A byte reference
    /// Undefined behavior if the vector is empty.
    constexpr const_byte_reference byte_back() const noexcept {
        return *(byte_begin() + byte_difference_type(byte_size() - 1u));
    }

    /// @brief Obtain a reference to a numbered byte, with bounds checking
    /// @param index Index in the range 0 to byte_size()-1 inclusive.
    /// @return A byte reference
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    byte_reference byte_at(byte_count_type index) {
        if (index >= byte_size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(byte_begin() + byte_difference_type(index));
    }

    /// @brief Obtain a const reference to a numbered byte, with bounds checking
    /// @param index Index in the range 0 to byte_size()-1 inclusive.
    /// @return A byte reference
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    const_byte_reference byte_at(byte_count_type index) const {
        if (index >= byte_size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(byte_begin() + byte_difference_type(index));
    }

    /// @brief Obtain a reference to a numbered element, with bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @return An element reference
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    reference at(element_count_type index) {
        if (index >= size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(begin() + element_difference_type(index));
    }

    /// @brief Obtain a const reference to a numbered element, with bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @return An element reference
    /// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_vector_out_of_range, if the index is
    /// outside the allowed range.
    const_reference at(element_count_type index) const {
        if (index >= size()) {
            throw bsoncxx::v_noabi::exception{error_code::k_vector_out_of_range};
        }
        return *(begin() + element_difference_type(index));
    }

    /// @brief Obtain a reference to a numbered element, without bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @return An element reference
    /// Undefined behavior if the index is out of bounds.
    reference operator[](element_count_type index) noexcept {
        return *(begin() + element_difference_type(index));
    }

    /// @brief Obtain a const reference to a numbered element, without bounds checking
    /// @param index Index in the range 0 to size()-1 inclusive.
    /// @return An element reference
    /// Undefined behavior if the index is out of bounds.
    constexpr const_reference operator[](element_count_type index) const noexcept {
        return *(begin() + element_difference_type(index));
    }

    /// @brief Test whether the vector is empty
    /// @return True if the vector has a size() of zero.
    constexpr bool empty() const noexcept {
        return size() == 0u;
    }

   private:
    friend class bsoncxx::v_noabi::builder::basic::sub_binary;

    view(impl::view_data<format> data) noexcept : impl_data(data) {}

    impl::view_data<format> impl_data;
};

} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares @ref bsoncxx::v_noabi::vector::view.
///
