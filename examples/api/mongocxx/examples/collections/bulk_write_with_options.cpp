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

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/options/bulk_write.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/write_concern.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::collection coll) {
    EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": {"$exists": 1}})")) == 0);

    mongocxx::options::bulk_write opts;

    opts.ordered(true);
    // ... other bulk write options.

    auto result_opt =
        coll.create_bulk_write(opts)
            .append(mongocxx::model::insert_one{bsoncxx::from_json(R"({"x": 10})")})
            .append(mongocxx::model::update_one{
                bsoncxx::from_json(R"({"x": {"$exists": 1}})"), bsoncxx::from_json(R"({"$set": {"x": 20}})")})
            .execute();

    EXPECT(result_opt);

    mongocxx::result::bulk_write& result = *result_opt;

    EXPECT(result.inserted_count() == 1);
    EXPECT(result.modified_count() == 1);

    EXPECT(coll.count_documents(bsoncxx::from_json(R"({"x": 20})")) == 1);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto coll = set_rw_concern_majority(guard.get().create_collection("coll"));

        example(coll);
    }
}
