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
// {"a": 1, "b": 2.0, "c": "three"}
void example(bsoncxx::document::view doc) {
    for (bsoncxx::document::element e : doc) {
        switch (e.type()) {
            case bsoncxx::type::k_int32:
                assert(e.key() == "a");
                assert(e.get_int32().value == 1);
                break;
            case bsoncxx::type::k_double:
                assert(e.key() == "b");
                assert(e.get_double().value == 2.0);
                break;
            case bsoncxx::type::k_string:
                assert(e.key() == "c");
                assert(e.get_string().value == "three");
                break;
        }
    }
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example(bsoncxx::from_json(R"({"a": 1, "b": 2.0, "c": "three"})"));
}
