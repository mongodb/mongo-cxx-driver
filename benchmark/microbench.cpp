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
// limitations under the mLicense.

#include "microbench.hpp"

#include <fstream>
#include <string>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

namespace benchmark {

bool finished_running(const std::chrono::duration<std::uint32_t, std::milli>& curr_time,
                      std::uint32_t iter) {
    return (curr_time > maxtime || (curr_time > mintime && iter > 100));
}

void microbench::run() {
    setup();

    double count = 0;
    std::uint32_t iteration = 0;
    for (iteration = 0; !finished_running(_score.get_execution_time(), iteration); iteration++) {
        before_task();

        _score.start_sample();
        task();
        _score.end_sample();

        after_task();
    }
    teardown();

    std::cout << iteration << " iterations" << std::endl;
    std::cout << _name << ": " << _score.get_score() << "MB/s" << std::endl;
}

std::vector<std::string> parse_json_file_to_strings(bsoncxx::stdx::string_view json_file) {
    std::vector<std::string> jsons;
    std::ifstream stream{json_file.to_string()};
    while (!stream.eof()) {
        std::string s;
        std::getline(stream, s);
        jsons.push_back(s);
    }
    return jsons;
}

std::vector<bsoncxx::document::value> parse_json_file_to_documents(
    bsoncxx::stdx::string_view json_file) {
    std::vector<bsoncxx::document::value> docs;
    std::ifstream stream{json_file.to_string()};
    while (stream.is_open() && !stream.eof()) {
        std::string s;
        std::getline(stream, s);

        if (s.length() > 0) {
            docs.push_back(bsoncxx::from_json(bsoncxx::stdx::string_view{s}));
        }
    }
    return docs;
}

std::vector<std::string> parse_documents_to_bson(
    const std::vector<bsoncxx::document::value>& docs) {
    std::vector<std::string> bsons;
    for (std::uint32_t i = 0; i < docs.size(); i++) {
        bsons.push_back(bsoncxx::to_json(docs[i]));
    }
    return bsons;
}
}