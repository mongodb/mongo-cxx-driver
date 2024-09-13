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

#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <examples/macros.hh>

namespace {

// [Example]
void example(bsoncxx::stdx::string_view big_string) {
    bsoncxx::builder::basic::array builder;
    builder.append("element");
    bsoncxx::array::value original{builder.view()};

    try {
        builder.append(big_string);  // Throws.

        assert(false && "should not reach this point");
    } catch (const bsoncxx::exception& ex) {
        assert(ex.code() == bsoncxx::error_code::k_cannot_append_string);
    }

    assert(builder.view() == original.view());
}
// [Example]

}  // namespace

int EXAMPLES_CDECL main() {
    example(examples::big_string().view());
}
