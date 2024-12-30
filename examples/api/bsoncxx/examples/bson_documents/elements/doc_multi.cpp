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

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {"a": 1, "b": 2.0, "c": "three"}
void example(bsoncxx::document::element e) {
    switch (e.type()) {
        case bsoncxx::type::k_int32: {
            EXPECT(e.key() == "a");

            bsoncxx::types::b_int32 v = e.get_int32();

            EXPECT(v.type_id == bsoncxx::type::k_int32);
            EXPECT(v.value == 1);

            break;
        }
        case bsoncxx::type::k_double: {
            EXPECT(e.key() == "b");

            bsoncxx::types::b_double v = e.get_double();

            EXPECT(v.type_id == bsoncxx::type::k_double);
            EXPECT(v.value == 2.0);

            break;
        }
        case bsoncxx::type::k_string: {
            EXPECT(e.key() == "c");

            bsoncxx::types::b_string v = e.get_string();

            EXPECT(v.type_id == bsoncxx::type::k_string);
            EXPECT(v.value == "three");

            break;
        }
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    auto const doc = bsoncxx::from_json(R"({"a": 1, "b": 2.0, "c": "three"})");

    example(doc["a"]);
    example(doc["b"]);
    example(doc["c"]);
}
