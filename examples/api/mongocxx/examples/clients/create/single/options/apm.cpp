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

#include <cstring>

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/events/command_started_event.hpp>
#include <mongocxx/options/apm.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void on_command_started_callback(mongocxx::events::command_started_event const& event);

void example() {
    mongocxx::options::apm apm_opts;

    apm_opts.on_command_started(&on_command_started_callback);
    // ... other APM options.

    mongocxx::options::client client_opts;
    client_opts.apm_opts(apm_opts);

    mongocxx::client client{mongocxx::uri{}, client_opts};

    EXPECT(client);
}
// [Example]

void on_command_started_callback(mongocxx::events::command_started_event const& event) {
    (void)event;
}

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
