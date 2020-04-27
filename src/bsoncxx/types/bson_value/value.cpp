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

#include <bsoncxx/json.hpp>

#include <bsoncxx/types/bson_value/value.hpp>

#include <cstdlib>
#include <cstring>
#include <utility>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/types/bson_value/private/value.hh>
#include <bsoncxx/types/private/convert.hh>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {
namespace bson_value {

namespace {

void create_from_bsoncxx(bson_value_t* v, const class view& bson_view) {
    switch (bson_view.type()) {
#define BSONCXX_ENUM(name, val)              \
    case bsoncxx::type::k_##name: {          \
        auto value = bson_view.get_##name(); \
        convert_to_libbson(value, v);        \
        break;                               \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        default:
            BSONCXX_UNREACHABLE;
    }
}

}  // namespace

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
    create_from_bsoncxx(&_impl->_value, bson_view);
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
