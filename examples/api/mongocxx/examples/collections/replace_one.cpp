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
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [
//   {"x": 1, "replaced": false},
//   {"x": 2, "replaced": false},
//   {"x": 3, "replaced": false},
// ]
void example(mongocxx::collection coll) {
    auto is_original = bsoncxx::from_json(R"({"replaced": false})");
    auto is_replaced = bsoncxx::from_json(R"({"replaced": true})");

    auto x0 = bsoncxx::from_json(R"({"x": 0, "replaced": true})");

    // Basic usage.
    {
        EXPECT(coll.count_documents(x0.view()) == 0);

        auto filter = bsoncxx::from_json(R"({"x": 2})");

        auto result_opt = coll.replace_one(filter.view(), x0.view());

        EXPECT(result_opt);

        mongocxx::result::replace_one& result = *result_opt;

        EXPECT(result.matched_count() == 1);
        EXPECT(result.modified_count() == 1);
        EXPECT(result.result().matched_count() == 1);
        EXPECT(result.result().modified_count() == 1);

        EXPECT(coll.count_documents(x0.view()) == 1);
    }

    // With options.
    {
        EXPECT(coll.count_documents(is_original.view()) == 2);
        EXPECT(coll.count_documents(is_replaced.view()) == 1);

        mongocxx::options::replace opts;

        opts.upsert(true);
        // ... other replace options.

        auto filter = bsoncxx::from_json(R"({"replaced": false})");

        EXPECT(coll.replace_one(filter.view(), x0.view(), opts));
        EXPECT(coll.count_documents(is_original.view()) == 1);
        EXPECT(coll.count_documents(is_replaced.view()) == 2);

        EXPECT(coll.replace_one(filter.view(), x0.view(), opts));
        EXPECT(coll.count_documents(is_original.view()) == 0);
        EXPECT(coll.count_documents(is_replaced.view()) == 3);

        EXPECT(coll.replace_one(filter.view(), x0.view(), opts));
        EXPECT(coll.count_documents(is_original.view()) == 0);
        EXPECT(coll.count_documents(is_replaced.view()) == 4);
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
                   .append(insert(bsoncxx::from_json(R"({"x": 1, "replaced": false})")))
                   .append(insert(bsoncxx::from_json(R"({"x": 2, "replaced": false})")))
                   .append(insert(bsoncxx::from_json(R"({"x": 3, "replaced": false})")))
                   .execute());

        example(coll);
    }
}
