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
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::collection coll) {
    using rc_level = mongocxx::read_concern::level;

    // Default.
    {
        mongocxx::read_concern rc = coll.read_concern();

        EXPECT(rc.acknowledge_level() == rc_level::k_server_default);
    }

    // Explicit.
    {
        mongocxx::read_concern rc;

        rc.acknowledge_level(rc_level::k_majority);
        // ... other read concern options.

        coll.read_concern(rc);

        EXPECT(coll.read_concern() == rc);
        EXPECT(coll.read_concern().acknowledge_level() == rc_level::k_majority);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    mongocxx::client client{mongocxx::uri{}};

    example(client["db"]["coll"]);
}
