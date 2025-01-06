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

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::types::bson_value::view v;
    EXPECT(v.type() == bsoncxx::type::k_null);
    EXPECT(v.get_null() == bsoncxx::types::b_null{});

    try {
        bsoncxx::types::b_int32 i = v.get_int32(); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (bsoncxx::exception const& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_need_element_type_k_int32);
    }

    try {
        bsoncxx::types::b_int64 i = v.get_int64(); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (bsoncxx::exception const& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_need_element_type_k_int64);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
