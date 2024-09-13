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
#include <vector>

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2.0, "three"]
void example(bsoncxx::array::view arr) {
    assert(std::distance(arr.begin(), arr.end()) == 3);

    std::vector<bsoncxx::array::element> elements;

    std::copy_if(
        arr.begin(), arr.end(), std::back_inserter(elements), [](const bsoncxx::array::element& e) {
            return e.key() == "0" || e.type() == bsoncxx::type::k_string;
        });

    assert(elements.size() == 2u);
    assert(elements[0].key() == "0");
    assert(elements[1].key() == "2");
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example(bsoncxx::from_json(R"({"v": [1, 2.0, "three"]})")["v"].get_array().value);
}
