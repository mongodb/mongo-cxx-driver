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

#include <bsoncxx/types/bson_value/value.hpp>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/types/bson_value/private/value.hh>
#include <bsoncxx/types/private/convert.hh>
#include <iostream>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {
namespace bson_value {

value::value(b_array v) : value(v.value) {}
value::value(b_binary v) : value(v.type_id, v.sub_type, v.size, v.bytes) {}
value::value(b_bool v) : value(v.value) {}
value::value(b_code v) : value(v.type_id, v) {}
value::value(b_codewscope v) : value(type::k_codewscope, v.code, v.scope) {}
value::value(b_date v) : value(v.value) {}
value::value(b_dbpointer v) : value(v.type_id, v.collection, v.value) {}
value::value(b_decimal128 v) : value(v.value) {}
value::value(b_document v) : value(v.view()) {}
value::value(b_double v) : value(v.value) {}
value::value(b_int32 v) : value(v.value) {}
value::value(b_int64 v) : value(v.value) {}
value::value(b_maxkey) : value(type::k_maxkey) {}
value::value(b_minkey) : value(type::k_minkey) {}
value::value(b_null) : value(nullptr) {}
value::value(b_oid v) : value(v.value) {}
value::value(b_regex v) : value(v.type_id, std::string{v.regex}, std::string{v.options}) {}
value::value(b_symbol v) : value(v.type_id, v) {}
value::value(b_timestamp v) : value(v.type_id, v.increment, v.timestamp) {}
value::value(b_undefined) : value(type::k_undefined) {}
value::value(b_utf8 v) : value(v.value) {}

value::value(decimal128 v) : value(type::k_decimal128, v.high(), v.low()) {}

value::value(double v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_DOUBLE;
    _impl->_value.value.v_double = v;
}

value::value(int32_t v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_INT32;
    _impl->_value.value.v_int32 = v;
}

value::value(int64_t v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_INT64;
    _impl->_value.value.v_int64 = v;
}

// TODO: enable_if T in T* decays to char
value::value(const char* v) : value(stdx::string_view{v}) {}
value::value(std::string v) : value(stdx::string_view{v}) {}
value::value(stdx::string_view v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_UTF8;
    _impl->_value.value.v_utf8.str = make_copy_for_libbson(v);
    _impl->_value.value.v_utf8.len = (uint32_t)v.size();
}

value::value(nullptr_t) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_NULL;
}

value::value(std::chrono::milliseconds v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_DATE_TIME;
    _impl->_value.value.v_datetime = v.count();
}

value::value(oid v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_OID;
    std::memcpy(_impl->_value.value.v_oid.bytes, v.bytes(), v.k_oid_length);
}
value::value(bool v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_BOOL;
    _impl->_value.value.v_bool = v;
}

value::value(const type id, uint64_t a, uint64_t b) : _impl{stdx::make_unique<impl>()} {
    if (id == type::k_decimal128) {
        _impl->_value.value_type = BSON_TYPE_DECIMAL128;
        _impl->_value.value.v_decimal128.high = a;
        _impl->_value.value.v_decimal128.low = b;
    } else if (id == type::k_timestamp) {
        _impl->_value.value_type = BSON_TYPE_TIMESTAMP;
        _impl->_value.value.v_timestamp.increment = (uint32_t)a;
        _impl->_value.value.v_timestamp.timestamp = (uint32_t)b;
    } else {
        throw std::logic_error{"Not decimal128 or timestamp"};
    }
}

value::value(const type id, stdx::string_view a, oid b) : _impl{stdx::make_unique<impl>()} {
    if (id == type::k_dbpointer) {
        _impl->_value.value_type = BSON_TYPE_DBPOINTER;
        _impl->_value.value.v_dbpointer.collection = make_copy_for_libbson(a);
        _impl->_value.value.v_dbpointer.collection_len = (uint32_t)a.length();
        std::memcpy(_impl->_value.value.v_dbpointer.oid.bytes, b.bytes(), b.k_oid_length);
    } else {
        throw std::logic_error{"Not dbpointer"};
    }
}

