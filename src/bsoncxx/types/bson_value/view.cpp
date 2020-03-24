// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/types/bson_value/view.hpp>

#include <cstdlib>
#include <cstring>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/private/libbson.hh>

#include <bsoncxx/config/private/prelude.hh>

#define BSONCXX_CITER \
    bson_iter_t iter; \
    bson_iter_init_from_data_at_offset(&iter, raw, length, offset, keylen);

#define BSONCXX_TYPE_CHECK(name)                                                \
    do {                                                                        \
        if (type() != bsoncxx::type::k_##name) {                                \
            throw bsoncxx::exception{error_code::k_need_element_type_k_##name}; \
        }                                                                       \
    } while (0)

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {
namespace bson_value {

// Boost doesn't mark the copy constructor and copy-assignment operator of string_ref as noexcept
// so we can't rely on automatic noexcept propagation. It really is though, so it is OK.
#if !defined(BSONCXX_POLY_USE_BOOST)
#define BSONCXX_ENUM(name, val)                                                                \
    view::view(b_##name value) noexcept : _type(static_cast<bsoncxx::type>(val)),              \
                                          _b_##name(std::move(value)) {                        \
        static_assert(std::is_nothrow_copy_constructible<b_##name>::value, "Copy may throw");  \
        static_assert(std::is_nothrow_copy_assignable<b_##name>::value, "Copy may throw");     \
        static_assert(std::is_nothrow_destructible<b_##name>::value, "Destruction may throw"); \
    }
#else
#define BSONCXX_ENUM(name, val)                                                                \
    view::view(b_##name value) noexcept : _type(static_cast<bsoncxx::type>(val)),              \
                                          _b_##name(std::move(value)) {                        \
        static_assert(std::is_nothrow_destructible<b_##name>::value, "Destruction may throw"); \
    }
#endif

#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

view::view(const view& rhs) noexcept {
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

view& view::operator=(const view& rhs) noexcept {
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

view::~view() {
    destroy();
}

bsoncxx::type view::type() const {
    return _type;
}

#define BSONCXX_ENUM(type, val)                       \
    const types::b_##type& view::get_##type() const { \
        BSONCXX_TYPE_CHECK(type);                     \
        return _b_##type;                             \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

view::view(const std::uint8_t* raw,
           std::uint32_t length,
           std::uint32_t offset,
           std::uint32_t keylen) {
    BSONCXX_CITER;

    auto value = bson_iter_value(&iter);

    _init((void*)value);
}

view::view(void* internal_value) {
    _init(internal_value);
}

void view::_init(void* internal_value) {
    bson_value_t* v = (bson_value_t*)(internal_value);

    _type = static_cast<bsoncxx::type>(v->value_type);

    switch (_type) {
        case bsoncxx::type::k_binary: {
            bson_subtype_t type = v->value.v_binary.subtype;
            std::uint32_t len = v->value.v_binary.data_len;
            const std::uint8_t* binary = v->value.v_binary.data;

            _b_binary = {static_cast<binary_sub_type>(type), len, binary};
            break;
        }
        case bsoncxx::type::k_utf8: {
            uint32_t len = v->value.v_utf8.len;
            const char* val = v->value.v_utf8.str;

            _b_utf8 = b_utf8{stdx::string_view{val, len}};
            break;
        }
        case bsoncxx::type::k_double: {
            _b_double = b_double{v->value.v_double};
            break;
        }
        case bsoncxx::type::k_int32: {
            _b_int32 = b_int32{v->value.v_int32};
            break;
        }
        case bsoncxx::type::k_int64: {
            _b_int64 = b_int64{v->value.v_int64};
            break;
        }
        case bsoncxx::type::k_undefined: {
            _b_undefined = b_undefined{};
            break;
        }
        case bsoncxx::type::k_oid: {
            const bson_oid_t* boid = &(v->value.v_oid);
            oid val_oid(reinterpret_cast<const char*>(boid->bytes), sizeof(boid->bytes));
            _b_oid = types::b_oid{std::move(val_oid)};
            break;
        }
        case bsoncxx::type::k_decimal128: {
            bson_decimal128_t d128 = v->value.v_decimal128;
            _b_decimal128 = b_decimal128{decimal128{d128.high, d128.low}};
            break;
        }
        case bsoncxx::type::k_bool: {
            _b_bool = b_bool{v->value.v_bool};
            break;
        }
        case bsoncxx::type::k_date: {
            _b_date = b_date{std::chrono::milliseconds{v->value.v_datetime}};
            break;
        }
        case bsoncxx::type::k_null: {
            _b_null = b_null{};
            break;
        }
        case bsoncxx::type::k_regex: {
            const char* options = v->value.v_regex.options;
            const char* regex = v->value.v_regex.regex;
            _b_regex = b_regex{stdx::string_view{regex}, stdx::string_view{options}};
            break;
        }
        case bsoncxx::type::k_dbpointer: {
            uint32_t collection_len = v->value.v_dbpointer.collection_len;
            const char* collection = v->value.v_dbpointer.collection;
            const bson_oid_t* boid = &(v->value.v_dbpointer.oid);

            oid v{reinterpret_cast<const char*>(boid->bytes), sizeof(boid->bytes)};

            _b_dbpointer = b_dbpointer{stdx::string_view{collection, collection_len}, std::move(v)};
            break;
        }
        case bsoncxx::type::k_code: {
            uint32_t len = v->value.v_code.code_len;
            const char* code = v->value.v_code.code;

            _b_code = b_code{stdx::string_view{code, len}};
            break;
        }
        case bsoncxx::type::k_symbol: {
            uint32_t len = v->value.v_symbol.len;
            const char* symbol = v->value.v_symbol.symbol;

            _b_symbol = b_symbol{stdx::string_view{symbol, len}};
            break;
        }
        case bsoncxx::type::k_codewscope: {
            uint32_t code_len = v->value.v_codewscope.code_len;
            const uint8_t* scope_ptr = v->value.v_codewscope.scope_data;
            uint32_t scope_len = v->value.v_codewscope.scope_len;
            const char* code = v->value.v_codewscope.code;
            document::view view(scope_ptr, scope_len);

            _b_codewscope = b_codewscope{stdx::string_view{code, code_len}, view};
            break;
        }
        case bsoncxx::type::k_timestamp: {
            uint32_t timestamp = v->value.v_timestamp.timestamp;
            uint32_t increment = v->value.v_timestamp.increment;

            _b_timestamp = b_timestamp{increment, timestamp};
            break;
        }
        case bsoncxx::type::k_minkey: {
            _b_minkey = b_minkey{};
            break;
        }
        case bsoncxx::type::k_maxkey: {
            _b_maxkey = b_maxkey{};
            break;
        }
        case bsoncxx::type::k_document: {
            const std::uint8_t* buf = v->value.v_doc.data;
            std::uint32_t len = v->value.v_doc.data_len;

            _b_document = b_document{document::view{buf, len}};
            break;
        }
        case bsoncxx::type::k_array: {
            // The bson_value_t struct does not have a separate union
            // member for arrays. They are handled the same as the document
            // BSON type.
            const std::uint8_t* buf = v->value.v_doc.data;
            std::uint32_t len = v->value.v_doc.data_len;

            _b_array = b_array{array::view{buf, len}};
            break;
        }
        default:
            BSONCXX_UNREACHABLE;
    }
}

bool operator==(const view& lhs, const view& rhs) {
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

bool operator!=(const view& lhs, const view& rhs) {
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

}  // namespace bson_value
}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
