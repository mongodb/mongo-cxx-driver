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

#include <mongocxx/client.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/structured_logging.hpp>
#include <mongocxx/structured_log.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    mongocxx::options::structured_logging structured_logging_opts;

    structured_logging_opts
        .handler([](mongocxx::structured_log_entry const& entry) {
            std::cout << mongocxx::to_string(entry.component()) << " [" << mongocxx::to_string(entry.level())
                      << "]: " << entry.message() << "\n";
        })
        .max_level_for_all_components(mongocxx::structured_log_level::k_warning)
        .max_level_for_component(mongocxx::structured_log_component::k_command, mongocxx::structured_log_level::k_debug);
    // ... other structured logging options.

    mongocxx::options::client client_opts;
    client_opts.structured_logging_opts(structured_logging_opts);

    mongocxx::client client{mongocxx::uri{}, client_opts};

    EXPECT(client);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
