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
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::builder::basic::array builder;
    builder.append(std::int32_t{1});
    builder.append(2.0);
    builder.append("three");
    bsoncxx::array::value owner = builder.extract();
    bsoncxx::array::view arr = owner.view();

    assert(arr[0].get_int32().value == 1);
    assert(arr[1].get_double().value == 2.0);
    assert(arr[2].get_string().value.compare("three") == 0);
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example();
}
