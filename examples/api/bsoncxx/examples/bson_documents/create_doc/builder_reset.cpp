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

    bsoncxx::builder::basic::document builder;

    builder.append(kvp("v", std::int32_t{1}));
    bsoncxx::document::value a_owner = builder.extract();

    builder.clear();

    builder.append(kvp("v", std::int64_t{2}));
    bsoncxx::document::value b_owner = builder.extract();

    bsoncxx::document::view a = a_owner.view();
    bsoncxx::document::view b = b_owner.view();

    ASSERT(a["v"].type() == bsoncxx::type::k_int32);
    ASSERT(b["v"].type() == bsoncxx::type::k_int64);

    ASSERT(a["v"].get_int32().value == 1);
    ASSERT(b["v"].get_int64().value == 2);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
