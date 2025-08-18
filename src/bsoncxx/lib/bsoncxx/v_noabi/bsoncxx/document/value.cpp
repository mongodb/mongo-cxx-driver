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

#include <bsoncxx/document/value.hpp>

//

#include <cstdint>
#include <utility>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace document {

static_assert(is_explicitly_convertible<value&&, v1::document::value>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<value const&, v1::document::value>::value, "v_noabi -> v1 must be explicit");

// Unconstrained template parameters for value(T const& t)` and `operator=(T const& t)` result in the following lies.
static_assert(is_explicitly_convertible<v1::document::value&&, value>::value, "v1 -> v_noabi: this is a lie!");
static_assert(is_explicitly_convertible<v1::document::value const&, value>::value, "v1 -> v_noabi: this is a lie!");

// Backward compatibility with unconstrained `operator=(T const&)` permits the following.
static_assert(std::is_assignable<value, v1::document::value&&>::value, "v1 -> v_noabi: assignment is supported");
static_assert(std::is_assignable<value, v1::document::value const&>::value, "v1 -> v_noabi: assignment is supported");

// Backward compatibility with lack of default destructor and `value({})` prevent implicit conversions.
static_assert(!is_implicitly_convertible<v1::document::value&&, value>::value, "v1 -> v_noabi is not supported");
static_assert(!is_implicitly_convertible<v1::document::value const&, value>::value, "v1 -> v_noabi is not supported");

namespace {

void uint8_t_deleter(std::uint8_t* ptr) {
    delete[] ptr;
}

} // namespace

value::value(v_noabi::document::view view)
    : _value{[&]() -> unique_ptr_type {
          auto res = unique_ptr_type{new std::uint8_t[view.size()], uint8_t_deleter};
          std::memcpy(res.get(), view.data(), view.size());
          return res;
      }()},
      _length{view.size()} {}

} // namespace document
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

v_noabi::document::value from_v1(v1::document::value&& v) {
    auto const deleter_ptr = v.get_deleter().target<v_noabi::document::value::deleter_type>();

    if (!deleter_ptr || *deleter_ptr == &v1::document::value::noop_deleter) {
        return from_v1(static_cast<v1::document::value const&>(v)); // Fallback to copy.
    }

    auto const length = v.length();
    auto const deleter = *deleter_ptr;

    return {v.release().release(), length, deleter};
}

} // namespace v_noabi
} // namespace bsoncxx
