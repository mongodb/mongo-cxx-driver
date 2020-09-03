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

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

int main(int, char**) {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto db = conn["test"];

    // Query for all the documents in a collection.
    {
        // @begin: cpp-query-all
        auto cursor = db["restaurants"].find({});
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-all
    }

    // Query for equality on a top level field.
    {
        // @begin: cpp-query-top-level-field
        auto cursor = db["restaurants"].find(make_document(kvp("borough", "Manhattan")));

        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-top-level-field
    }

    // Query by a field in an embedded document.
    {
        // @begin: cpp-query-embedded-document
        auto cursor = db["restaurants"].find(make_document(kvp("address.zipcode", "10075")));
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-embedded-document
    }

    // Query by a field in an array.
    {
        // @begin: cpp-query-field-in-array
        auto cursor = db["restaurants"].find(make_document(kvp("grades.grade", "B")));
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-field-in-array
    }

    // Query with the greater-than operator ($gt).
    {
        // @begin: cpp-query-greater-than
        auto cursor = db["restaurants"].find(
            make_document(kvp("grade.score", make_document(kvp("$gt", 30)))));
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-greater-than
    }

    // Query with the less-than operator ($lt).
    {
        // @begin: cpp-query-less-than
        auto cursor = db["restaurants"].find(
            make_document(kvp("grades.score", make_document(kvp("$lt", 10)))));
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-less-than
    }

    // Query with a logical conjunction (AND) of query conditions.
    {
        // @begin: cpp-query-logical-and
        auto cursor = db["restaurants"].find(
            make_document(kvp("cuisine", "Italian"), kvp("address.zipcode", "10075")));
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-logical-and
    }

    // Query with a logical disjunction (OR) of query conditions.
    {
        // @begin: cpp-query-logical-or
        auto cursor = db["restaurants"].find(
            make_document(kvp("$or",
                              make_array(make_document(kvp("cuisine", "Italian")),
                                         make_document(kvp("address.zipcode", "10075"))))));
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-logical-or
    }

    // Sort query results.
    {
        // @begin: cpp-query-sort
        mongocxx::options::find opts;
        opts.sort(make_document(kvp("borough", 1), kvp("address.zipcode", -1)));

        auto cursor = db["restaurants"].find({}, opts);
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-sort
    }
}
