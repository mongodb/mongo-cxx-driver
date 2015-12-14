// Copyright 2015 MongoDB Inc.
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

#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto db = conn["test"];

    // Group documents by field and calculate count.
    {
        // @begin: cpp-group-documents-by-a-field-and-calculate-count
        mongocxx::pipeline stages;
        bsoncxx::builder::stream::document group_stage;

        group_stage << "_id"
                    << "$borough"
                    << "count" << open_document << "$sum" << 1 << close_document;

        stages.group(group_stage.view());

        auto cursor = db["restaurants"].aggregate(stages);

        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-group-documents-by-a-field-and-calculate-count
    }

    // Filter and group documents.
    {
        // @begin: cpp-filter-and-group-documents
        mongocxx::pipeline stages;
        bsoncxx::builder::stream::document match_stage, group_stage;

        match_stage << "borough"
                    << "Queens"
                    << "cuisine"
                    << "Brazilian";

        group_stage << "_id"
                    << "$address.zipcode"
                    << "count" << open_document << "$sum" << 1 << close_document;

        stages.match(match_stage.view()).group(group_stage.view());

        auto cursor = db["restaurants"].aggregate(stages);

        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-filter-and-group-documents
    }
}
