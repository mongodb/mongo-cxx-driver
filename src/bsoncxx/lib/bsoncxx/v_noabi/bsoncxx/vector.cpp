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

#include <cstdint>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/vector/impl.hpp>

#include <bson/bson.h>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace impl {

enum element_type : std::uint8_t {
    signed_integer = 0,
    unsigned_integer = 1,
    floating_point = 2,
};

enum element_size : std::uint8_t {
    bits_1 = 0,
    bits_8 = 3,
    bits_32 = 7,
};

static header make_header(element_type element_type, element_size element_size, std::uint8_t padding) {
    return header{{(std::uint8_t)((std::uint8_t)element_type << 4 | (std::uint8_t)element_size), padding}};
}

static header copy_header(std::uint8_t const* bytes) {
    header result;
    memcpy(&result.bytes, bytes, sizeof result.bytes);
    return result;
}

template <typename Impl>
static std::uint32_t libbson_length_for_append(std::size_t element_count, Impl func) {
    std::uint32_t result = func(element_count);
    if (result < sizeof(header::bytes)) {
        throw bsoncxx::v_noabi::exception{error_code::k_vector_too_large};
    }
    return result;
}

std::uint32_t format_traits<formats::f_int8>::length_for_append(std::size_t element_count) {
    return libbson_length_for_append(element_count, bson_vector_int8_binary_data_length);
}

std::uint32_t format_traits<formats::f_float32>::length_for_append(std::size_t element_count) {
    return libbson_length_for_append(element_count, bson_vector_float32_binary_data_length);
}

std::uint32_t format_traits<formats::f_packed_bit>::length_for_append(std::size_t element_count) {
    return libbson_length_for_append(element_count, bson_vector_packed_bit_binary_data_length);
}

header format_traits<formats::f_int8>::write_frame(std::uint8_t* binary_data, std::uint32_t, std::size_t) {
    header hdr = make_header(element_type::signed_integer, element_size::bits_8, 0);
    memcpy(binary_data, &hdr, sizeof hdr);
    return hdr;
}

header format_traits<formats::f_float32>::write_frame(std::uint8_t* binary_data, std::uint32_t, std::size_t) {
    header hdr = make_header(element_type::floating_point, element_size::bits_32, 0);
    memcpy(binary_data, &hdr, sizeof hdr);
    return hdr;
}

header format_traits<formats::f_packed_bit>::write_frame(
    std::uint8_t* binary_data,
    std::uint32_t binary_data_length,
    std::size_t element_count) {
    header hdr = make_header(
        element_type::unsigned_integer, element_size::bits_1, std::uint8_t(std::size_t(7u) & -element_count));
    binary_data[binary_data_length - 1] = UINT8_C(0);
    memcpy(binary_data, &hdr, sizeof hdr);
    return hdr;
}

template <typename Format, typename Impl>
static view_data<Format> libbson_const_validate(bsoncxx::v_noabi::types::b_binary const& binary, Impl func) {
    if (binary.sub_type != binary_sub_type::k_vector || !func(NULL, binary.bytes, binary.size)) {
        throw bsoncxx::v_noabi::exception{error_code::k_invalid_vector};
    }
    return {binary.bytes, binary.size, copy_header(binary.bytes)};
}

view_data<formats::f_int8 const> format_traits<formats::f_int8>::const_validate(
    bsoncxx::v_noabi::types::b_binary const& binary) {
    return libbson_const_validate<formats::f_int8 const>(binary, bson_vector_int8_const_view_init);
}

view_data<formats::f_float32 const> format_traits<formats::f_float32>::const_validate(
    bsoncxx::v_noabi::types::b_binary const& binary) {
    return libbson_const_validate<formats::f_float32 const>(binary, bson_vector_float32_const_view_init);
}

view_data<formats::f_packed_bit const> format_traits<formats::f_packed_bit>::const_validate(
    bsoncxx::v_noabi::types::b_binary const& binary) {
    return libbson_const_validate<formats::f_packed_bit const>(binary, bson_vector_packed_bit_const_view_init);
}

} // namespace impl
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx
