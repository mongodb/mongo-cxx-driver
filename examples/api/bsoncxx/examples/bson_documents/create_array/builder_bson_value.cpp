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
#include <cstdint>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::kvp;

    bsoncxx::types::bson_value::value values[]{
        std::int32_t{1},
        2.0,
        "three",
    };

    bsoncxx::array::value owner =
        bsoncxx::builder::basic::make_array(values[0], values[1], values[2]);
    bsoncxx::array::view arr = owner.view();

    assert(arr[0].type() == bsoncxx::type::k_int32);
    assert(arr[1].type() == bsoncxx::type::k_double);
    assert(arr[2].type() == bsoncxx::type::k_string);

    assert(arr[0].get_value() == values[0]);
    assert(arr[1].get_value() == values[1]);
    assert(arr[2].get_value() == values[2]);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
