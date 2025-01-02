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
    bsoncxx::builder::basic::array builder;

    builder.append(std::int32_t{1});
    bsoncxx::array::value a_owner = builder.extract();

    builder.clear();

    builder.append(std::int64_t{2});
    bsoncxx::array::value b_owner = builder.extract();

    bsoncxx::array::view a = a_owner.view();
    bsoncxx::array::view b = b_owner.view();

    EXPECT(a[0].type() == bsoncxx::type::k_int32);
    EXPECT(b[0].type() == bsoncxx::type::k_int64);

    EXPECT(a[0].get_int32().value == 1);
    EXPECT(b[0].get_int64().value == 2);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
