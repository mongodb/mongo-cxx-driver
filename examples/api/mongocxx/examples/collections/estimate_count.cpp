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
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/options/estimated_document_count.hpp>
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
    EXPECT(coll.estimated_document_count() == 3);

    // With options.
    {
        mongocxx::options::estimated_document_count opts;

        // ... set estimated count options.

        EXPECT(coll.estimated_document_count(opts) == 3);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto coll = set_rw_concern_majority(guard.get().create_collection("coll"));

        using insert = mongocxx::model::insert_one;

        EXPECT(coll.create_bulk_write()
                   .append(insert{bsoncxx::from_json(R"({"x": 1})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 2})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 3})")})
                   .execute());

        example(coll);
    }
}
