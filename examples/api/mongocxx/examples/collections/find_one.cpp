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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [
//   {"x": 1, "found": false},
//   {"x": 2, "found": true},
//   {"x": 3, "found": false},
// ]
void example(mongocxx::collection coll) {
    // Basic usage.
    {
        auto filter = bsoncxx::from_json(R"({"found": true})");

        auto result_opt = coll.find_one(filter.view());

        EXPECT(result_opt);

        bsoncxx::document::view doc = result_opt->view();

        EXPECT(doc["_id"]);

        EXPECT(doc["x"]);
        EXPECT(doc["x"].get_int32().value == 2);
    }

    // With options.
    {
        mongocxx::options::find opts;

        opts.projection(bsoncxx::from_json(R"({"_id": 0, "x": 1})"));
        opts.sort(bsoncxx::from_json(R"({"x": 1})"));
        // ... other find options.

        auto result_opt = coll.find_one(bsoncxx::builder::basic::make_document(), opts);

        EXPECT(result_opt);
        EXPECT(*result_opt == bsoncxx::from_json(R"({"x": 1})"));
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
                   .append(insert{bsoncxx::from_json(R"({"x": 1, "found": false})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 2, "found": true})")})
                   .append(insert{bsoncxx::from_json(R"({"x": 3, "found": false})")})
                   .execute());

        example(coll);
    }
}
