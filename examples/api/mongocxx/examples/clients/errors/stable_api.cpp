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
#include <mongocxx/options/server_api.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    try {
        mongocxx::options::server_api::version version =
            mongocxx::options::server_api::version_from_string("0"); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        EXPECT(ex.code() == mongocxx::error_code::k_invalid_parameter);
    }

    try {
        std::string version = mongocxx::options::server_api::version_to_string(
            static_cast<mongocxx::options::server_api::version>(1)); // Throws.

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        EXPECT(ex.code() == mongocxx::error_code::k_invalid_parameter);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
