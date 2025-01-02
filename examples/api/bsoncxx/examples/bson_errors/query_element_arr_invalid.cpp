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

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::array::value::deleter_type deleter = [](std::uint8_t*) {};
    std::uint8_t data[] = {0u}; // An invalid BSON array.

    bsoncxx::array::value owner{data, sizeof(data), deleter};
    bsoncxx::array::view arr = owner.view();

    bsoncxx::array::element e = arr[0];

    EXPECT(!e); // An invalid BSON array returns an invalid element.

    try {
        bsoncxx::stdx::string_view key = e.key(); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (bsoncxx::exception const& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_unset_element);
    }

    try {
        bsoncxx::type type = e.type(); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (bsoncxx::exception const& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_unset_element);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
