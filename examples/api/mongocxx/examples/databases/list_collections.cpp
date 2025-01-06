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

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::database db) {
    int a = 0;
    int b = 0;
    int c = 0;

    auto count_fields = [&a, &b, &c](mongocxx::cursor cursor) {
        a = 0;
        b = 0;
        c = 0;

        for (bsoncxx::document::view doc : cursor) {
            EXPECT(doc["name"]);

            auto name = bsoncxx::string::to_string(doc["name"].get_string().value);

            if (name == "a") {
                ++a;
            } else if (name == "b") {
                ++b;
            } else if (name == "c") {
                ++c;
            }
        }
    };

    // Basic usage.
    {
        count_fields(db.list_collections());

        EXPECT(a == 1); // Present.
        EXPECT(b == 1); // Present.
        EXPECT(c == 0); // Missing.
    }

    // With a filter.
    {
        auto filter = bsoncxx::from_json(R"({"name": {"$ne": "b"}})");

        count_fields(db.list_collections(filter.view()));

        EXPECT(a == 1); // Present.
        EXPECT(b == 0); // Filtered.
        EXPECT(c == 0); // Missing.
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto db = guard.get();

        (void)db.create_collection("a");
        (void)db.create_collection("b");
        // (void)db.create_collection("c");

        example(db);
    }
}
