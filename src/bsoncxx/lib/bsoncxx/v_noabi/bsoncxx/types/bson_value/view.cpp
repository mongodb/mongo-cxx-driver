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

#include <cstdlib>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/convert.hh>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#define BSONCXX_CITER                                                       \
    bson_iter_t iter;                                                       \
    bson_iter_init_from_data_at_offset(&iter, raw, length, offset, keylen); \
    ((void)0)

#define BSONCXX_TYPE_CHECK(name)                                                         \
    do {                                                                                 \
        if (type() != bsoncxx::v_noabi::type::k_##name) {                                \
            throw bsoncxx::v_noabi::exception{error_code::k_need_element_type_k_##name}; \
        }                                                                                \
    } while (0)

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

view::view() noexcept : view(nullptr) {}

#define BSONCXX_ENUM(name, val)                                                                                  \
    view::view(b_##name v) noexcept : _type(static_cast<bsoncxx::v_noabi::type>(val)), _b_##name(std::move(v)) { \
        static_assert(std::is_nothrow_copy_constructible<b_##name>::value, "Copy may throw");                    \
        static_assert(std::is_nothrow_copy_assignable<b_##name>::value, "Copy may throw");                       \
        static_assert(std::is_nothrow_destructible<b_##name>::value, "Destruction may throw");                   \
    }

#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

view::view(view const& rhs) noexcept {
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

view& view::operator=(view const& rhs) noexcept {
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
        default:
            break;
    }

    _type = rhs._type;
    return *this;
}

view::~view() {
    destroy();
}

bsoncxx::v_noabi::type view::type() const {
    return _type;
}

#define BSONCXX_ENUM(type, val)                       \
    types::b_##type const& view::get_##type() const { \
        BSONCXX_TYPE_CHECK(type);                     \
        return _b_##type;                             \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

view::view(std::uint8_t const* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen) {
    BSONCXX_CITER;

    auto value = bson_iter_value(&iter);

    // ABI backward compatibility. Const is restored in `view::_init`.
    _init(const_cast<void*>(static_cast<void const*>(value)));
}

view::view(void* internal_value) noexcept {
    _init(internal_value);
}

void view::_init(void* internal_value) noexcept {
    if (!internal_value) {
        _type = bsoncxx::v_noabi::type::k_null;
        _b_null = bsoncxx::v_noabi::types::b_null{};
        return;
    }

    auto v = static_cast<bson_value_t const*>(internal_value);
    _type = static_cast<bsoncxx::v_noabi::type>(v->value_type);

    switch (_type) {
#define BSONCXX_ENUM(name, val)                                         \
    case bsoncxx::v_noabi::type::k_##name: {                            \
        ::bsoncxx::v_noabi::types::convert_from_libbson(v, &_b_##name); \
        break;                                                          \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        default:
            BSONCXX_PRIVATE_UNREACHABLE;
    }
}

bool operator==(view const& lhs, view const& rhs) {
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
    BSONCXX_PRIVATE_UNREACHABLE;
}

bool operator!=(view const& lhs, view const& rhs) {
    return !(lhs == rhs);
}

void view::destroy() noexcept {
    switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        _b_##type.~b_##type();  \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }
}

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
