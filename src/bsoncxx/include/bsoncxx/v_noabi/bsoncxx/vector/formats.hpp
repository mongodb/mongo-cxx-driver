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

#include <bsoncxx/vector/formats-fwd.hpp>

//

#include <array>

#include <bsoncxx/types-fwd.hpp>
#include <bsoncxx/vector/accessor-fwd.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace formats {

/// @brief Vector format for 32-bit floating point elements, packed least significant byte first.
struct f_float32 {
    static BSONCXX_ABI_EXPORT_CDECL(std::uint32_t) length_for_append(std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(void)
    write_frame(std::uint8_t* binary_data, std::uint32_t binary_data_length, std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(void) validate(types::b_binary const& binary);
};

/// @brief Vector format for signed 8-bit integer elements.
struct f_int8 {
    static BSONCXX_ABI_EXPORT_CDECL(std::uint32_t) length_for_append(std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(void)
    write_frame(std::uint8_t* binary_data, std::uint32_t binary_data_length, std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(void) validate(types::b_binary const& binary);
};

/// @brief Vector format for single bit elements, packed most significant bit first.
struct f_packed_bit {
    static BSONCXX_ABI_EXPORT_CDECL(std::uint32_t) length_for_append(std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(void)
    write_frame(std::uint8_t* binary_data, std::uint32_t binary_data_length, std::size_t element_count);
    static BSONCXX_ABI_EXPORT_CDECL(void) validate(types::b_binary const& binary);
};

} // namespace formats
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities in @ref bsoncxx::v_noabi::vector::formats.
///
