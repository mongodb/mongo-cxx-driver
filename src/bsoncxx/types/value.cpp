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

#include <bsoncxx/config/private/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {

// Boost doesn't mark the copy constructor and copy-assignment operator of string_ref as noexcept
// so we can't rely on automatic noexcept propagation. It really is though, so it is OK.
#if !defined(BSONCXX_POLY_USE_BOOST)
#define BSONCXX_ENUM(name, val)                                                                \
    value::value(b_##name value) noexcept : _type(static_cast<bsoncxx::type>(val)),            \
                                            _b_##name(std::move(value)) {                      \
        static_assert(std::is_nothrow_copy_constructible<b_##name>::value, "Copy may throw");  \
        static_assert(std::is_nothrow_copy_assignable<b_##name>::value, "Copy may throw");     \
        static_assert(std::is_nothrow_destructible<b_##name>::value, "Destruction may throw"); \
    }
#else
#define BSONCXX_ENUM(name, val)                                                                \
    value::value(b_##name value) noexcept : _type(static_cast<bsoncxx::type>(val)),            \
                                            _b_##name(std::move(value)) {                      \
        static_assert(std::is_nothrow_destructible<b_##name>::value, "Destruction may throw"); \
    }
#endif

#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

value::value(const value& rhs) noexcept {
    switch (static_cast<int>(rhs._type)) {
#define BSONCXX_ENUM(type, val)                      \
    case val:                                        \
        new (&_b_##type) b_##type(rhs.get_##type()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    _type = rhs._type;
}

value& value::operator=(const value& rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }

    destroy();

    switch (static_cast<int>(rhs._type)) {
#define BSONCXX_ENUM(type, val)                      \
    case val:                                        \
        new (&_b_##type) b_##type(rhs.get_##type()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    _type = rhs._type;
    return *this;
}

value::~value() {
    destroy();
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

    // Silence compiler warnings about failing to return a value.
    BSONCXX_UNREACHABLE;
}

bool operator!=(const value& lhs, const value& rhs) {
    return !(lhs == rhs);
}

void value::destroy() noexcept {
    switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        _b_##type.~b_##type();  \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }
}

}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
