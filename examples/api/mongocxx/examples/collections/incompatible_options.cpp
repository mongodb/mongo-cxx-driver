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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::collection coll) {
    mongocxx::options::find_one_and_update opts;

    opts.collation(bsoncxx::from_json(R"({"locale": "simple"})"));

    {
        mongocxx::write_concern wc;
        wc.acknowledge_level(mongocxx::write_concern::level::k_unacknowledged);
        opts.write_concern(wc);
    }

    auto empty = bsoncxx::builder::basic::make_document();

    try {
        auto name = coll.find_one_and_update(empty.view(), empty.view(), opts);

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        EXPECT(ex.code() == mongocxx::error_code::k_invalid_parameter);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    {
        db_lock guard{client, EXAMPLES_COMPONENT_NAME_STR};

        example(set_rw_concern_majority(guard.get()).create_collection("coll"));
    }
}
