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

#include <cstdlib>
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    // @begin: cpp-logic-error
    // Using an uninitialized collection throws a mongocxx::logic_error.
    // A mongocxx::logic_error is-a mongocxx::exception is-a std::system_error.
    mongocxx::collection coll;
    try {
        coll.name();
    } catch (mongocxx::logic_error& e) {

        // We can compare e.code() to a known std::error_code.
        if (e.code() != mongocxx::invalid_collection_object_error()) {
            return EXIT_FAILURE;
        }

        std::cout << "Using an uninitialized collection throws:" << std::endl;
        std::cout << e.what() << std::endl << std::endl;
    }
    // @end: cpp-logic-error

    // @begin: cpp-operation-exception
    // Renaming a collection that does not exist throws a mongocxx::operation_exception.
    // A mongocxx::operation_exception is-a mongocxx::exception is-a std::system_error.
    coll = conn["test"]["coll"];
    coll.drop();
    try {
        coll.rename("coll2");
    } catch (mongocxx::operation_exception& e) {
        std::cout << "Renaming a collection that does not exist throws:" << std::endl;
        std::cout << e.what() << std::endl;
        if (e.raw_server_error()) {
            std::cout << bsoncxx::to_json(*(e.raw_server_error())) << std::endl;
        }
        std::cout << std::endl;
    }
    // @end: cpp-operation-exception

    // @begin: cpp-bulk-write-exception
    // Adding a document whose "_id" is already present throws a mongocxx::bulk_write_exception.
    // A mongocxx::bulk_write_exception is-a mongocxx::operation_exception is-a mongocxx::exception
    // is-a std::system_error.
    auto doc1 = document{} << "_id" << 1 << finalize;
    coll.insert_one(doc1.view());
    try {
        coll.insert_one(doc1.view());
    } catch (mongocxx::bulk_write_exception& e) {
        std::cout << "Adding a document whose _id is already present throws:" << std::endl;
        std::cout << e.what() << std::endl;
        if (e.raw_server_error()) {
            std::cout << "Raw server error:" << std::endl;
            std::cout << bsoncxx::to_json(*(e.raw_server_error())) << std::endl;
        }
        std::cout << std::endl;
    }
    // @end: cpp-bulk-write-exception

    return EXIT_SUCCESS;
}
