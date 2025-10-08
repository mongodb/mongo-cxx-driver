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
    mongocxx::uri uri{"mongodb://localhost:27017/?maxPoolSize=1&waitQueueTimeoutMS=1"};
    mongocxx::pool pool{uri};

    auto entry_opt = pool.try_acquire();

    EXPECT(entry_opt);
    EXPECT(*entry_opt);

    {
        mongocxx::pool::entry hold = std::move(*entry_opt);

        EXPECT(hold);

        entry_opt = pool.try_acquire();

        EXPECT(!entry_opt);
    }

    entry_opt = pool.try_acquire();

    EXPECT(entry_opt);
    EXPECT(*entry_opt);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    example();
}
