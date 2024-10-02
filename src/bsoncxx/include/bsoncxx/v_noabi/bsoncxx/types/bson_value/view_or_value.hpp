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

#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/view_or_value.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

///
/// Equivalent to `v_noabi::view_or_value<view, value>`.
///
using view_or_value = bsoncxx::v_noabi::view_or_value<view, value>;

}  // namespace bson_value
}  // namespace types
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace types {
namespace bson_value {

using ::bsoncxx::v_noabi::types::bson_value::view_or_value;

}  // namespace bson_value
}  // namespace types
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::types::bson_value::view_or_value.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace types {
namespace bson_value {

/// @ref bsoncxx::v_noabi::types::bson_value::view_or_value
class view_or_value {};

}  // namespace bson_value
}  // namespace types
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
