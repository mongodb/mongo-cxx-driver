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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::kvp;

    bsoncxx::types::bson_value::value values[]{
        std::int32_t{1},
        2.0,
        "three",
    };

    bsoncxx::document::value owner = bsoncxx::builder::basic::make_document(
        kvp("a", values[0]), kvp("b", values[1]), kvp("c", values[2]));
    bsoncxx::document::view doc = owner.view();

    assert(doc["a"].type() == bsoncxx::type::k_int32);
    assert(doc["b"].type() == bsoncxx::type::k_double);
    assert(doc["c"].type() == bsoncxx::type::k_string);

    assert(doc["a"].get_value() == values[0]);
    assert(doc["b"].get_value() == values[1]);
    assert(doc["c"].get_value() == values[2]);
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example();
}
