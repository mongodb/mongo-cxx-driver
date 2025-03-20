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

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace formats {

/// @brief Vector format for 32-bit floating point elements, packed least significant byte first.
struct f_float32 {};

/// @brief Vector format for signed 8-bit integer elements.
struct f_int8 {};

/// @brief Vector format for single bit elements, packed most significant bit first.
struct f_packed_bit {};

} // namespace formats
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares vector formats:
/// @ref bsoncxx::v_noabi::vector::formats::f_float32
/// @ref bsoncxx::v_noabi::vector::formats::f_int8
/// @ref bsoncxx::v_noabi::vector::formats::f_packed_bit
///
