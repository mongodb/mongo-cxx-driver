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

#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace benchmark {

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

class bulk_insert : public microbench {
   public:
    bulk_insert() = delete;

    bulk_insert(std::int32_t doc_num) : microbench{1}, _conn{mongocxx::uri{}}, _doc_num{doc_num} {}

    void setup(bsoncxx::stdx::string_view);

    void before_task();

    void teardown();

   protected:
    void task();

   private:
    mongocxx::client _conn;
    std::int32_t _doc_num;
    std::vector<bsoncxx::document::value> _docs;
    mongocxx::collection _coll;
};

void bulk_insert::setup(bsoncxx::stdx::string_view json_file) {
    auto doc = parse_json_file_to_documents(json_file)[0];
    for (std::int32_t i = 0; i < _doc_num; i++) {
        _docs.push_back(doc);
    }
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void bulk_insert::before_task() {
    _conn["perftest"]["corpus"].drop();
    _conn["perftest"].create_collection("corpus");
    _coll = _conn["perftest"]["corpus"];
}

void bulk_insert::teardown() {
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void bulk_insert::task() {
    _coll.insert_many(_docs);
}
}