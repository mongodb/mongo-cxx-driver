// Copyright 2014 MongoDB Inc.
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

#include "catch.hpp"
#include "helpers.hpp"

#include <mongocxx/private/libmongoc.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>

using namespace mongocxx;
using namespace bsoncxx;

TEST_CASE("Collection", "[collection]") {
    const std::string collection_name("mongocxx");
    const std::string database_name("test");

    MOCK_CLIENT
    MOCK_DATABASE
    MOCK_COLLECTION

    client mongo_client;
    database mongo_db = mongo_client[database_name];
    collection mongo_coll = mongo_db[collection_name];

    SECTION("Writes", "[collection]") {
        auto bulk_operation_new = libmongoc::bulk_operation_new.create_instance();
        auto bulk_operation_insert = libmongoc::bulk_operation_insert.create_instance();
        auto bulk_operation_remove_one = libmongoc::bulk_operation_remove_one.create_instance();
        auto bulk_operation_set_client = libmongoc::bulk_operation_set_client.create_instance();
        auto bulk_operation_set_database = libmongoc::bulk_operation_set_database.create_instance();
        auto bulk_operation_set_collection = libmongoc::bulk_operation_set_collection.create_instance();
        auto bulk_operation_execute = libmongoc::bulk_operation_execute.create_instance();
        auto bulk_operation_destroy = libmongoc::bulk_operation_destroy.create_instance();

        bool bulk_operation_new_called = false;
        bool bulk_operation_insert_called = false;
        bool bulk_operation_remove_one_called = false;
        bool bulk_operation_set_client_called = false;
        bool bulk_operation_set_database_called = false;
        bool bulk_operation_set_collection_called = false;
        bool bulk_operation_execute_called = false;
        bool bulk_operation_destroy_called = false;

        auto test_doc = builder::stream::document{} 
            << "_id" << "wow" << "foo" << "bar"
        << builder::stream::finalize;

        SECTION("Insert One", "[collection::insert_one]") {
            bool expected_order_setting = false;

            bulk_operation_new->interpose([&](bool ordered) -> mongoc_bulk_operation_t* {
                bulk_operation_new_called = true;
                REQUIRE(ordered == expected_order_setting);
                return nullptr;
            });

            bulk_operation_insert->interpose([&](mongoc_bulk_operation_t* bulk, const bson_t* doc) {
                bulk_operation_insert_called = true;
                REQUIRE(bson_get_data(doc) == test_doc.view().data());
            });

            bulk_operation_set_client->interpose([&](mongoc_bulk_operation_t* bulk, void* client) {
                bulk_operation_set_client_called = true;
                REQUIRE(client == mongo_client.implementation());
            });

            bulk_operation_set_database->interpose([&](mongoc_bulk_operation_t* bulk, const char* db) {
                bulk_operation_set_database_called = true;
                REQUIRE(!database_name.compare(db));
            });

            bulk_operation_set_collection->interpose([&](mongoc_bulk_operation_t* bulk, const char* coll) {
                bulk_operation_set_collection_called = true;
                REQUIRE(!collection_name.compare(coll));
            });

            bulk_operation_execute->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply,
                bson_error_t *error) {
                bulk_operation_execute_called = true;
                bson_init(reply);
                return 1;
            });

            bulk_operation_destroy->interpose([&](mongoc_bulk_operation_t*){
                bulk_operation_destroy_called = true;
            });

            // Run the operation
            mongo_coll.insert_one(test_doc);

            REQUIRE(bulk_operation_insert_called);
        }

        SECTION("Delete One", "[collection::delete_one]") {
            bool expected_order_setting = false;

            bulk_operation_new->interpose([&](bool ordered) -> mongoc_bulk_operation_t* {
                bulk_operation_new_called = true;
                REQUIRE(ordered == expected_order_setting);
                return nullptr;
            });

            bulk_operation_remove_one->interpose([&](mongoc_bulk_operation_t* bulk, const bson_t* doc) {
                bulk_operation_remove_one_called = true;
                REQUIRE(bson_get_data(doc) == test_doc.view().data());
            });

            bulk_operation_set_client->interpose([&](mongoc_bulk_operation_t* bulk, void* client) {
                bulk_operation_set_client_called = true;
                REQUIRE(client == mongo_client.implementation());
            });

            bulk_operation_set_database->interpose([&](mongoc_bulk_operation_t* bulk, const char* db) {
                bulk_operation_set_database_called = true;
                REQUIRE(!database_name.compare(db));
            });

            bulk_operation_set_collection->interpose([&](mongoc_bulk_operation_t* bulk, const char* coll) {
                bulk_operation_set_collection_called = true;
                REQUIRE(!collection_name.compare(coll));
            });

            bulk_operation_execute->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply,
                bson_error_t *error) {
                bulk_operation_execute_called = true;
                bson_init(reply);
                return 1;
            });

            bulk_operation_destroy->interpose([&](mongoc_bulk_operation_t*){
                bulk_operation_destroy_called = true;
            });

            // Run the operation
            mongo_coll.delete_one(test_doc);

            REQUIRE(bulk_operation_remove_one_called);
        }

        REQUIRE(bulk_operation_new_called);
        REQUIRE(bulk_operation_set_client_called);
        REQUIRE(bulk_operation_set_database_called);
        REQUIRE(bulk_operation_set_collection_called);
        REQUIRE(bulk_operation_execute_called);
        REQUIRE(bulk_operation_destroy_called);
    }

}
