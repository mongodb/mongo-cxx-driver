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

#include <bsoncxx/builder/basic/sub_binary-fwd.hpp>

//

#include <cstdint>

#include <bsoncxx/vector/accessor-fwd.hpp>
#include <bsoncxx/vector/detail-fwd.hpp>
#include <bsoncxx/vector/formats-fwd.hpp>

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace basic {

///
/// Represents a BSON Binary element being constructed during an append operation.
///
class sub_binary {
   public:
    ///
    /// Default constructor
    ///
    sub_binary(core* core) : _core{core} {}

    /// @brief Allocate space for an un-initialized BSON Binary element of any subtype.
    /// @param sub_type BSON binary subtype code, identifying the format of the data within.
    /// @param length Number of bytes to allocate
    /// @return Pointer to uninitialized memory within the bson_t, valid only during this sub_binary builder's lifetime.
    ///      The caller must overwrite every byte if the resulting BSON document is to be used.
    /// @throws bsoncxx::v_noabi::exception if this sub_binary has already allocated.
    /// @throws bsoncxx::v_noabi::exception if the binary fails to append due to the BSON size limit.
    std::uint8_t* allocate(binary_sub_type sub_type, std::uint32_t length) {
        return _core->append(sub_type, length);
    }

    /// @brief Allocate and format space for a BSON Binary Vector with uninitialized elements.
    /// @param fmt Instance of a format type from @ref bsoncxx::v_noabi::vector::formats
    /// @param element_count Number of elements to allocate space for.
    /// @return A writable vector::accessor, valid during the lifetime of this sub_binary builder. Every element must be
    /// overwritten before that element is read or the resulting document is used.
    /// @throws bsoncxx::v_noabi::exception if this sub_binary has already allocated.
    /// @throws bsoncxx::v_noabi::exception if the binary fails to append due to the BSON size limit.
    /// @throws bsoncxx::v_noabi::exception if a vector of the requested size would be too large to represent.
    template <typename Format, typename SFINAE = typename vector::detail::format_traits<Format>::value_type>
    vector::accessor<Format> allocate(Format fmt, std::size_t element_count) {
        (void)fmt;
        std::uint32_t binary_data_length = Format::length_for_append(element_count);
        std::uint8_t* binary_data = allocate(binary_sub_type::k_vector, binary_data_length);
        Format::write_frame(binary_data, binary_data_length, element_count);
        return {vector::detail::accessor_data<Format>(binary_data, binary_data_length)};
    }

   private:
    core* _core;
};

} // namespace basic
} // namespace builder
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares @ref bsoncxx::v_noabi::builder::basic::sub_binary
///
