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

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
// {"a": 1, "b": 2.0, "c": "three"}
void example(bsoncxx::document::element e) {
    switch (e.type()) {
        case bsoncxx::type::k_int32: {
            assert(e.key().compare("a") == 0);

            bsoncxx::types::b_int32 v = e.get_int32();

            assert(v.type_id == bsoncxx::type::k_int32);
            assert(v.value == 1);

            break;
        }
        case bsoncxx::type::k_double: {
            assert(e.key().compare("b") == 0);

            bsoncxx::types::b_double v = e.get_double();

            assert(v.type_id == bsoncxx::type::k_double);
            assert(v.value == 2.0);

            break;
        }
        case bsoncxx::type::k_string: {
            assert(e.key().compare("c") == 0);

            bsoncxx::types::b_string v = e.get_string();

            assert(v.type_id == bsoncxx::type::k_string);
            assert(v.value.compare("three") == 0);

            break;
        }
    }
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    const auto doc = bsoncxx::from_json(R"({"a": 1, "b": 2.0, "c": "three"})");

    example(doc["a"]);
    example(doc["b"]);
    example(doc["c"]);
}
