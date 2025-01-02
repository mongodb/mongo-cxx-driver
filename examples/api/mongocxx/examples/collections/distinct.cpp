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
#include <vector>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [
//   {"x": 1, "flag": 1},
//   {"x": 2, "flag": 1},
//   {"x": 3, "flag": 0},
// ]
void example(mongocxx::collection coll) {
    // Basic usage.
    {
        mongocxx::cursor cursor = coll.distinct("x", bsoncxx::from_json(R"({"flag": 1})"));

        std::vector<bsoncxx::document::value> docs{cursor.begin(), cursor.end()};

        EXPECT(docs.size() == 1u);

        auto doc = docs[0].view();

        EXPECT(doc["values"]);
        EXPECT(doc["values"].type() == bsoncxx::type::k_array);

        auto arr = doc["values"].get_array().value;

        EXPECT(std::distance(arr.begin(), arr.end()) == 2);

        EXPECT(arr[0].get_int32().value == 1);
        EXPECT(arr[1].get_int32().value == 2);
    }

    // With options.
    {
        mongocxx::options::distinct opts;

        // ... set distinct options.

        mongocxx::cursor cursor = coll.distinct("x", bsoncxx::from_json(R"({"flag": 1})"), opts);

        EXPECT(std::distance(cursor.begin(), cursor.end()) == 1);
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
                   .append(insert{bsoncxx::from_json(R"({"x": 1, "flag": 1})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 2, "flag": 1})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 3, "flag": 0})")})
                   .execute());

        example(coll);
    }
}
