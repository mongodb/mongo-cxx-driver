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

#include <bsoncxx/builder/concatenate.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace basic {

using ::bsoncxx::v_noabi::builder::concatenate;

}  // namespace basic
}  // namespace builder
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace builder {
namespace basic {

using ::bsoncxx::v_noabi::builder::basic::concatenate;

}  // namespace basic
}  // namespace builder
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Redeclares @ref bsoncxx::v_noabi::builder::concatenate in the @ref
/// bsoncxx::v_noabi::builder::basic namespace.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace builder {
namespace basic {

/// @ref bsoncxx::v_noabi::builder::concatenate(v_noabi::document::view_or_value doc)
/// @note An overload accepting @ref v_noabi::array::view_or_value and returning a @ref
/// v_noabi::builder::concatenate_array is also declared in this scope.
v_noabi::concatenate_doc concatenate(v_noabi::document::view_or_value doc);

}  // namespace basic
}  // namespace builder
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
