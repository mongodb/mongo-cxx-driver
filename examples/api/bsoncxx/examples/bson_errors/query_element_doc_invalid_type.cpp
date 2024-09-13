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
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {"x": 1}
void example(bsoncxx::document::element e) {
    ASSERT(e.key().compare("x") == 0);
    ASSERT(e.type() == bsoncxx::type::k_int32);
    ASSERT(e.get_int32().value == 1);

    try {
        bsoncxx::types::b_double d = e.get_double();  // Throws.

        ASSERT(false && "should not reach this point");
    } catch (const bsoncxx::exception& ex) {
        ASSERT(ex.code() == bsoncxx::error_code::k_need_element_type_k_double);
    }

    try {
        bsoncxx::types::b_string str = e.get_string();  // Throws.

        ASSERT(false && "should not reach this point");
    } catch (const bsoncxx::exception& ex) {
        ASSERT(ex.code() == bsoncxx::error_code::k_need_element_type_k_string);
    }
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    const auto doc = bsoncxx::from_json(R"({"x": 1})");

    example(doc["x"]);
}
