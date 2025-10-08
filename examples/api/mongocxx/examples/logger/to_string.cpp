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

#include <utility>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    bsoncxx::stdx::string_view error = mongocxx::to_string(mongocxx::log_level::k_error);
    bsoncxx::stdx::string_view critical = mongocxx::to_string(mongocxx::log_level::k_critical);
    bsoncxx::stdx::string_view warning = mongocxx::to_string(mongocxx::log_level::k_warning);
    bsoncxx::stdx::string_view message = mongocxx::to_string(mongocxx::log_level::k_message);
    bsoncxx::stdx::string_view info = mongocxx::to_string(mongocxx::log_level::k_info);
    bsoncxx::stdx::string_view debug = mongocxx::to_string(mongocxx::log_level::k_debug);
    bsoncxx::stdx::string_view trace = mongocxx::to_string(mongocxx::log_level::k_trace);

    EXPECT(error == "error");
    EXPECT(critical == "critical");
    EXPECT(warning == "warning");
    EXPECT(message == "message");
    EXPECT(info == "info");
    EXPECT(debug == "debug");
    EXPECT(trace == "trace");
}
// [Example]

} // namespace

RUNNER_REGISTER_FORKING_COMPONENT() {
    example();
}
