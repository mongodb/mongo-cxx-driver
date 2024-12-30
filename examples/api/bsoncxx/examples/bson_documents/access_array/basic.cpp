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
// [1, 2.0, "three"]
void example(bsoncxx::array::view arr) {
    for (bsoncxx::array::element e : arr) {
        switch (e.type()) {
            case bsoncxx::type::k_int32:
                EXPECT(e.key() == "0");
                EXPECT(e.get_int32().value == 1);
                break;
            case bsoncxx::type::k_double:
                EXPECT(e.key() == "1");
                EXPECT(e.get_double().value == 2.0);
                break;
            case bsoncxx::type::k_string:
                EXPECT(e.key() == "2");
                EXPECT(e.get_string().value == "three");
                break;
        }
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() { // clang-format off
    example(bsoncxx::builder::basic::make_array(1, 2.0, "three"));

} // clang-format on
