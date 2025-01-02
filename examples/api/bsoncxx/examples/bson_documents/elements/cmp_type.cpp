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

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {"a": {"$numberInt": "1"}, "b": {"$numberLong": "2"}}
void example(bsoncxx::document::element e) {
    std::int32_t a{1};
    std::int64_t b{2};

    if (e.type() == bsoncxx::type::k_int32) {
        EXPECT(e.key() == "a");
        EXPECT(e.get_int32().value == a);
    } else if (e.type() == bsoncxx::type::k_int64) {
        EXPECT(e.key() == "b");
        EXPECT(e.get_int64().value == b);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    auto const doc = bsoncxx::from_json(R"({"a": {"$numberInt": "1"}, "b": {"$numberLong": "2"}})");

    example(doc["a"]);
    example(doc["b"]);
}
