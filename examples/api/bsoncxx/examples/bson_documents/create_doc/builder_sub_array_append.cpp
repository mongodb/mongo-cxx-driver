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

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::kvp;

    std::int32_t values[] = {1, 2, 3};

    bsoncxx::document::value owner =
        bsoncxx::builder::basic::make_document(kvp("v", [&](bsoncxx::builder::basic::sub_array arr) {
            for (int i = 0; i < 3; ++i) {
                arr.append(values[i]);
            }
        }));
    bsoncxx::array::view v = owner.view()["v"].get_array().value;

    EXPECT(v == bsoncxx::builder::basic::make_array(1, 2, 3));
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
