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

#include <iostream>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/concatenate.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// a: [1]
// b: [2]
void example(bsoncxx::array::view a, bsoncxx::array::view b) {
    bsoncxx::builder::basic::array builder;

    builder.append(bsoncxx::builder::concatenate(a));
    builder.append(bsoncxx::builder::concatenate(b));

    EXPECT(builder.view() == bsoncxx::builder::basic::make_array(1, 2));
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::builder::basic::make_array(1), bsoncxx::builder::basic::make_array(2));
}
