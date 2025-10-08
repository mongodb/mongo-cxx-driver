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

#include <cstring>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::client_session session, mongocxx::collection coll) {
    using bsoncxx::builder::basic::make_document;

    using mongocxx::model::insert_one;

    auto x0 = bsoncxx::from_json(R"({"x": 0})");
    auto inc = bsoncxx::from_json(R"({"$inc": {"x": 1}})");

    session.start_transaction();

    {
        auto result_opt = coll.create_bulk_write(session)
                              .append(insert_one{x0.view()})
                              .append(insert_one{x0.view()})
                              .append(insert_one{x0.view()})
                              .execute();

        EXPECT(result_opt);
        EXPECT(result_opt->inserted_count() == 3);
    }

    {
        auto result_opt = coll.update_many(session, bsoncxx::from_json(R"({"x": {"$exists": 1}})"), inc.view());

        EXPECT(result_opt);
        EXPECT(result_opt->modified_count() == 3);
    }

    session.commit_transaction();

    EXPECT(coll.count_documents(session, bsoncxx::from_json(R"({"x": 1})")) == 3);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_REPLICA() {
    mongocxx::client client{mongocxx::uri{}};

    try {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        example(client.start_session(), set_rw_concern_majority(guard.get()).create_collection("coll"));
    } catch (mongocxx::exception const& ex) {
        if (std::strstr(ex.what(), "not supported") != nullptr) {
            // MongoDB 4.2+ required for sharded clusters.
        } else {
            throw;
        }
    }
}
