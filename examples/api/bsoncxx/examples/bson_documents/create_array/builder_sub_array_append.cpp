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

#include <cstdint>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::make_array;

    std::int32_t values[] = {1, 2, 3};

    bsoncxx::array::value owner = make_array([&](bsoncxx::builder::basic::sub_array arr) {
        for (int i = 0; i < 3; ++i) {
            arr.append(values[i]);
        }
    });
    bsoncxx::array::view v = owner.view()[0].get_array().value;

    EXPECT(v == make_array(1, 2, 3));
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
