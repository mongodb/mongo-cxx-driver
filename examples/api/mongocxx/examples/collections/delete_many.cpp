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
#include <mongocxx/result/delete.hpp>
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
    auto x1 = bsoncxx::from_json(R"({"x": 1})");
    auto x2 = bsoncxx::from_json(R"({"x": 2})");
    auto x3 = bsoncxx::from_json(R"({"x": 3})");

    // Basic usage.
    {
        EXPECT(coll.count_documents(x1.view()) == 1);
        EXPECT(coll.count_documents(x2.view()) == 1);
        EXPECT(coll.count_documents(x3.view()) == 1);

        auto result_opt = coll.delete_many(bsoncxx::from_json(R"({"x": {"$gt": 1}})"));

        EXPECT(result_opt);

        mongocxx::result::delete_result& result = *result_opt;

        EXPECT(result.deleted_count() == 2);
        EXPECT(result.result().deleted_count() == 2);

        EXPECT(coll.count_documents(x1.view()) == 1);
        EXPECT(coll.count_documents(x2.view()) == 0);
        EXPECT(coll.count_documents(x3.view()) == 0);
    }

    // With options.
    {
        EXPECT(coll.count_documents(x1.view()) == 1);

        mongocxx::options::delete_options opts;

        // ... set delete options.

        EXPECT(coll.delete_many(bsoncxx::from_json(R"({"x": {"$exists": 1}})"), opts));

        EXPECT(coll.count_documents(x1.view()) == 0);
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
