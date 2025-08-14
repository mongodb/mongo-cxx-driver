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

#include <bsoncxx/v1/types/id.hpp>

//

#include <string>

namespace bsoncxx {
namespace v1 {
namespace types {

std::string to_string(id rhs) {
#pragma push_macro("X")
#undef X
#define X(_name, _value) \
    case id::k_##_name:  \
        return #_name;

    switch (rhs) {
        BSONCXX_V1_TYPES_XMACRO(X)

        default:
            return "?";
    }
#pragma pop_macro("X")
}

std::string to_string(binary_subtype rhs) {
#pragma push_macro("X")
#undef X
#define X(_name, _value)            \
    case binary_subtype::k_##_name: \
        return #_name;

    switch (rhs) {
        BSONCXX_V1_BINARY_SUBTYPES_XMACRO(X)

        default:
            // All BSON binary subtype values in the range [0x80, 0xFF] are "user defined".
            if (rhs >= binary_subtype::k_user) {
                return "user";
            } else {
                return "?";
            }
    }
#pragma pop_macro("X")
}

} // namespace types
} // namespace v1
} // namespace bsoncxx
