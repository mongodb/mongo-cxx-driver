// Copyright 2014 MongoDB Inc.
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

#include "bson/types.hpp"
#include "bson/json.hpp"

namespace bson {

std::ostream& operator<<(std::ostream& out, type rhs) {
    switch (static_cast<uint8_t>(rhs)) {
#define MONGOCXX_ENUM(name, val) \
    case val:                    \
        out << #name;            \
        break;
#include "bson/enums/type.hpp"
#undef MONGOCXX_ENUM
        default:
            out << "?";
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, binary_sub_type rhs) {
    switch (static_cast<uint8_t>(rhs)) {
#define MONGOCXX_ENUM(name, val) \
    case val:                    \
        out << #name;            \
        break;
#include "bson/enums/binary_sub_type.hpp"
#undef MONGOCXX_ENUM
        default:
            out << "?";
    }

    return out;
}

namespace types {

#define MONGOCXX_ENUM(name, val)                                       \
    std::ostream& operator<<(std::ostream& out, const b_##name& rhs) { \
        json_visitor jv(out, false, 0);                                \
        jv.visit_value(rhs);                                           \
        return out;                                                    \
    }
#include "bson/enums/type.hpp"
#undef MONGOCXX_ENUM

}  // namespace types
}  // namespace bson
