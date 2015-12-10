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

#include <bsoncxx/config/prelude.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {

// The default constructor does not initialize the value union.
value::value() : _type{} {
}

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
    if (this == &rhs) {
        return *this;
    }

    if (_type) {
            switch (static_cast<typename std::underlying_type<bsoncxx::type>::type>(*_type)) {
#define BSONCXX_ENUM(type, val)     \
        case val:                   \
            _b_##type.~b_##type();  \
            break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        }
    }

    _type = rhs._type;
 
    if (_type) {
        switch (static_cast<typename std::underlying_type<bsoncxx::type>::type>(*_type)) {
#define BSONCXX_ENUM(type, val)           \
        case val:                         \
            _b_##type = rhs.get_##type(); \
            break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        }
    }

    return *this;
}

value::value(value&& rhs) noexcept {
    *this = std::move(rhs);
    rhs._type = stdx::nullopt;
}

value& value::operator=(value&& rhs) noexcept {
    if (this == &rhs) {
        // TODO: use bsoncxx error category
        throw std::runtime_error("self move assign");
    }

    if (_type) {
            switch (static_cast<typename std::underlying_type<bsoncxx::type>::type>(*_type)) {
#define BSONCXX_ENUM(type, val)     \
        case val:                   \
            _b_##type.~b_##type();  \
            break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        }
    }

    _type = rhs._type;
    rhs._type = stdx::nullopt;

    if (_type) {
        switch (static_cast<typename std::underlying_type<bsoncxx::type>::type>(*_type)) {
#define BSONCXX_ENUM(type, val)                   \
        case val:                                 \
            _b_##type = std::move(rhs._b_##type); \
            break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        }
    }

    return *this;
}

value::~value() {
    if (_type) {
            switch (static_cast<typename std::underlying_type<bsoncxx::type>::type>(*_type)) {
#define BSONCXX_ENUM(type, val)     \
        case val:                   \
            _b_##type.~b_##type();  \
            break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        }
    }
}

stdx::optional<bsoncxx::type> value::type() const {
    return _type;
}

#define BSONCXX_ENUM(type, val)                        \
    const types::b_##type& value::get_##type() const { \
        return _b_##type;                              \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

bool operator==(const value& lhs, const value& rhs) {
    if (!lhs && !rhs) {
        return true;
    }

    if (!lhs || !rhs) {
        return false;
    }

    if (lhs.type() != rhs.type()) {
        return false;
    }

    switch (static_cast<typename std::underlying_type<bsoncxx::type>::type>(*(lhs.type()))) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        return lhs.get_##type() == rhs.get_##type();
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    BSONCXX_UNREACHABLE;
}

bool operator!=(const value& lhs, const value& rhs) {
    return !(lhs == rhs);
}

}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
