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
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::collection coll) {
    auto x1 = bsoncxx::from_json(R"({"x": 1})");
    auto x2 = bsoncxx::from_json(R"({"x": 2})");

    // Basic usage.
    {
        EXPECT(coll.count_documents(x1.view()) == 0);

        auto result_opt = coll.insert_one(x1.view());

        EXPECT(result_opt);

        mongocxx::result::insert_one& result = *result_opt;

        EXPECT(result.result().inserted_count() == 1);

        EXPECT(coll.count_documents(x1.view()) == 1);
    }

    // With options.
    {
        EXPECT(coll.count_documents(x2.view()) == 0);

        mongocxx::options::insert opts;

        // ... set insert options.

        EXPECT(coll.insert_one(x2.view(), opts));

        EXPECT(coll.count_documents(x2.view()) == 1);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        example(set_rw_concern_majority(guard.get().create_collection("coll")));
    }
}
