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
#include <cstdint>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::builder::basic::kvp;

    bsoncxx::types::b_int32 a{1};
    bsoncxx::types::b_double b{2.0};
    bsoncxx::types::b_string c{"three"};

    bsoncxx::document::value owner =
        bsoncxx::builder::basic::make_document(kvp("a", a), kvp("b", b), kvp("c", c));
    bsoncxx::document::view doc = owner.view();

    assert(doc["a"].type() == bsoncxx::type::k_int32);
    assert(doc["b"].type() == bsoncxx::type::k_double);
    assert(doc["c"].type() == bsoncxx::type::k_string);

    assert(doc["a"].get_int32() == a);
    assert(doc["b"].get_double() == b);
    assert(doc["c"].get_string() == c);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
