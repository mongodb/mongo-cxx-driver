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

namespace {

// Equivalent to bson_vector_element_type_t.
// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class): used by bitwise operations.
enum element_type : std::uint8_t {
    signed_integer = 0,
    unsigned_integer = 1,
    floating_point = 2,
};

// Equivalent to bson_vector_element_size_t.
// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class): used by bitwise operations.
enum element_size : std::uint8_t {
    bits_1 = 0,
    bits_8 = 3,
    bits_32 = 7,
};

detail::header make_header(element_type type, element_size size, std::uint8_t padding) {
    return {{static_cast<std::uint8_t>((type << 4) | size), padding}};
}

void write_header(std::uint8_t* binary_data, detail::header const& hdr) {
    std::memcpy(binary_data, hdr.data(), detail::header_size);
}

template <typename Impl>
std::uint32_t libbson_length_for_append(std::size_t element_count, Impl func) {
    std::uint32_t result = func(element_count);
    if (result < BSON_VECTOR_HEADER_LEN) {
        throw exception{error_code::k_vector_too_large};
    }
    return result;
}

template <typename Impl>
void libbson_validate(types::b_binary const& binary, Impl func) {
    if (binary.sub_type != binary_sub_type::k_vector || !func(NULL, binary.bytes, binary.size)) {
        throw exception{error_code::k_invalid_vector};
    }
}

} // namespace

namespace formats {

std::uint32_t f_int8::length_for_append(std::size_t element_count) {
    return libbson_length_for_append(element_count, bson_vector_int8_binary_data_length);
}

std::uint32_t f_float32::length_for_append(std::size_t element_count) {
    return libbson_length_for_append(element_count, bson_vector_float32_binary_data_length);
}

std::uint32_t f_packed_bit::length_for_append(std::size_t element_count) {
    return libbson_length_for_append(element_count, bson_vector_packed_bit_binary_data_length);
}

void f_int8::write_frame(std::uint8_t* binary_data, std::uint32_t, std::size_t) {
    write_header(binary_data, make_header(element_type::signed_integer, element_size::bits_8, 0));
}

void f_float32::write_frame(std::uint8_t* binary_data, std::uint32_t, std::size_t) {
    write_header(binary_data, make_header(element_type::floating_point, element_size::bits_32, 0));
}

void f_packed_bit::write_frame(std::uint8_t* binary_data, std::uint32_t binary_data_length, std::size_t element_count) {
    static constexpr std::uint8_t mask = {0x7u};

    binary_data[binary_data_length - 1] = UINT8_C(0);
    write_header(binary_data, make_header(unsigned_integer, bits_1, std::uint8_t(mask & -element_count)));
}

void formats::f_int8::validate(types::b_binary const& binary) {
    return libbson_validate(binary, bson_vector_int8_const_view_init);
}

void formats::f_float32::validate(types::b_binary const& binary) {
    return libbson_validate(binary, bson_vector_float32_const_view_init);
}

void formats::f_packed_bit::validate(types::b_binary const& binary) {
    return libbson_validate(binary, bson_vector_packed_bit_const_view_init);
}

} // namespace formats
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx
