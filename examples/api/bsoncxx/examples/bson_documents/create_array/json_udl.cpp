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
#include <bsoncxx/json.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using namespace bsoncxx;

    bsoncxx::document::value a = R"([1, 2])"_bson;
    bsoncxx::document::value b = bsoncxx::from_json(R"([1, 2])");

    assert(a == b);
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example();
}
