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

#include <stdexcept>

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace array {

element::element() : document::element() {}

element::element(std::uint8_t const* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen)
    : document::element(raw, length, offset, keylen) {}

element::element(stdx::string_view const key) : document::element(key) {}

bool operator==(element const& elem, types::bson_value::view const& v) {
    return elem.get_value() == v;
}

bool operator==(types::bson_value::view const& v, element const& elem) {
    return elem == v;
}

bool operator!=(element const& elem, types::bson_value::view const& v) {
    return !(elem == v);
}

bool operator!=(types::bson_value::view const& v, element const& elem) {
    return !(elem == v);
}

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx
