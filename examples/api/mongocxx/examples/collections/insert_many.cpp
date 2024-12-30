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

#include <array>
#include <vector>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/result/insert_many.hpp>
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
    auto y1 = bsoncxx::from_json(R"({"y": 1})");
    auto y2 = bsoncxx::from_json(R"({"y": 2})");
    auto z1 = bsoncxx::from_json(R"({"z": 1})");
    auto z2 = bsoncxx::from_json(R"({"z": 2})");

    // Basic usage.
    {
        std::vector<bsoncxx::document::view> docs = {x1.view(), x2.view()};

        auto result_opt = coll.insert_many(docs);

        EXPECT(result_opt);

        mongocxx::result::insert_many& result = *result_opt;

        EXPECT(result.inserted_count() == 2);
        EXPECT(result.result().inserted_count() == 2);
    }

    // Iterators.
    {
        bsoncxx::document::view docs[] = {y1.view(), y2.view()};

        auto result_opt = coll.insert_many(std::begin(docs), std::end(docs));

        EXPECT(result_opt);

        mongocxx::result::insert_many& result = *result_opt;

        EXPECT(result.inserted_count() == 2);
        EXPECT(result.result().inserted_count() == 2);
    }

    // With options.
    {
        std::array<bsoncxx::document::view, 2> docs = {z1.view(), z2.view()};

        mongocxx::options::insert opts;

        // ... set insert options.

        EXPECT(coll.insert_many(docs, opts));
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
