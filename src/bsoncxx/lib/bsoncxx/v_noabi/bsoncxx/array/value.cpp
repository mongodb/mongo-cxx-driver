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

#include <bsoncxx/array/value.hpp>

//

#include <cstring>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace array {

static_assert(is_explicitly_convertible<value&&, v1::array::value>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<value const&, v1::array::value>::value, "v_noabi -> v1 must be explicit");

// Backward compatibility with lack of default destructor and `value({})` prevents the following conversions.
static_assert(!is_explicitly_convertible<v1::array::value&&, value>::value, "v1 -> v_noabi is not supported");
static_assert(!is_explicitly_convertible<v1::array::value const&, value>::value, "v1 -> v_noabi is not supported");

namespace {

void uint8_t_deleter(std::uint8_t* ptr) {
    delete[] ptr;
}

} // namespace

value::value(v_noabi::array::view view)
    : _value{[&]() -> unique_ptr_type {
          auto res = unique_ptr_type{new std::uint8_t[view.size()], uint8_t_deleter};
          std::memcpy(res.get(), view.data(), view.size());
          return res;
      }()},
      _length{view.size()} {}

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

v_noabi::array::value from_v1(v1::array::value&& v) {
    auto const deleter_ptr = v.get_deleter().target<v_noabi::array::value::deleter_type>();

    if (!deleter_ptr || *deleter_ptr == &v1::document::value::noop_deleter) {
        return from_v1(static_cast<v1::array::value const&>(v)); // Fallback to copy.
    }

    auto const length = v.length();
    auto const deleter = *deleter_ptr;

    return {v.release().release(), length, deleter};
}

} // namespace v_noabi
} // namespace bsoncxx
