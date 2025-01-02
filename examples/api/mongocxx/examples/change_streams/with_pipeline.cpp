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

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::database db) {
    mongocxx::collection coll = db.create_collection("coll");

    mongocxx::pipeline pipeline;

    pipeline.match(bsoncxx::from_json(R"({"operationType": "insert", "fullDocument.watched": true})"));
    // ... other pipeline options.

    mongocxx::change_stream stream = coll.watch(pipeline);

    // Observed.
    EXPECT(coll.insert_one(bsoncxx::from_json(R"({"x": 1, "watched": true})")));

    // Not observed (fullDocument mismatch).
    EXPECT(coll.insert_one(bsoncxx::from_json(R"({"x": 2, "watched": false})")));

    // Not observed (operationType mismatch).
    EXPECT(coll.update_one(bsoncxx::from_json(R"({"x": 2})"), bsoncxx::from_json(R"({"$set": {"watched": true}})")));

    // Observed.
    EXPECT(coll.insert_one(bsoncxx::from_json(R"({"x": 3, "watched": true})")));

    int count = 0;
    auto now = [] { return std::chrono::steady_clock::now(); };
    auto start = now();

    // periodicNoopIntervalSecs: 10 (default)
    while (count < 2 && now() - start < std::chrono::seconds(10)) {
        for (bsoncxx::document::view change : stream) {
            ++count;
        }
    }

    EXPECT(count == 2);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_REPLICA() {
    mongocxx::client client{mongocxx::uri{}};

    try {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto db = set_rw_concern_majority(guard.get());
        auto coll = db["coll"];

        example(db);

        EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": {"$exists": 1}})")) == 3);

        auto doc_opt = coll.find_one(bsoncxx::from_json(R"({"x": 2})"));

        EXPECT(doc_opt);

        auto& doc = *doc_opt;

        EXPECT(doc["watched"]);
        EXPECT(doc["watched"].get_bool().value);
    } catch (mongocxx::exception const& ex) {
        if (std::strstr(ex.what(), "not supported") != nullptr) {
            // MongoDB 4.2+ required for sharded clusters.
        } else {
            throw;
        }
    }
}
