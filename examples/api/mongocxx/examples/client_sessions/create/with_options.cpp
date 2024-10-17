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

#include <mongocxx/client.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/options/client_session.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::client client) {
    mongocxx::options::client_session opts;

    opts.snapshot(true);
    // ... other client session options.

    mongocxx::client_session session = client.start_session(opts);

    EXPECT(session.options().snapshot() == true);
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    example(mongocxx::client{mongocxx::uri{}});
}