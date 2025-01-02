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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// a: {"a": 1}
// b: {"b": 2}
void example(bsoncxx::document::view a, bsoncxx::document::view b) {
    bsoncxx::builder::basic::document builder;

    builder.append(bsoncxx::builder::concatenate(a));
    builder.append(bsoncxx::builder::concatenate(b));

    EXPECT(builder.view() == bsoncxx::from_json(R"({"a": 1, "b": 2})"));
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::from_json(R"({"a": 1})"), bsoncxx::from_json(R"({"b": 2})"));
}
