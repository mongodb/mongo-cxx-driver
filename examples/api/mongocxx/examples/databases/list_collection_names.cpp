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

#include <algorithm>
#include <string>
#include <vector>

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::database db) {
    // Basic usage.
    {
        std::vector<std::string> names = db.list_collection_names();

        EXPECT(std::count(names.begin(), names.end(), "a") == 1); // Present.
        EXPECT(std::count(names.begin(), names.end(), "b") == 1); // Present.
        EXPECT(std::count(names.begin(), names.end(), "c") == 0); // Missing.
    }

    // With a filter.
    {
        auto filter = bsoncxx::from_json(R"({"name": {"$ne": "b"}})");

        std::vector<std::string> names = db.list_collection_names(filter.view());

        EXPECT(std::count(names.begin(), names.end(), "a") == 1); // Present.
        EXPECT(std::count(names.begin(), names.end(), "b") == 0); // Filtered.
        EXPECT(std::count(names.begin(), names.end(), "c") == 0); // Missing.
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto db = set_rw_concern_majority(guard.get());

        (void)db.create_collection("a");
        (void)db.create_collection("b");
        // (void)db.create_collection("c");

        example(db);
    }
}
