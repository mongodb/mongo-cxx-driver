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
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
// {"a": 1, "b": 2}
void example(bsoncxx::document::view doc) {
    assert(doc.begin() != doc.end());

    auto iter = doc.begin();  // "a": 1

    ++iter;  // "b": 2
    ++iter;  // End iterator.

    assert(iter == doc.end());

    ++iter;  // DO NOT DO THIS

    assert(iter == doc.end());  // Incrementing an end iterator results in an end iterator.

    bsoncxx::document::element e = *iter;  // DO NOT DO THIS

    assert(!e);  // An end iterator returns an invalid element.
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example(bsoncxx::from_json(R"({"a": 1, "b": 2})"));
}
