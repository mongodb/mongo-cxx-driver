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

#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

namespace benchmark {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class json_multi_import : public microbench {
   public:
    static std::uint32_t const TOTAL_FILES{100};

    json_multi_import() = delete;

    // The task size comes from the Driver Perfomance Benchmarking Reference Doc.
    json_multi_import(std::string dir, std::uint32_t thread_num = std::thread::hardware_concurrency() * 2)
        : microbench{"TestJsonMultiImport", 565, std::set<benchmark_type>{benchmark_type::parallel_bench, benchmark_type::write_bench}},
          _directory{std::move(dir)},
          _pool{mongocxx::uri{}},
          _thread_num{thread_num} {}

    void setup();

    void before_task();

    void teardown();

   protected:
    void task();

   private:
    void concurrency_task(std::uint32_t start_file, std::uint32_t num_files);

    std::string _directory;
    mongocxx::pool _pool;
    std::uint32_t _thread_num;
};
void json_multi_import::setup() {
    auto conn = _pool.acquire();
    mongocxx::database db = (*conn)["perftest"];
    db.drop();
}

void json_multi_import::before_task() {
    auto conn = _pool.acquire();
    (*conn)["perftest"]["corpus"].drop();
    (*conn)["perftest"].create_collection("corpus");
}

void json_multi_import::teardown() {
    auto conn = _pool.acquire();
    (*conn)["perftest"].drop();
}

void json_multi_import::task() {
    std::div_t result = std::div(static_cast<std::int32_t>(TOTAL_FILES), static_cast<std::int32_t>(_thread_num));
    std::uint32_t num_each = static_cast<std::uint32_t>(result.quot);
    if (result.rem != 0) {
        num_each++;
    }

    std::vector<std::thread> threads;
    for (std::uint32_t i = 0; i < TOTAL_FILES; i += num_each) {
        threads.push_back(
            std::thread{[i, num_each, this] { concurrency_task(i, std::min(TOTAL_FILES - i, num_each)); }});
    }
    for (std::uint32_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

void json_multi_import::concurrency_task(std::uint32_t start_file, std::uint32_t num_files) {
    auto client = _pool.acquire();
    mongocxx::options::insert ins_opts;
    ins_opts.ordered(false);
    for (std::uint32_t i = start_file; i < start_file + num_files; i++) {
        std::stringstream ss;
        ss << _directory << "/ldjson" << std::setfill('0') << std::setw(3) << i << ".txt";

        std::vector<bsoncxx::document::value> docs = parse_json_file_to_documents(ss.str());

        (*client)["perftest"]["corpus"].insert_many(docs, ins_opts);
    }
}
}  // namespace benchmark
