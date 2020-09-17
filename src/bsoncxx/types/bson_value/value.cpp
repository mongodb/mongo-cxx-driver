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

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/types/bson_value/private/value.hh>
#include <bsoncxx/types/private/convert.hh>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {
namespace bson_value {

value::value(b_double v) : value(v.value) {}
value::value(double v) {
    _impl = stdx::make_unique<impl>();
    _impl->_value.value_type = BSON_TYPE_DOUBLE;
    _impl->_value.value.v_double = v;
}

// BSONCXX_ENUM(utf8, 0x02)
// value::value(b_utf8 v) : value(v.value) {}
// value::value(stdx::string_view v) : value(std::string(v)){}
// value::value(std::string v) {
//    _impl = stdx::make_unique<impl>();
//    _impl->_value.value_type = BSON_TYPE_UTF8;
//
//
//    _impl->_value.value.v_utf8.str = const_cast<char *>(v.c_str());
//    _impl->_value.value.v_utf8.len = static_cast<uint32_t>(v.size());
//}

// BSONCXX_ENUM(document, 0x03)
// BSONCXX_ENUM(array, 0x04)
// BSONCXX_ENUM(binary, 0x05)
// BSONCXX_ENUM(undefined, 0x06)
// BSONCXX_ENUM(oid, 0x07)
value::value(b_bool v) : value(v.value) {}
value::value(bool v) {
    _impl = stdx::make_unique<impl>();
    _impl->_value.value_type = BSON_TYPE_BOOL;
    _impl->_value.value.v_bool = v;
}
// BSONCXX_ENUM(date, 0x09)
// BSONCXX_ENUM(null, 0x0A)
// BSONCXX_ENUM(regex, 0x0B)
// BSONCXX_ENUM(dbpointer, 0x0C)
// BSONCXX_ENUM(code, 0x0D)
// BSONCXX_ENUM(symbol, 0x0E)
// BSONCXX_ENUM(codewscope, 0x0F)
// BSONCXX_ENUM(int32, 0x10)
// BSONCXX_ENUM(timestamp, 0x11)
// BSONCXX_ENUM(int64, 0x12)
// BSONCXX_ENUM(decimal128, 0x13)
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
