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

#include "big_string.hh"

//

#include <cstddef>
#include <limits>
#include <memory>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(bsoncxx::stdx::string_view big_string) {
    bsoncxx::builder::basic::array builder;
    builder.append("element");
    bsoncxx::array::value original{builder.view()};  // Copy of current state.

    try {
        builder.append([&](bsoncxx::builder::basic::sub_array arr) {
            arr.append(big_string);  // Throws.
        });

        EXPECT(false && "should not reach this point");
    } catch (const bsoncxx::exception& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_cannot_append_string);
    }

    // Builder is in an erroneous state.
    try {
        builder.view();  // Throws.

        EXPECT(false && "should not reach this point");
    } catch (const bsoncxx::exception& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_unmatched_key_in_builder);
    }

    // Reset builder to a usable state.
    builder.clear();
    EXPECT(builder.view().empty());

    // Restore the original state prior to the append failure.
    builder.append(bsoncxx::builder::concatenate_array{original.view()});

    EXPECT(builder.view() == original.view());
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example(examples::big_string().view());
}
