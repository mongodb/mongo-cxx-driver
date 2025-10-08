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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(bsoncxx::stdx::string_view big_string) {
    using bsoncxx::builder::basic::kvp;

    bsoncxx::builder::basic::document builder;
    builder.append(kvp("key", "value"));
    bsoncxx::document::value original{builder.view()};

    try {
        builder.append(kvp("too big", big_string)); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (bsoncxx::exception const& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_cannot_append_string);
    }

    EXPECT(builder.view() == original.view());
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    examples::with_big_string(example);
}
