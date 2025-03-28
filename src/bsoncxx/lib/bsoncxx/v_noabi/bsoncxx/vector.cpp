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

#include <bsoncxx/vector/detail.hpp>
#include <bsoncxx/vector/formats.hpp>

//

#include <cstdint>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>

#include <bson/bson.h>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace detail {

// Equivalent to bson_vector_element_type_t.
enum element_type : std::uint8_t {
    signed_integer = 0,
    unsigned_integer = 1,
    floating_point = 2,
};

// Equivalent to bson_vector_element_size_t.
enum element_size : std::uint8_t {
    bits_1 = 0,
    bits_8 = 3,
    bits_32 = 7,
};

static header make_header(element_type element_type, element_size element_size, std::uint8_t padding) {
    return {{static_cast<std::uint8_t>((element_type << 4) | element_size), padding}};
}

static void write_header(std::uint8_t* binary_data, header const& hdr) {
    std::memcpy(binary_data, hdr.data(), header_size);
}

template <typename Impl>
static std::uint32_t libbson_length_for_append(std::size_t element_count, Impl func) {
    std::uint32_t result = func(element_count);
    if (result < BSON_VECTOR_HEADER_LEN) {
        throw exception{error_code::k_vector_too_large};
    }
    return result;
}

template <typename Impl>
static void libbson_validate(types::b_binary const& binary, Impl func) {
    if (binary.sub_type != binary_sub_type::k_vector || !func(NULL, binary.bytes, binary.size)) {
        throw exception{error_code::k_invalid_vector};
    }
}

} // namespace detail

namespace formats {

std::uint32_t f_int8::length_for_append(std::size_t element_count) {
    return detail::libbson_length_for_append(element_count, bson_vector_int8_binary_data_length);
}

std::uint32_t f_float32::length_for_append(std::size_t element_count) {
    return detail::libbson_length_for_append(element_count, bson_vector_float32_binary_data_length);
}

std::uint32_t f_packed_bit::length_for_append(std::size_t element_count) {
    return detail::libbson_length_for_append(element_count, bson_vector_packed_bit_binary_data_length);
}

void f_int8::write_frame(std::uint8_t* binary_data, std::uint32_t, std::size_t) {
    detail::write_header(
        binary_data, detail::make_header(detail::element_type::signed_integer, detail::element_size::bits_8, 0));
}

void f_float32::write_frame(std::uint8_t* binary_data, std::uint32_t, std::size_t) {
    detail::write_header(
        binary_data, detail::make_header(detail::element_type::floating_point, detail::element_size::bits_32, 0));
}

void f_packed_bit::write_frame(std::uint8_t* binary_data, std::uint32_t binary_data_length, std::size_t element_count) {
    binary_data[binary_data_length - 1] = UINT8_C(0);
    detail::write_header(
        binary_data,
        detail::make_header(
            detail::element_type::unsigned_integer,
            detail::element_size::bits_1,
            std::uint8_t(std::size_t{7u} & -element_count)));
}

void formats::f_int8::validate(types::b_binary const& binary) {
    return detail::libbson_validate(binary, bson_vector_int8_const_view_init);
}

void formats::f_float32::validate(types::b_binary const& binary) {
    return detail::libbson_validate(binary, bson_vector_float32_const_view_init);
}

void formats::f_packed_bit::validate(types::b_binary const& binary) {
    return detail::libbson_validate(binary, bson_vector_packed_bit_const_view_init);
}

} // namespace formats
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx
