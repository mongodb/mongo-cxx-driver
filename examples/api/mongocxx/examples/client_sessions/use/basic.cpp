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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::client_session session, mongocxx::database db) {
    using bsoncxx::builder::basic::make_document;

    mongocxx::collection coll = db.create_collection(session, "coll");

    auto x1 = bsoncxx::from_json(R"({"x": 1})");

    EXPECT(coll.insert_one(session, x1.view()));
    EXPECT(coll.update_one(session, x1.view(), bsoncxx::from_json(R"({"$inc": {"x": 1}})")));

    auto doc_opt = coll.find_one(session, make_document());

    EXPECT(doc_opt);

    auto& doc = *doc_opt;

    EXPECT(doc["x"]);
    EXPECT(doc["x"].get_int32().value == 2);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        example(client.start_session(), set_rw_concern_majority(guard.get()));
    }
}
