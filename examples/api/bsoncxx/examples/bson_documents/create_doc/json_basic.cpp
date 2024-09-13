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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::document::value owner = bsoncxx::from_json(R"(
        {
            "a": 1,
            "b": 2.0,
            "c": "three"
        }
    )");
    bsoncxx::document::view doc = owner.view();

    assert(doc["a"].get_int32().value == 1);
    assert(doc["b"].get_double().value == 2.0);
    assert(doc["c"].get_string().value.compare("three") == 0);
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example();
}
