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

#include "score_recorder.hpp"

#include <algorithm>
#include <set>
#include <vector>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>

namespace benchmark {

enum class benchmark_type {
    bson_bench,
    single_bench,
    multi_bench,
    parallel_bench,
    read_bench,
    write_bench,
    driver_bench,
    all_benchmarks
};

const std::chrono::milliseconds mintime{60000};
const std::chrono::milliseconds maxtime{300000};

class microbench {
   public:
    microbench() : _score{0} {}

    microbench(double a, std::string name = "un-named", std::set<benchmark_type> tags = {})
        : _score{a}, _tags{tags}, _name{name + std::to_string(a)} {}

    void run();

    std::string get_name() {
        return _name;
    }

    benchmark::score_recorder& get_results() {
        return _score;
    }

    bool has_tag(benchmark_type tag) {
        return _tags.find(tag) != _tags.end();
    }

   protected:
    virtual void setup() {}

    virtual void before_task() {}

    virtual void task() {}

    virtual void after_task() {}

    virtual void teardown() {}

    benchmark::score_recorder _score;
    std::set<benchmark_type> _tags;
    std::string _name;
};

std::vector<std::string> parse_json_file_to_strings(bsoncxx::stdx::string_view json_file);

std::vector<bsoncxx::document::value> parse_json_file_to_documents(
    bsoncxx::stdx::string_view json_file);

std::vector<std::string> parse_documents_to_bson(const std::vector<bsoncxx::document::value>& docs);
}
