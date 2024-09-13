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

#include <algorithm>
#include <iterator>
#include <vector>

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {"a": 1, "b": 2.0, "c": "three"}
void example(bsoncxx::document::view doc) {
    ASSERT(std::distance(doc.begin(), doc.end()) == 3);

    std::vector<bsoncxx::document::element> elements;

    std::copy_if(doc.begin(),
                 doc.end(),
                 std::back_inserter(elements),
                 [](const bsoncxx::document::element& e) {
                     return e.key().compare("a") == 0 || e.type() == bsoncxx::type::k_string;
                 });

    ASSERT(elements.size() == 2u);
    ASSERT(elements[0].key().compare("a") == 0);
    ASSERT(elements[1].key().compare("c") == 0);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::from_json(R"({"a": 1, "b": 2.0, "c": "three"})"));
}
