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

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/json.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::array::value::deleter_type deleter = [](std::uint8_t*) {};
    std::uint8_t data[] = {0u}; // An invalid BSON array.

    bsoncxx::array::value owner{data, sizeof(data), deleter};
    bsoncxx::array::view arr = owner.view();

    auto iter = arr.begin();

    EXPECT(iter == arr.end()); // An invalid BSON document returns an end iterator.

    bsoncxx::array::element e = *iter; // DO NOT DO THIS

    EXPECT(!e); // An end iterator returns an invalid element.
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
