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
#include <bsoncxx/types/bson_value/view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {"a": {"$numberInt": "1"}, "b": {"$numberLong": "2"}}
void example(bsoncxx::document::element e) {
    bsoncxx::types::b_int32 a{1};
    bsoncxx::types::b_int64 b{2};

    if (e.get_value() == a) {
        EXPECT(e.key() == "a");
    } else if (e.get_value() == b) {
        EXPECT(e.key() == "b");
    }

    bsoncxx::types::bson_value::view va{a};
    bsoncxx::types::bson_value::view vb{b};

    if (e == va) {
        EXPECT(e.key() == "a");
    } else if (e == vb) {
        EXPECT(e.key() == "b");
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    auto const doc = bsoncxx::from_json(R"({"a": {"$numberInt": "1"}, "b": {"$numberLong": "2"}})");

    example(doc["a"]);
    example(doc["b"]);
}
