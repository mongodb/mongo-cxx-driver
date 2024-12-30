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
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    {
        mongocxx::pool pool;

        mongocxx::pool::entry entry = pool.acquire();

        EXPECT(entry);

        mongocxx::client& client = *entry;

        EXPECT(client);
        EXPECT(client.uri().to_string() == mongocxx::uri::k_default_uri);
    }

    {
        mongocxx::uri uri{"mongodb://localhost:27017/?appName=example"};
        mongocxx::pool pool{uri};

        mongocxx::pool::entry entry = pool.acquire();

        EXPECT(entry);

        mongocxx::client& client = *entry;

        EXPECT(client);
        EXPECT(client.uri().to_string() == uri.to_string());
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
