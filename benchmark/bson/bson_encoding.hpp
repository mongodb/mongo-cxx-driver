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

#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <iostream>
#include "../microbench.hpp"

namespace benchmark {

class bson_encoding : public microbench {
   public:
    // TODO: need to wait for scoring object to be finished to implement constructor
    bson_encoding() = delete;

    bson_encoding(double task_size, bsoncxx::stdx::string_view json_file)
        : microbench{task_size,
                     "bson_encoding",
                     std::set<benchmark_type>{benchmark_type::bson_bench}},
          _json{parse_json_file_to_strings(json_file)[0]},
          _doc{bsoncxx::from_json(bsoncxx::stdx::string_view{_json})} {}

   protected:
    void task();
    void teardown();

   private:
    std::int64_t _x;
    std::string _s;
    std::string _json;
    bsoncxx::document::value _doc;
};

void bson_encoding::task() {
    // bsoncxx::stdx::string_view json_view{_json};
    // for (std::uint32_t i = 0; i < 10000; i++) {
    //    bsoncxx::from_json(bsoncxx::stdx::string_view{_json});
    //}
    _x = 0;
    for (auto&& it : _doc.view()) {
        if (it.type() == bsoncxx::type::k_utf8) {
            _s = it.get_utf8().value.to_string();
        }
    }

    // std::cout << _x << std::endl;
}

void bson_encoding::teardown() {
    std::cout << _x << std::endl;
}
}