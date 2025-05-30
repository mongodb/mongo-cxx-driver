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

#include <bsoncxx/v1/types/view.hpp>

//

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace types {

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                                                             \
    static_assert(is_regular<b_##_name>::value, "bsoncxx::v1::types::b_" #_name " must be regular"); \
    static_assert(is_semitrivial<b_##_name>::value, "bsoncxx::v1::types::b_" #_name " must be semitrivial");

BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

static_assert(is_regular<view>::value, "bsoncxx::v1::types::view must be regular");
static_assert(is_semitrivial<view>::value, "bsoncxx::v1::types::view must be semitrivial");

} // namespace types
} // namespace v1
} // namespace bsoncxx
