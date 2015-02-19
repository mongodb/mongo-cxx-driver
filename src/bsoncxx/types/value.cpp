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

#include <bsoncxx/types/value.hpp>

#include <cstdlib>
#include <cstring>

#include <bson.h>

#include <bsoncxx/json.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {

#define BSONCXX_ENUM(name, val)                                                 \
    value::value(b_##name value)                                                \
        : _type(static_cast<bsoncxx::type>(val)), _b_##name(std::move(value)) { \
    }

#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

value::value(const value& rhs) {
    *this = rhs;
}

value& value::operator=(const value& rhs) {
    _type = rhs._type;

    switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val)       \
    case val:                         \
        _b_##type = rhs.get_##type(); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    return *this;
}

value::value(value&& rhs) {
    *this = std::move(rhs);
}

value& value::operator=(value&& rhs) {
    _type = rhs._type;

    switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val)               \
    case val:                                 \
        _b_##type = std::move(rhs._b_##type); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    return *this;
}

value::~value() {
    switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        _b_##type.~b_##type();  \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }
}

bsoncxx::type value::type() const {
    return _type;
}

#define BSONCXX_ENUM(type, val)                        \
    const types::b_##type& value::get_##type() const { \
        return _b_##type;                              \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

bool operator==(const value& lhs, const value& rhs) {
    if (lhs.type() != rhs.type()) {
        return false;
    }

    switch (static_cast<int>(lhs.type())) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        return lhs.get_##type() == rhs.get_##type();
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    // TODO It shouldn't be possible to get here.  replace with macro unreachable
    std::abort();
}

bool operator!=(const value& lhs, const value& rhs) {
    return !(lhs == rhs);
}

}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
