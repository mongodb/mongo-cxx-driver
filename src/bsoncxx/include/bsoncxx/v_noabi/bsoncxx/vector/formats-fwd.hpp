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

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace vector {
namespace formats {

struct f_float32;
struct f_int8;
struct f_packed_bit;

} // namespace formats
} // namespace vector
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace vector {
namespace formats {

using ::bsoncxx::v_noabi::vector::formats::f_float32;
using ::bsoncxx::v_noabi::vector::formats::f_int8;
using ::bsoncxx::v_noabi::vector::formats::f_packed_bit;

} // namespace formats
} // namespace vector
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Declares entities in @ref bsoncxx::v_noabi::vector::formats.
///
