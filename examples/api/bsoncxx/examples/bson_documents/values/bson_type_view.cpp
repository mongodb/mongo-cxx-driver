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

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using view_type = bsoncxx::types::bson_value::view;

    bsoncxx::types::b_int32 v0{1};
    bsoncxx::types::b_double v1{2.0};
    bsoncxx::types::b_string v2{"three"};

    view_type v;
    EXPECT(v.type() == bsoncxx::type::k_null);
    EXPECT(v.get_null() == bsoncxx::types::b_null{});

    v = view_type(v0);
    EXPECT(v.type() == v0.type_id);
    EXPECT(v.get_int32() == v0);

    v = view_type(v1);
    EXPECT(v.type() == v1.type_id);
    EXPECT(v.get_double() == v1);

    v = view_type(v2);
    EXPECT(v.type() == v2.type_id);
    EXPECT(v.get_string() == v2);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
