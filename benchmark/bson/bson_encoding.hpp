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
#include "../microbench.hpp"

namespace benchmark {

class bson_encoding : public microbench {
   public:
    // TODO: need to wait for scoring object to be finished to implement constructor
    bson_encoding() : microbench{10000} {}

    void setup(bsoncxx::stdx::string_view);

   protected:
    void task();

   private:
    std::string _json;
};

void bson_encoding::setup(bsoncxx::stdx::string_view json_file) {
    _json = parse_json_file_to_strings(json_file)[0];
}

void bson_encoding::task() {
    for (std::uint32_t i = 0; i < 10000; i++) {
        bsoncxx::document::value doc = bsoncxx::from_json(bsoncxx::stdx::string_view{_json});
    }
}
}