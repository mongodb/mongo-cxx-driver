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

#include <chrono>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/options/index_view.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::collection coll) {
    // Index options.
    {
        auto opts = bsoncxx::from_json(R"({"name": "custom_name", "unique": true})");
        // ... other index options.

        bsoncxx::document::value result =
            coll.create_index(bsoncxx::from_json(R"({"key_a": 1})"), opts.view());

        EXPECT(result["name"]);
        EXPECT(result["name"].get_string().value == "custom_name");
    }

    // Create index options.
    {
        auto opts = bsoncxx::builder::basic::make_document();

        mongocxx::options::index_view create_opts;

        // ... set create index options.

        bsoncxx::document::value result =
            coll.create_index(bsoncxx::from_json(R"({"key_b": 1})"), opts.view());

        EXPECT(result["name"]);
        EXPECT(result["name"].get_string().value == "key_b_1");
    }
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto db = set_rw_concern_majority(guard.get());

        example(db.create_collection("coll"));

        for (auto doc : db["coll"].list_indexes()) {
            EXPECT(doc["name"]);

            if (doc["name"].get_string().value == "custom_name") {
                EXPECT(doc["unique"]);
                EXPECT(doc["unique"].get_bool().value == true);
            }
        }
    }
}
