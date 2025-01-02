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

#include <bsoncxx/decimal128.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    try {
        bsoncxx::decimal128 d{"invalid"}; // Throws.

        EXPECT(false && "should not reach this point");
    } catch (bsoncxx::exception const& ex) {
        EXPECT(ex.code() == bsoncxx::error_code::k_invalid_decimal128);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
