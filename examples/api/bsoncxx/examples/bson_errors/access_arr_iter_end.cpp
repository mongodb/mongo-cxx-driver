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
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2, 3]
void example(bsoncxx::array::view arr) {
    EXPECT(arr.begin() != arr.end());

    auto iter = arr.begin(); // 1

    ++iter; // 2
    ++iter; // 3
    ++iter; // End iterator.

    EXPECT(iter == arr.end());

    ++iter; // DO NOT DO THIS

    EXPECT(iter == arr.end()); // Incrementing an end iterator results in an end iterator.

    bsoncxx::array::element e = *iter; // DO NOT DO THIS

    EXPECT(!e); // An end iterator returns an invalid element.
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::builder::basic::make_array(1, 2, 3));
}
