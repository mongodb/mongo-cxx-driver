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

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::document::value::deleter_type deleter = [](std::uint8_t*) {};
    std::uint8_t data[] = {0u}; // An invalid BSON document.

    bsoncxx::document::value owner{data, sizeof(data), deleter};
    bsoncxx::document::view doc = owner.view();

    bsoncxx::document::element e = doc["x"];

    EXPECT(!e); // An invalid BSON document returns an invalid element.

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
