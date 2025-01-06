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
    using wc_level = mongocxx::write_concern::level;

    // Default.
    {
        mongocxx::write_concern wc = coll.write_concern();

        EXPECT(wc.acknowledge_level() == wc_level::k_default);
        EXPECT(wc.timeout() == std::chrono::milliseconds(0));
    }

    // Explicit.
    {
        mongocxx::write_concern wc;

        wc.majority(std::chrono::milliseconds(5000));
        // ... other write concern options.

        coll.write_concern(wc);

        EXPECT(coll.write_concern() == wc);
        EXPECT(coll.write_concern().acknowledge_level() == wc_level::k_majority);
        EXPECT(coll.write_concern().timeout() == std::chrono::seconds(5));
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    mongocxx::client client{mongocxx::uri{}};

    example(client["db"]["coll"]);
}
