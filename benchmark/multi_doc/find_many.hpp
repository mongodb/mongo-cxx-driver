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

#pragma once

#include "../microbench.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace benchmark {

using bsoncxx::builder::basic::concatenate;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class find_many : public microbench {
   public:
    // The task size comes from the Driver Perfomance Benchmarking Reference Doc.
    find_many(std::string json_file)
        : microbench{"TestFindManyAndEmptyCursor", 16.22, std::set<benchmark_type>{benchmark_type::multi_bench, benchmark_type::read_bench}},
          _conn{mongocxx::uri{}},
          _json_file{std::move(json_file)} {}

    void setup();

    void teardown();

   protected:
    void task();

   private:
    mongocxx::client _conn;
    std::string _json_file;
};

void find_many::setup() {
    auto doc = parse_json_file_to_documents(_json_file)[0];
    mongocxx::database db = _conn["perftest"];
    db.drop();
    auto coll = db["corpus"];
    for (std::int32_t i = 0; i < iterations; i++) {
        coll.insert_one(doc.view());
    }
}

void find_many::teardown() {
    _conn["perftest"].drop();
}

void find_many::task() {
    auto coll = _conn["perftest"]["corpus"];
    auto cursor = coll.find({});

    // Iterate over the cursor.
    for ([[maybe_unused]] auto&& doc : cursor) {
    }
}
} // namespace benchmark
