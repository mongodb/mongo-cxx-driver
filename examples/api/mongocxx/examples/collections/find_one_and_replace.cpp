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

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [
//   {"x": 1},
//   {"x": 2},
//   {"x": 3},
// ]
void example(mongocxx::collection coll) {
    auto x0 = bsoncxx::from_json(R"({"x": 0})");
    auto x1 = bsoncxx::from_json(R"({"x": 1})");
    auto x2 = bsoncxx::from_json(R"({"x": 2})");
    auto x3 = bsoncxx::from_json(R"({"x": 3})");

    // Basic usage.
    {
        EXPECT(coll.count_documents(x0.view()) == 0);

        auto result_opt = coll.find_one_and_replace(x2.view(), x0.view());

        EXPECT(result_opt);

        bsoncxx::document::view doc = result_opt->view();

        EXPECT(doc["_id"]);

        EXPECT(doc["x"]);
        EXPECT(doc["x"].get_int32().value == 2);

        EXPECT(coll.count_documents(x0.view()) == 1);
    }

    // With options.
    {
        EXPECT(coll.count_documents(x0.view()) == 1);

        mongocxx::options::find_one_and_replace opts;

        opts.projection(bsoncxx::from_json(R"({"_id": 0, "x": 1})"));
        // ... other findOneAndReplace options.

        auto result_opt = coll.find_one_and_replace(x3.view(), x0.view(), opts);

        EXPECT(result_opt);
        EXPECT(*result_opt == x3);

        EXPECT(coll.count_documents(x0.view()) == 2);
    }

    EXPECT(coll.count_documents(x1.view()) == 1);
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
