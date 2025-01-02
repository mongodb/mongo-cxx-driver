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
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2.0, "three"]
void example(bsoncxx::array::element e) {
    if (e.type() == bsoncxx::type::k_int32) {
        EXPECT(e.key() == "0");

        bsoncxx::types::b_int32 v = e.get_int32();

        EXPECT(v.type_id == bsoncxx::type::k_int32);
        EXPECT(v.value == 1);
    } else {
        EXPECT(e.key() != "0");
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    auto const owner = bsoncxx::builder::basic::make_array(1, 2.0, "three");
    auto const arr = owner.view();

    example(arr[0]);
    example(arr[1]);
    example(arr[2]);
}
