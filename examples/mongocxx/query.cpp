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
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
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
        auto cursor = db["restaurants"].find(document{} << "borough"
                                                        << "Manhattan" << finalize);

        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-top-level-field
    }

    // Query by a field in an embedded document.
    {
        // @begin: cpp-query-embedded-document
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "address.zipcode"
                       << "10075";

        auto cursor = db["restaurants"].find(filter_builder.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-embedded-document
    }

    // Query by a field in an array.
    {
        // @begin: cpp-query-field-in-array
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "grades.grade"
                       << "B";

        auto cursor = db["restaurants"].find(filter_builder.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-field-in-array
    }

    // Query with the greater-than operator ($gt).
    {
        // @begin: cpp-query-greater-than
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "grades.score" << open_document << "$gt" << 30 << close_document;

        auto cursor = db["restaurants"].find(filter_builder.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-greater-than
    }

    // Query with the less-than operator ($lt).
    {
        // @begin: cpp-query-less-than
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "grades.score" << open_document << "$lt" << 10 << close_document;

        auto cursor = db["restaurants"].find(filter_builder.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-less-than
    }

    // Query with a logical conjunction (AND) of query conditions.
    {
        // @begin: cpp-query-logical-and
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "cuisine"
                       << "Italian"
                       << "address.zipcode"
                       << "10075";

        auto cursor = db["restaurants"].find(filter_builder.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-logical-and
    }

    // Query with a logical disjunction (OR) of query conditions.
    {
        // @begin: cpp-query-logical-or
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "$or" << open_array << open_document << "cuisine"
                       << "Italian" << close_document << open_document << "address.zipcode"
                       << "10075" << close_document << close_array;

        auto cursor = db["restaurants"].find(filter_builder.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-logical-or
    }

    // Sort query results.
    {
        // @begin: cpp-query-sort
        mongocxx::options::find opts;
        bsoncxx::builder::stream::document order_builder;
        order_builder << "borough" << 1 << "address.zipcode" << -1;
        opts.sort(order_builder.view());

        auto cursor = db["restaurants"].find({}, opts);
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-sort
    }
}
