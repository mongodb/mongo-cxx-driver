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
#include <string>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/document/view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::kvp;

    std::string keys[] = {"a", "b", "c"};
    std::int32_t values[] = {1, 2, 3};

    bsoncxx::array::value owner =
        bsoncxx::builder::basic::make_array([&](bsoncxx::builder::basic::sub_document doc) {
            for (int i = 0; i < 3; ++i) {
                doc.append(kvp(keys[i], values[i]));
            }
        });
    bsoncxx::document::view v = owner.view()[0].get_document().value;

    EXPECT(v["a"].get_int32().value == 1);
    EXPECT(v["b"].get_int32().value == 2);
    EXPECT(v["c"].get_int32().value == 3);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
