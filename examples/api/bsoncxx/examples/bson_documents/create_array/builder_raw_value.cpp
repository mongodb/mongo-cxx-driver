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

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2]
void example(const std::uint8_t* data, std::size_t length) {
    using deleter_type = bsoncxx::array::value::deleter_type;

    std::uint8_t* raw = new std::uint8_t[length];
    std::copy_n(data, length, raw);

    deleter_type deleter = [](std::uint8_t* data) { delete[] data; };
    bsoncxx::array::value owner{raw, length, deleter};
    bsoncxx::array::view arr = owner.view();

    assert(arr[0].get_int32().value == 1);
    assert(arr[1].get_int32().value == 2);
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    bsoncxx::document::value owner = bsoncxx::from_json(R"({"v": [1, 2]})");
    bsoncxx::array::view arr = owner.view()["v"].get_array().value;

    example(arr.data(), arr.length());
}
