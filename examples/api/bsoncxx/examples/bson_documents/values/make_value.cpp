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

#include <string>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using value_type = bsoncxx::types::bson_value::value;
    using view_type = bsoncxx::types::bson_value::view;

    {
        value_type owner = bsoncxx::types::bson_value::make_value(1);
        view_type v = owner.view();

        EXPECT(v.type() == bsoncxx::type::k_int32);
        EXPECT(v.get_int32().value == 1);
    }

    {
        value_type owner = bsoncxx::types::bson_value::make_value(bsoncxx::types::b_int64{2});
        view_type v = owner.view();

        EXPECT(v.type() == bsoncxx::type::k_int64);
        EXPECT(v.get_int64().value == 2);
    }

    {
        value_type owner = bsoncxx::types::bson_value::make_value(std::string("three"));
        view_type v = owner.view();

        EXPECT(v.type() == bsoncxx::type::k_string);
        EXPECT(v.get_string().value == "three");
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
