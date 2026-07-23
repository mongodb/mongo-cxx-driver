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

#include <iostream>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    mongocxx::instance instance;

    // A handler may be any invocable compatible with mongocxx::log_handler.
    auto handler =
        [](mongocxx::log_level level, bsoncxx::stdx::string_view domain, bsoncxx::stdx::string_view message) {
            std::cout << mongocxx::to_string(level) << ": " << domain << ": " << message << "\n";
        };

    // the default logging behavior is active here

    {
        // Install `handler` for the duration of this scope. Unstructured log messages emitted by
        // mongoc while the guard is alive are routed to `handler` instead.
        mongocxx::logger_guard guard{handler};

        // ... perform operations whose log messages should be handled by `handler` ...

        {
            // Guards nest. Disable unstructured logging entirely for this inner scope by installing
            // a null handler.
            mongocxx::logger_guard quiet{nullptr};

            // ... perform operations whose log messages should be suppressed ...
        }

        // The inner guard has been destroyed, so `handler` is active again.
    }

    // The outer guard has been destroyed, so the default logging behavior is active again.
}
// [Example]

} // namespace

RUNNER_REGISTER_FORKING_COMPONENT() {
    example();
}
