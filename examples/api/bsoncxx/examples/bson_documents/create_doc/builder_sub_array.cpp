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

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::kvp;

    bsoncxx::document::value owner =
        bsoncxx::builder::basic::make_document(kvp("v", [](bsoncxx::builder::basic::sub_array arr) {
            arr.append(std::int32_t{1}, std::int64_t{2});
        }));
    bsoncxx::array::view v = owner.view()["v"].get_array().value;

    assert(v[0].type() == bsoncxx::type::k_int32);
    assert(v[1].type() == bsoncxx::type::k_int64);

    assert(v[0].get_int32().value == 1);
    assert(v[1].get_int64().value == 2);
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example();
}
