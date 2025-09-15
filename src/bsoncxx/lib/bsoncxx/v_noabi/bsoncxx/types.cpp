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

#include <bsoncxx/types.hpp>

//

#include <bsoncxx/v1/detail/macros.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace types {

#pragma push_macro("X")
#undef X
#define X(_name, _value) constexpr v_noabi::type b_##_name::type_id;
BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
