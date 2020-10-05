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

value::value(b_double v) : value(v.value) {}
value::value(double v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_DOUBLE;
    _impl->_value.value.v_double = v;
}

value::value(b_int32 v) : value(v.value) {}
value::value(int32_t v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_INT32;
    _impl->_value.value.v_int32 = v;
}

value::value(b_int64 v) : value(v.value) {}
value::value(int64_t v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_INT64;
    _impl->_value.value.v_int64 = v;
}

// TODO: enable_if T in T* decays to char
value::value(const char* v) : value(b_utf8{v}) {}
value::value(std::string v) : value(b_utf8{v}) {}
value::value(stdx::string_view v) : value(b_utf8{v}) {}
value::value(b_utf8 v) : _impl{stdx::make_unique<impl>()} {
    convert_to_libbson(v, &_impl->_value);
}

// BSONCXX_ENUM(document, 0x03)
// BSONCXX_ENUM(array, 0x04)
// BSONCXX_ENUM(binary, 0x05)
value::value(b_undefined) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_UNDEFINED;
}

value::value(b_null) : value(nullptr) {}
value::value(nullptr_t) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_NULL;
}

value::value(b_date v) : value(v.value) {}
value::value(std::chrono::milliseconds v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_DATE_TIME;
    _impl->_value.value.v_datetime = v.count();
}

value::value(b_oid v) : value(v.value) {}
value::value(oid v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_OID;
    std::memcpy(_impl->_value.value.v_oid.bytes, v.bytes(), v.k_oid_length);
}
value::value(b_bool v) : value(v.value) {}
value::value(bool v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_BOOL;
    _impl->_value.value.v_bool = v;
}

value::value(b_decimal128 v) : value(v.value) {}
value::value(decimal128 v) : _impl{stdx::make_unique<impl>()} {
    _impl->_value.value_type = BSON_TYPE_DECIMAL128;
    _impl->_value.value.v_decimal128.high = v.high();
    _impl->_value.value.v_decimal128.low = v.low();
}

void value::variadic_value(const type) {
    std::cout << "id: " << _impl->_value.value_type << std::endl;
    std::cout << "regex: " << _impl->_value.value.v_regex.regex << std::endl;
    std::cout << "options: " << _impl->_value.value.v_regex.options << std::endl;
}

template <typename T, typename... Args>
void value::variadic_value(const type id, T value, Args... args) {
    switch (id) {
        case type::k_regex:
            _impl->_value.value_type = BSON_TYPE_REGEX;
            if (sizeof...(args) == 1)
                _impl->_value.value.v_regex.regex = make_copy_for_libbson(value);
            else if (sizeof...(args) == 0)
                _impl->_value.value.v_regex.options = make_copy_for_libbson(value);
            else
                throw bsoncxx::exception{bsoncxx::error_code::k_internal_error};
            return variadic_value(id, std::forward<Args>(args)...);
        default:
            BSONCXX_UNREACHABLE;
    }
}

value::value(b_regex v) : value(v.type_id, std::string{v.regex}, std::string{v.options}) {}

template <typename T, typename... Args>
value::value(const type id, T value, Args... args) : _impl{stdx::make_unique<impl>()} {
    variadic_value(id, std::forward<T>(value), std::forward<Args>(args)...);
}

// BSONCXX_ENUM(dbpointer, 0x0C)
// BSONCXX_ENUM(code, 0x0D)
// BSONCXX_ENUM(symbol, 0x0E)
// BSONCXX_ENUM(codewscope, 0x0F)
// BSONCXX_ENUM(timestamp, 0x11)
// BSONCXX_ENUM(maxkey, 0x7F)
// BSONCXX_ENUM(minkey, 0xFF)

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
