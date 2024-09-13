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

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
// [1, 2.0, "three"]
void example(bsoncxx::array::element e) {
    bsoncxx::types::bson_value::view v = e.get_value();
    assert(v.type() == e.type());

    switch (v.type()) {
        case bsoncxx::type::k_int32:
            assert(e.key() == "0");
            assert(v.get_int32() == e.get_int32());
            break;
        case bsoncxx::type::k_double:
            assert(e.key() == "1");
            assert(v.get_double() == e.get_double());
            break;
        case bsoncxx::type::k_string:
            assert(e.key() == "2");
            assert(v.get_string() == e.get_string());
            break;
    }
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    const auto doc = bsoncxx::from_json(R"({"v": [1, 2.0, "three"]})");
    const auto arr = doc["v"].get_array().value;

    example(arr[0]);
    example(arr[1]);
    example(arr[2]);
}
