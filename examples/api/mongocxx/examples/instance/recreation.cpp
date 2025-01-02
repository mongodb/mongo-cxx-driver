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

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    {
        mongocxx::instance instance;

        EXPECT(&mongocxx::instance::current() == &instance);

        try {
            mongocxx::instance another_instance; // Throws.

            EXPECT(false && "should not reach this point");
        } catch (mongocxx::exception const& ex) {
            EXPECT(ex.code() == mongocxx::error_code::k_cannot_recreate_instance);
        }

        EXPECT(&mongocxx::instance::current() == &instance);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_FORKING_COMPONENT() {
    example();
}