value::value(const type id, stdx::string_view a, bsoncxx::document::view_or_value b)
    : _impl{stdx::make_unique<impl>()} {
    if (id == type::k_codewscope) {
        _impl->_value.value_type = BSON_TYPE_CODEWSCOPE;
        _impl->_value.value.v_codewscope.code = make_copy_for_libbson(a);
        _impl->_value.value.v_codewscope.code_len = (uint32_t)a.length();
        _impl->_value.value.v_codewscope.scope_len = (uint32_t)b.view().length();
        _impl->_value.value.v_codewscope.scope_data = (uint8_t*)bson_malloc0(b.view().length());
        std::memcpy(
            _impl->_value.value.v_codewscope.scope_data, b.view().data(), b.view().length());
    }
}
value::value(bsoncxx::document::view v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_DOCUMENT;
    _impl->_value.value.v_doc.data_len = (uint32_t)v.length();
    _impl->_value.value.v_doc.data = (uint8_t*)bson_malloc0(v.length());
    std::memcpy(_impl->_value.value.v_doc.data, v.data(), v.length());
}

value::value(std::vector<unsigned char> v, binary_sub_type sub_type)
    : value(type::k_binary, sub_type, (uint32_t)v.size(), (uint8_t*)v.data()) {}
value::value(const type id, const binary_sub_type sub_id, uint32_t size, const uint8_t* data)
    : _impl{stdx::make_unique<impl>()} {
    if (id != type::k_binary)
        throw std::logic_error{"Not binary"};

    _impl->_value.value_type = BSON_TYPE_BINARY;

    _impl->_value.value.v_binary.subtype = static_cast<bson_subtype_t>(sub_id);
    _impl->_value.value.v_binary.data_len = size;
    _impl->_value.value.v_binary.data = (uint8_t*)bson_malloc(size);
    std::memcpy(_impl->_value.value.v_binary.data, data, size);
}

value::value(bsoncxx::array::view v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_ARRAY;
    _impl->_value.value.v_doc.data_len = (uint32_t)v.length();
    _impl->_value.value.v_doc.data = (uint8_t*)bson_malloc0(v.length());
    std::memcpy(_impl->_value.value.v_doc.data, v.data(), v.length());
}

value::value(const type id) : _impl{stdx::make_unique<impl>()} {
    if (id == type::k_minkey) {
        _impl->_value.value_type = BSON_TYPE_MINKEY;
    } else if (id == type::k_maxkey) {
        _impl->_value.value_type = BSON_TYPE_MAXKEY;
    } else if (id == type::k_undefined) {
        _impl->_value.value_type = BSON_TYPE_UNDEFINED;
    } else {
        throw std::logic_error{"Must be min/max key or undefined"};
    }
}

value::value(const type id, stdx::string_view a, stdx::string_view b)
    : _impl{stdx::make_unique<impl>()} {
    if (id == type::k_regex) {
        _impl->_value.value_type = BSON_TYPE_REGEX;
        _impl->_value.value.v_regex.regex = make_copy_for_libbson(a);
        _impl->_value.value.v_regex.options = make_copy_for_libbson(b);
    } else if (id == type::k_code) {
        _impl->_value.value_type = BSON_TYPE_CODE;
        _impl->_value.value.v_code.code = make_copy_for_libbson(a);
        _impl->_value.value.v_code.code_len = (uint32_t)a.length();
    } else if (id == type::k_symbol) {
        _impl->_value.value_type = BSON_TYPE_SYMBOL;
        _impl->_value.value.v_symbol.symbol = make_copy_for_libbson(a);
        _impl->_value.value.v_symbol.len = (uint32_t)a.length();
    } else {
        throw std::logic_error{"Unknown type"};
    }
}

value::~value() = default;

value::value(value&&) noexcept = default;

value& value::operator=(value&&) noexcept = default;

value::value(const std::uint8_t* raw,
             std::uint32_t length,
             std::uint32_t offset,
             std::uint32_t keylen) {
    bson_iter_t iter;

    bson_iter_init_from_data_at_offset(&iter, raw, length, offset, keylen);
    auto value = bson_iter_value(&iter);

    _impl = stdx::make_unique<impl>(value);
}

value::value(void* internal_value)
    : _impl(stdx::make_unique<impl>((bson_value_t*)internal_value)) {}

value::value(const value& rhs) : value(&rhs._impl->_value) {}

value::value(const class view& bson_view) {
    _impl = stdx::make_unique<impl>();
    convert_to_libbson(&_impl->_value, bson_view);
}

value& value::operator=(const value& rhs) {
    *this = value{rhs};
    return *this;
}

bson_value::view value::view() const noexcept {
    return _impl->view();
}

value::operator bson_value::view() const noexcept {
    return view();
}

}  // namespace bson_value
}  // namespace types

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
