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

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::database db) {
    mongocxx::collection coll = db.create_collection("coll");

    mongocxx::change_stream stream = coll.watch();

    auto result_opt = coll.insert_one(bsoncxx::from_json(R"({"x": 1})"));
    EXPECT(result_opt);
    auto id = result_opt->inserted_id();

    int count = 0;
    auto now = [] { return std::chrono::steady_clock::now(); };
    auto start = now();

    // periodicNoopIntervalSecs: 10 (default)
    while (count < 1 && now() - start < std::chrono::seconds(10)) {
        for (bsoncxx::document::view change : stream) {
            ++count;

            EXPECT(change["operationType"]);
            EXPECT(change["operationType"].get_string().value == "insert");

            EXPECT(change["ns"]);
            EXPECT(change["ns"]["db"].get_string().value == db.name());
            EXPECT(change["ns"]["coll"].get_string().value == coll.name());

            EXPECT(change["fullDocument"]);
            EXPECT(change["fullDocument"]["x"]);

            EXPECT(change["documentKey"]);
            EXPECT(change["documentKey"]["_id"].get_oid().value == id);
        }
    }

    EXPECT(count == 1);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_REPLICA() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        example(set_rw_concern_majority(guard.get()));
    }
}
