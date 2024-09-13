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
#include <string>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::types::bson_value::value v = nullptr;

    assert(v.view().type() == bsoncxx::type::k_null);
    assert(v.view().get_null() == bsoncxx::types::b_null{});

    v = std::int32_t{1};

    assert(v.view().type() == bsoncxx::type::k_int32);
    assert(v.view().get_int32().value == 1);

    v = 2.0;

    assert(v.view().type() == bsoncxx::type::k_double);
    assert(v.view().get_double().value == 2.0);

    v = std::string("three");

    assert(v.view().type() == bsoncxx::type::k_string);
    assert(v.view().get_string().value.compare("three") == 0);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
