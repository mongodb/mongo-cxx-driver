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

#include <string>

#include <mongocxx/private/libmongoc.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/exception/operation.hpp>

using namespace mongocxx;
using namespace bsoncxx;

TEST_CASE("Collection", "[collection]") {
    const std::string collection_name("mongocxx");
    const std::string database_name("test");

    MOCK_CLIENT
    MOCK_DATABASE
    MOCK_COLLECTION
    MOCK_BULK

    client mongo_client;
    write_concern concern;
    database mongo_db = mongo_client[database_name];
    collection mongo_coll = mongo_db[collection_name];

    auto filter_doc = builder::stream::document{} 
        << "_id" << "wow" << "foo" << "bar"
    << builder::stream::finalize;

    SECTION("Count" "[collection::count]") {
        auto collection_count_called = false;
        bool success;

        collection_count->interpose([&](
            mongoc_collection_t* coll,
            mongoc_query_flags_t flags,
            const bson_t* query,
            int64_t skip,
            int64_t limit,
            const mongoc_read_prefs_t* read_prefs,
            bson_error_t* error
        ){
            collection_count_called = true;
            REQUIRE(coll == mongo_coll.implementation());
            REQUIRE(flags == MONGOC_QUERY_NONE);
            REQUIRE(bson_get_data(query) == filter_doc.view().data());
            REQUIRE(skip == 0);
            REQUIRE(limit == 0);
            return success ? 123 : -1;
        });

        SECTION("Succeeds") {
            success = true;
            mongo_coll.count(filter_doc);
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.count(filter_doc), exception::operation);
        }

        REQUIRE(collection_count_called);
    }

    SECTION("Create Index", "collection::create_index") {
        auto collection_create_index_called = false;

        auto index_spec = builder::stream::document{} 
            << "_id" << "wow" << "foo" << "bar"
        << builder::stream::finalize;

        bool success;

        collection_create_index->interpose([&](
            mongoc_collection_t* coll,
            const bson_t* keys,
            const mongoc_index_opt_t* opt,
            bson_error_t* error
        ){
            collection_create_index_called = true;
            REQUIRE(coll == mongo_coll.implementation());
            return success;
        });

        SECTION("Succeeds") {
            success = true;
            mongo_coll.create_index(index_spec);
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.create_index(index_spec), exception::operation);
        }

        REQUIRE(collection_create_index_called);
    }

    SECTION("Drop" "[collection::drop]") {
        auto collection_drop_called = false;
        bool success;

        collection_drop->interpose([&](
            mongoc_collection_t* coll,
            bson_error_t* error
        ) {
            collection_drop_called = true;
            REQUIRE(coll == mongo_coll.implementation());
            return success;
        });

        SECTION("Succeeds") {
            success = true;
            mongo_coll.drop();
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.drop(), exception::operation);
        }

        REQUIRE(collection_drop_called);
    }

    SECTION("Writes", "[collection::writes]") {
        auto expected_order_setting = false;
        auto expect_set_write_concern_called = false;

        auto modification_doc = builder::stream::document{}
            << "cool" << "wow" << "foo" << "bar"
        << builder::stream::finalize;

        bulk_operation_new->interpose([&](bool ordered) -> mongoc_bulk_operation_t* {
            bulk_operation_new_called = true;
            REQUIRE(ordered == expected_order_setting);
            return nullptr;
        });

        bulk_operation_set_client->interpose([&](
            mongoc_bulk_operation_t* bulk,
            void* client
        ) {
            bulk_operation_set_client_called = true;
            REQUIRE(client == mongo_client.implementation());
        });

        bulk_operation_set_database->interpose([&](
            mongoc_bulk_operation_t* bulk,
            const char* db
        ) {
            bulk_operation_set_database_called = true;
            REQUIRE(database_name == db);
        });

        bulk_operation_set_collection->interpose([&](
            mongoc_bulk_operation_t* bulk,
            const char* coll
        ) {
            bulk_operation_set_collection_called = true;
            REQUIRE(collection_name == coll);
        });

        bulk_operation_set_write_concern->interpose([&](
            mongoc_bulk_operation_t* bulk,
            const mongoc_write_concern_t* wc
        ) {
            bulk_operation_set_write_concern_called = true;
            // TODO: actually test the write concern setting is correct or default
            //REQUIRE(wc == concern.implementation());
        });

        bulk_operation_execute->interpose([&](
            mongoc_bulk_operation_t* bulk,
            bson_t* reply,
            bson_error_t *error
        ) {
            bulk_operation_execute_called = true;
            bson_init(reply);
            return 1;
        });

        bulk_operation_destroy->interpose([&](mongoc_bulk_operation_t*) {
            bulk_operation_destroy_called = true;
        });

        SECTION("Insert One", "[collection::insert_one]") {
            bulk_operation_insert->interpose([&](
                mongoc_bulk_operation_t* bulk,
                const bson_t* doc
            ) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.insert_one(filter_doc);
        }

        SECTION("Update One", "[collection::update_one]") {
            bool upsert_option;

            bulk_operation_update_one->interpose([&](
                mongoc_bulk_operation_t* bulk,
                const bson_t* query,
                const bson_t* update,
                bool upsert
            ) {
                bulk_operation_op_called = true;
                REQUIRE(upsert == upsert_option);
                REQUIRE(bson_get_data(query) == filter_doc.view().data());
                REQUIRE(bson_get_data(update) == modification_doc.view().data());
            });

            options::update options;

            SECTION("Default Options") {
                upsert_option = false;
            }

            SECTION("Upsert true") {
                upsert_option = true;
                options.upsert(upsert_option);
            }

            SECTION("Upsert false") {
                upsert_option = false;
                options.upsert(upsert_option);
            }

            SECTION("Write Concern provided") {
                upsert_option = false;
                options.write_concern(concern);
                expect_set_write_concern_called = true;
            }

            mongo_coll.update_one(filter_doc, modification_doc, options);
        }

        SECTION("Update Many", "[collection::update_many]") {
            bool upsert_option;

            bulk_operation_update->interpose([&](
                mongoc_bulk_operation_t* bulk,
                const bson_t* query,
                const bson_t* update,
                bool upsert
            ) {
                bulk_operation_op_called = true;
                REQUIRE(upsert == upsert_option);
                REQUIRE(bson_get_data(query) == filter_doc.view().data());
                REQUIRE(bson_get_data(update) == modification_doc.view().data());
            });

            options::update options;

            SECTION("Default Options") {
                upsert_option = false;
            }

            SECTION("Upsert true") {
                upsert_option = true;
                options.upsert(upsert_option);
            }

            SECTION("Upsert false") {
                upsert_option = false;
                options.upsert(upsert_option);
            }

            mongo_coll.update_many(filter_doc, modification_doc, options);
        }

        SECTION("Replace One", "[collection::replace_one]") {
            bool upsert_option;

            bulk_operation_replace_one->interpose([&](
                mongoc_bulk_operation_t* bulk,
                const bson_t* query,
                const bson_t* update,
                bool upsert
            ) {
                bulk_operation_op_called = true;
                REQUIRE(upsert == upsert_option);
                REQUIRE(bson_get_data(query) == filter_doc.view().data());
                REQUIRE(bson_get_data(update) == modification_doc.view().data());
            });

            options::update options;

            SECTION("Default Options") {
                upsert_option = false;
            }

            SECTION("Upsert true") {
                upsert_option = true;
                options.upsert(upsert_option);
            }

            SECTION("Upsert false") {
                upsert_option = false;
                options.upsert(upsert_option);
            }

            mongo_coll.replace_one(filter_doc, modification_doc, options);
        }

        SECTION("Delete One", "[collection::delete_one]") {
            bulk_operation_remove_one->interpose([&](
                mongoc_bulk_operation_t* bulk,
                const bson_t* doc
            ) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.delete_one(filter_doc);
        }

        SECTION("Delete Many", "[collection::delete_many]") {
            bulk_operation_remove->interpose([&](
                mongoc_bulk_operation_t* bulk,
                const bson_t* doc
            ) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.delete_many(filter_doc);
        }

        REQUIRE(bulk_operation_new_called);
        REQUIRE(expect_set_write_concern_called == bulk_operation_set_write_concern_called);
        REQUIRE(bulk_operation_op_called);
        REQUIRE(bulk_operation_set_client_called);
        REQUIRE(bulk_operation_set_database_called);
        REQUIRE(bulk_operation_set_collection_called);
        REQUIRE(bulk_operation_execute_called);
        REQUIRE(bulk_operation_destroy_called);
    }
}
