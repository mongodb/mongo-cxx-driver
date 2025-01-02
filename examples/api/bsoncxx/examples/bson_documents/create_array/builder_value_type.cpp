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
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::array::value owner = bsoncxx::builder::basic::make_array(std::int32_t{1}, 2.0, "three");
    bsoncxx::array::view arr = owner.view();

    EXPECT(arr[0].type() == bsoncxx::type::k_int32);
    EXPECT(arr[1].type() == bsoncxx::type::k_double);
    EXPECT(arr[2].type() == bsoncxx::type::k_string);

    EXPECT(arr[0].get_int32().value == 1);
    EXPECT(arr[1].get_double().value == 2.0);
    EXPECT(arr[2].get_string().value == "three");
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
