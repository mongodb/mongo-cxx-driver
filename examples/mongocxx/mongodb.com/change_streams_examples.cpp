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

#include <atomic>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/change_stream.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include <examples/macros.hh>

namespace {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

using namespace mongocxx;

mongocxx::collection
make_test_coll(mongocxx::client& client, bsoncxx::stdx::string_view db_name, bsoncxx::stdx::string_view coll_name) {
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);

    read_concern rc_majority;
    rc_majority.acknowledge_level(read_concern::level::k_majority);

    auto db = client[db_name];
    auto coll = db[coll_name];

    coll.drop();
    coll = db.create_collection(coll_name);

    coll.write_concern(wc_majority);
    coll.read_concern(rc_majority);

    return coll;
}

void change_streams_example_1(mongocxx::client& client) {
    collection inventory = make_test_coll(client, "streams", "events");

    // Start Changestream Example 1
    change_stream stream = inventory.watch();
    auto it = stream.begin();
    while (it == stream.end()) {
        // Server returned no new notifications. Restart iteration to poll server.
        it = stream.begin();
    }
    bsoncxx::document::view next = *it;
    // End Changestream Example 1

    if (next["operationType"].get_string().value != "insert") {
        throw std::logic_error{"expected operationType to be 'insert'"};
    }
}

void change_streams_example_2(mongocxx::client& client) {
    collection inventory = make_test_coll(client, "streams", "events");

    // Start Changestream Example 2
    options::change_stream options;
    options.full_document(bsoncxx::string::view_or_value{"updateLookup"});
    change_stream stream = inventory.watch(options);
    auto it = stream.begin();
    while (it == stream.end()) {
        // Server returned no new notifications. Restart iteration to poll server.
        it = stream.begin();
    }
    bsoncxx::document::view next = *it;
    // End Changestream Example 2

    if (next["operationType"].get_string().value != "insert") {
        throw std::logic_error{"expected operationType to be 'insert'"};
    }
}

void change_streams_example_3(mongocxx::client& client) {
    collection inventory = make_test_coll(client, "streams", "events");

    bsoncxx::stdx::optional<bsoncxx::document::value> next;

    // Get one notification to set `next`.
    {
        change_stream stream = inventory.watch();
        auto it = stream.begin();
        while (it == stream.end()) {
            // Server returned no new notifications. Restart iteration to poll server.
            it = stream.begin();
        }
        next = bsoncxx::document::value(*it);
    }

    // Start Changestream Example 3
    auto resume_token = (*next)["_id"].get_document().value;
    options::change_stream options;
    options.resume_after(resume_token);
    change_stream stream = inventory.watch(options);
    auto it = stream.begin();
    while (it == stream.end()) {
        // Server returned no new notifications. Restart iteration to poll server.
        it = stream.begin();
    }
    // End Changestream Example 3

    if ((*it)["operationType"].get_string().value != "insert") {
        throw std::logic_error{"expected operationType to be 'insert'"};
    }
}

void change_streams_example_4(mongocxx::client& client) {
    collection inventory = make_test_coll(client, "streams", "events");

    // Create a pipeline with
    //  [{"$match": {"$or": [{"fullDocument.username": "alice"}, {"operationType": "delete"}]}}]

    // Start Changestream Example 4
    mongocxx::pipeline cs_pipeline;
    cs_pipeline.match(make_document(
        kvp("$or",
            make_array(
                make_document(kvp("fullDocument.username", "alice")), make_document(kvp("operationType", "delete"))))));

    change_stream stream = inventory.watch(cs_pipeline);
    auto it = stream.begin();
    while (it == stream.end()) {
        // Server returned no new notifications. Restart iteration to poll server.
        it = stream.begin();
    }
    // End Changestream Example 4

    if ((*it)["operationType"].get_string().value != "insert") {
        throw std::logic_error{"expected operationType to be 'insert'"};
    }
}

} // namespace

int EXAMPLES_CDECL main() {
    if (char const* const topology_env = std::getenv("MONGOCXX_TEST_TOPOLOGY")) {
        auto const topology = std::string(topology_env);
        if (topology != "replica") {
            std::cerr << "Skipping: change_streams example requires a replica set" << std::endl;
            return 0;
        }
    }

    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance const inst{};

    std::function<void(mongocxx::client&)> const examples[] = {
        change_streams_example_1,
        change_streams_example_2,
        change_streams_example_3,
        change_streams_example_4,
    };

    for (auto const& example : examples) {
        mongocxx::pool pool{mongocxx::uri{}};

        mongocxx::client const conn{mongocxx::uri{}};

        auto const db = conn["documentation_examples"];
        auto client = pool.acquire();

        collection inventory = make_test_coll(*client, "streams", "events");

        std::atomic_bool insert_thread_done;
        insert_thread_done.store(false);

        // Start a thread to repeatedly insert documents to generate notifications.
        auto insert_thread = std::thread{[&pool, &insert_thread_done] {
            auto client = pool.acquire();
            auto inventory = (*client)["streams"]["events"];
            while (true) {
                auto doc = make_document(kvp("username", "alice"));
                inventory.insert_one(doc.view());
                if (insert_thread_done) {
                    return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }};

        try {
            example(*client);
        } catch (std::logic_error const& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        insert_thread_done = true;
        insert_thread.join();
    }

    return EXIT_SUCCESS;
}
