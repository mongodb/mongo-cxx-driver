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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

#include <examples/api/runner.hh>
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

    bsoncxx::document::value doc =
        bsoncxx::builder::basic::make_document(kvp("a", values[0]), kvp("b", values[1]), kvp("c", values[2]));

    EXPECT(doc.view() == bsoncxx::from_json(R"({"a": 1, "b": 2.0, "c": "three"})"));
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
