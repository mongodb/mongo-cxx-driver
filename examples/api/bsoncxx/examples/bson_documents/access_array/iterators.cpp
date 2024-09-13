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

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2]
void example(bsoncxx::array::view arr) {
    ASSERT(arr.begin() != arr.end());

    auto iter = arr.begin();
    ASSERT(iter == arr.begin());

    {
        bsoncxx::array::element e = *iter;

        ASSERT(e.key().compare("0") == 0);
        ASSERT(e.get_int32().value == 1);
    }

    ++iter;

    ASSERT(iter->key().compare("1") == 0);
    ASSERT(iter->get_int32().value == 2);

    {
        auto iter_copy = iter++;

        ASSERT(iter_copy != iter);
        ASSERT(iter_copy->key().compare("1") == 0);
        ASSERT(iter_copy->get_int32() == 2);
    }

    ASSERT(iter == arr.end());
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::from_json(R"({"v": [1, 2]})")["v"].get_array().value);
}
