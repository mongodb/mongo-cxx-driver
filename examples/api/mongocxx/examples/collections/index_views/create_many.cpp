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

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/index_model.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::index_view indexes) {
    std::vector<mongocxx::index_model> models;

    models.emplace_back(bsoncxx::from_json(R"({"x": 1})"));
    models.emplace_back(bsoncxx::from_json(R"({"y": 1})"));

    auto result = indexes.create_many(models);

    // SERVER-78611
    if (result["raw"]) {
        result = result["raw"].get_document().value.begin()->get_document().value;
    }

    EXPECT(result["ok"]);
    EXPECT(result["ok"].get_double().value == 1.0);

    EXPECT(result["numIndexesBefore"]);
    EXPECT(result["numIndexesBefore"].get_int32().value == 1); // _id_

    EXPECT(result["numIndexesAfter"]);
    EXPECT(result["numIndexesAfter"].get_int32().value == 3); // _id_, x_1, y_1
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        example(set_rw_concern_majority(guard.get().create_collection("coll")).indexes());
    }
}
