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

#include <bsoncxx/v1/types/id-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

namespace bsoncxx {
namespace v1 {
namespace types {

///
/// Enumeration identifying a BSON type.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @showenumvalues
///
enum class id : std::uint8_t {};

///
/// Enumeration identifying a BSON binary subtype.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @showenumvalues
///
enum class binary_subtype : std::uint8_t {};

} // namespace types
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides enumerations identifying the type of a BSON element.
///
