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

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// [
//     {
//         "key": { "_id": 1 },
//         "name": "_id_"
//     {
//         "key": { "x": 1 },
//         "name": "x_1"
//     },
//     {
//         "key": { "y": 1 },
//         "name": "custom_name"
//     }
// ]
void example(mongocxx::index_view indexes) {
    auto count_indexes = [&indexes] {
        auto cursor = indexes.list();
        return std::distance(cursor.begin(), cursor.end());
    };

    EXPECT(count_indexes() == 3); // _id_, x_1, custom_name

    indexes.drop_one("custom_name");

    EXPECT(count_indexes() == 2); // _id_, x_1

    indexes.drop_one(bsoncxx::from_json(R"({"x": 1})"));

    EXPECT(count_indexes() == 1); // _id_
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        auto coll = set_rw_concern_majority(guard.get().create_collection("coll"));

        (void)coll.create_index(bsoncxx::from_json(R"({"x": 1})"));
        (void)coll.create_index(bsoncxx::from_json(R"({"y": 1})"), bsoncxx::from_json(R"({"name": "custom_name"})"));

        example(coll.indexes());
    }
}
