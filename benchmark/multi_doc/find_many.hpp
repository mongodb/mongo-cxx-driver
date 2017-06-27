// Copyright 2017 MongoDB Inc.
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

#pragma once

#include "../microbench.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace benchmark {

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::concatenate;

class find_many : public microbench {
   public:
    find_many() : microbench{1}, _conn{mongocxx::uri{}} {}

    void setup(bsoncxx::stdx::string_view);

    void teardown();

   protected:
    void task();

   private:
    mongocxx::client _conn;
};

void find_many::setup(bsoncxx::stdx::string_view json_file) {
    auto doc = parse_json_file_to_documents(json_file)[0];
    mongocxx::database db = _conn["perftest"];
    db.drop();
    auto coll = db["corpus"];
    for (std::int32_t i = 0; i < 10000; i++) {
        coll.insert_one(doc.view());
    }
}

void find_many::teardown() {
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void find_many::task() {
    auto coll = _conn["perftest"]["corpus"];
    auto cursor = coll.find({});

    // Iterate over the cursor.
    for (auto&& doc : cursor) {
    }
}
}