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

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [
//     {"x": 1},
//     {"x": 2},
//     {"x": 3},
// ]
void example(mongocxx::collection coll) {
    // Basic usage.
    {
        EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": {"$exists": 1}})")) == 3);
        EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": {"$gt": 1}})")) == 2);
        EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": {"$gt": 2}})")) == 1);
    }

    // With options.
    {
        mongocxx::options::count opts;

        opts.limit(1);
        // ... other count options.

        EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": {"$exists": 1}})"), opts) == 1);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto db = set_rw_concern_majority(guard.get());

        auto coll = db.create_collection("coll");

        using insert = mongocxx::model::insert_one;

        EXPECT(coll.create_bulk_write()
                   .append(insert{bsoncxx::from_json(R"({"x": 1})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 2})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 3})")})
                   .execute());

        example(coll);
    }
}
