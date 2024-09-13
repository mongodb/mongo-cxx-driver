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

#include <cassert>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2]
void example(bsoncxx::array::view arr) {
    assert(arr.find(0) == arr.begin());

    {
        auto iter = arr.find(1);

        assert(iter != arr.end());
        assert(iter->key().compare("1") == 0);
        assert(iter->get_int32().value == 2);
    }

    assert(arr.find(2) == arr.end());
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example(bsoncxx::from_json(R"({"v": [1, 2]})")["v"].get_array().value);
}
