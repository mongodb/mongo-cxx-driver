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
#include <vector>

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2.0, "three"]
void example(bsoncxx::array::view arr) {
    ASSERT(std::distance(arr.begin(), arr.end()) == 3);

    std::vector<bsoncxx::array::element> elements;

    std::copy_if(
        arr.begin(), arr.end(), std::back_inserter(elements), [](const bsoncxx::array::element& e) {
            return e.key().compare("0") == 0 || e.type() == bsoncxx::type::k_string;
        });

    ASSERT(elements.size() == 2u);
    ASSERT(elements[0].key().compare("0") == 0);
    ASSERT(elements[1].key().compare("2") == 0);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::from_json(R"({"v": [1, 2.0, "three"]})")["v"].get_array().value);
}
