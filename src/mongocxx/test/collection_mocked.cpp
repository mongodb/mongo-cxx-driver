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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/operation.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/stdx.hpp>

using namespace mongocxx;
using namespace bsoncxx;

TEST_CASE("A default constructed collection is false-ish", "[collection]") {
    collection c;
    REQUIRE(!c);
}

TEST_CASE("Collection", "[collection]") {
    // dummy_collection is the name the mocked collection_get_name returns
    const std::string collection_name("dummy_collection");
    const std::string database_name("test");

    MOCK_CLIENT
    MOCK_DATABASE
    MOCK_COLLECTION
    MOCK_BULK
    MOCK_CURSOR

    client mongo_client{uri{}};
    write_concern concern;
    database mongo_db = mongo_client[database_name];
    collection mongo_coll = mongo_db[collection_name];
    REQUIRE(mongo_coll);

    auto filter_doc = builder::stream::document{} << "_id"
                                                  << "wow"
                                                  << "foo"
                                                  << "bar" << builder::stream::finalize;

    SECTION("Aggregate", "[Collection::aggregate]") {
        auto collection_aggregate_called = false;
        auto expected_allow_disk_use = true;
        auto expected_max_time_ms = 1234;
        auto expected_batch_size = 5678;
        auto expected_use_cursor = true;
        auto expected_bypass_document_validation = true;

        pipeline pipe;
        options::aggregate opts;

        collection_aggregate->interpose(
            [&](mongoc_collection_t* collection, mongoc_query_flags_t flags, const bson_t* pipeline,
                const bson_t* options, const mongoc_read_prefs_t* read_prefs) -> mongoc_cursor_t* {
                collection_aggregate_called = true;
                REQUIRE(collection == mongo_coll.implementation());
                REQUIRE(flags == MONGOC_QUERY_NONE);

                bsoncxx::array::view p(bson_get_data(pipeline), pipeline->len);
                bsoncxx::document::view o(bson_get_data(options), options->len);

                mongocxx::stdx::string_view bar(
                    p[0].get_document().value["$match"].get_document().value["foo"].get_utf8());
                std::int32_t one(
                    p[1].get_document().value["$sort"].get_document().value["foo"].get_int32());

                REQUIRE(bar == mongocxx::stdx::string_view("bar"));
                REQUIRE(one == 1);

                if (opts.allow_disk_use())
                    REQUIRE(o["allowDiskUse"].get_bool().value == expected_allow_disk_use);
                else
                    REQUIRE(o.find("allowDiskUse") == o.end());

                if (opts.max_time_ms())
                    REQUIRE(o["maxTimeMS"].get_int64().value == expected_max_time_ms);
                else
                    REQUIRE(o.find("maxTimeMS") == o.end());

                if (opts.use_cursor()) REQUIRE(o.find("cursor") != o.end());

                if (opts.batch_size()) {
                    REQUIRE(o.find("cursor") != o.end());
                    REQUIRE(o["cursor"].get_document().value["batchSize"].get_int32() ==
                            expected_batch_size);
                }

                if (opts.bypass_document_validation())
                    REQUIRE(o["bypassDocumentValidation"].get_bool().value ==
                            expected_bypass_document_validation);
                else
                    REQUIRE(!o["bypassDocumentValidation"]);

                REQUIRE(read_prefs == mongo_coll.read_preference().implementation());

                return NULL;
            });

        pipe.match(builder::stream::document{} << "foo"
                                               << "bar" << builder::stream::finalize);
        pipe.sort(builder::stream::document{} << "foo" << 1 << builder::stream::finalize);

        SECTION("With default options") {
        }

        SECTION("With some options") {
            opts.allow_disk_use(expected_allow_disk_use);
            opts.max_time_ms(expected_max_time_ms);
            opts.batch_size(expected_batch_size);
            opts.bypass_document_validation(expected_bypass_document_validation);
            opts.use_cursor(expected_use_cursor);
        }

        mongo_coll.aggregate(pipe, opts);

        REQUIRE(collection_aggregate_called);
    }

    SECTION("Count", "[collection::count]") {
        auto collection_count_called = false;
        bool success = true;
        std::int64_t expected_skip = 0;
        std::int64_t expected_limit = 0;
        auto expected_read_pref = mongo_coll.read_preference().implementation();

        const bson_t* expected_opts = nullptr;

        collection_count_with_opts->interpose(
            [&](mongoc_collection_t* coll, mongoc_query_flags_t flags, const bson_t* query,
                int64_t skip, int64_t limit, const bson_t* cmd_opts,
                const mongoc_read_prefs_t* read_prefs, bson_error_t* error) {
                collection_count_called = true;
                REQUIRE(coll == mongo_coll.implementation());
                REQUIRE(flags == MONGOC_QUERY_NONE);
                REQUIRE(bson_get_data(query) == filter_doc.view().data());
                REQUIRE(skip == expected_skip);
                REQUIRE(limit == expected_limit);
                if (expected_read_pref) {
                    REQUIRE(MONGOC_READ_SECONDARY == mongoc_read_prefs_get_mode(read_prefs));
                }
                if (expected_opts) {
                    REQUIRE(bson_equal(cmd_opts, expected_opts));
                }
                return success ? 123 : -1;
            });

        SECTION("Succeeds with defaults") {
            REQUIRE_NOTHROW(mongo_coll.count(filter_doc));
        }

        SECTION("Succeeds with options") {
            options::count opts;
            opts.skip(expected_skip);
            opts.limit(expected_limit);
            REQUIRE_NOTHROW(mongo_coll.count(filter_doc, opts));
        }

        SECTION("Succeeds with hint") {
            options::count opts;
            hint index_hint("a_1");
            opts.hint(index_hint);

            // set our expected_opts so we check against that
            bsoncxx::document::value doc =
                bsoncxx::builder::stream::document{}
                << bsoncxx::builder::stream::concatenate{index_hint.to_document()}
                << bsoncxx::builder::stream::finalize;
            libbson::scoped_bson_t cmd_opts{doc};
            expected_opts = cmd_opts.bson();

            REQUIRE_NOTHROW(mongo_coll.count(filter_doc, opts));
        }

        SECTION("Succeeds with read_prefs") {
            options::count opts;
            read_preference rp;
            rp.mode(read_preference::read_mode::k_secondary);
            opts.read_preference(rp);
            REQUIRE_NOTHROW(mongo_coll.count(filter_doc, opts));
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.count(filter_doc), exception::operation);
        }

        REQUIRE(collection_count_called);
    }

    SECTION("Create Index", "collection::create_index") {
        auto collection_create_index_called = false;

        auto index_spec = builder::stream::document{} << "_id"
                                                      << "wow"
                                                      << "foo"
                                                      << "bar" << builder::stream::finalize;

        bool success;

        collection_create_index->interpose([&](mongoc_collection_t* coll, const bson_t* keys,
                                               const mongoc_index_opt_t* opt, bson_error_t* error) {
            collection_create_index_called = true;
            REQUIRE(coll == mongo_coll.implementation());
            return success;
        });

        SECTION("Succeeds") {
            success = true;
            REQUIRE_NOTHROW(mongo_coll.create_index(index_spec));
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.create_index(index_spec), exception::operation);
        }

        REQUIRE(collection_create_index_called);
    }

    SECTION(
        "Drop"
        "[collection::drop]") {
        auto collection_drop_called = false;
        bool success;

        collection_drop->interpose([&](mongoc_collection_t* coll, bson_error_t* error) {
            collection_drop_called = true;
            REQUIRE(coll == mongo_coll.implementation());
            return success;
        });

        SECTION("Succeeds") {
            success = true;
            REQUIRE_NOTHROW(mongo_coll.drop());
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.drop(), exception::operation);
        }

        REQUIRE(collection_drop_called);
    }

    SECTION("Find", "[collection::find]") {
        auto collection_find_called = false;
        auto find_doc = builder::stream::document{} << "a" << 1 << builder::stream::finalize;
        auto doc = find_doc.view();
        mongocxx::stdx::optional<bsoncxx::document::view> expected_sort{};

        collection_find->interpose([&](mongoc_collection_t* coll, mongoc_query_flags_t flags,
                                       uint32_t skip, uint32_t limit, uint32_t batch_size,
                                       const bson_t* query, const bson_t* fields,
                                       const mongoc_read_prefs_t* read_prefs) {
            collection_find_called = true;
            REQUIRE(coll == mongo_coll.implementation());
            REQUIRE(flags == MONGOC_QUERY_NONE);
            REQUIRE(skip == skip);
            REQUIRE(limit == limit);
            REQUIRE(batch_size == batch_size);
            bsoncxx::document::view query_view{bson_get_data(query), query->len};
            REQUIRE(query_view["$query"].get_document() == doc);
            if (expected_sort) {
                REQUIRE(query_view["$orderby"].get_document() == *expected_sort);
            }
            REQUIRE(fields == NULL);
            REQUIRE(read_prefs == NULL);

            mongoc_cursor_t* cursor = NULL;
            return cursor;
        });

        SECTION("find succeeds") {
            REQUIRE_NOTHROW(mongo_coll.find(doc));
        }

        SECTION("find with sort succeeds") {
            options::find opts{};
            auto sort_doc = builder::stream::document{} << "x" << -1 << builder::stream::finalize;
            expected_sort = sort_doc.view();
            opts.sort(*expected_sort);
            REQUIRE_NOTHROW(mongo_coll.find(doc,opts));
        }

        REQUIRE(collection_find_called);
    }

    SECTION("Writes", "[collection::writes]") {
        auto expected_order_setting = false;
        auto expect_set_write_concern_called = false;
        auto expect_set_bypass_document_validation_called = false;
        auto expected_bypass_document_validation = false;

        auto modification_doc = builder::stream::document{} << "cool"
                                                            << "wow"
                                                            << "foo"
                                                            << "bar" << builder::stream::finalize;

        bulk_operation_new->interpose([&](bool ordered) -> mongoc_bulk_operation_t* {
            bulk_operation_new_called = true;
            REQUIRE(ordered == expected_order_setting);
            return nullptr;
        });

        bulk_operation_set_bypass_document_validation->interpose(
            [&](mongoc_bulk_operation_t* bulk, bool bypass) {
                bulk_operation_set_bypass_document_validation_called = true;
                REQUIRE(expected_bypass_document_validation == bypass);
            });

        bulk_operation_set_client->interpose([&](mongoc_bulk_operation_t* bulk, void* client) {
            bulk_operation_set_client_called = true;
            REQUIRE(client == mongo_client.implementation());
        });

        bulk_operation_set_database->interpose([&](mongoc_bulk_operation_t* bulk, const char* db) {
            bulk_operation_set_database_called = true;
            REQUIRE(database_name == db);
        });

        bulk_operation_set_collection->interpose(
            [&](mongoc_bulk_operation_t* bulk, const char* coll) {
                bulk_operation_set_collection_called = true;
                REQUIRE(collection_name == coll);
            });

        bulk_operation_set_write_concern->interpose(
            [&](mongoc_bulk_operation_t* bulk, const mongoc_write_concern_t* wc) {
                bulk_operation_set_write_concern_called = true;
                // TODO: actually test the write concern setting is correct or default
                // REQUIRE(wc == concern.implementation());
            });

        bulk_operation_execute->interpose(
            [&](mongoc_bulk_operation_t* bulk, bson_t* reply, bson_error_t* error) {
                bulk_operation_execute_called = true;
                bson_init(reply);
                return 1;
            });

        bulk_operation_destroy->interpose(
            [&](mongoc_bulk_operation_t*) { bulk_operation_destroy_called = true; });

        SECTION("Insert One", "[collection::insert_one]") {
            bulk_operation_insert->interpose([&](mongoc_bulk_operation_t* bulk, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.insert_one(filter_doc);
        }

        SECTION("Insert One Bypassing Validation", "[collection::insert_one]") {
            bulk_operation_insert->interpose([&](mongoc_bulk_operation_t* bulk, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            expect_set_bypass_document_validation_called = true;
            SECTION("...set to false"){
                expected_bypass_document_validation = false;
            }
            SECTION("...set to true"){
                expected_bypass_document_validation = true;
            }
            options::insert opts{};
            opts.bypass_document_validation(expected_bypass_document_validation);
            mongo_coll.insert_one(filter_doc, opts);
        }

        SECTION("Update One", "[collection::update_one]") {
            bool upsert_option = false;

            bulk_operation_update_one->interpose([&](mongoc_bulk_operation_t* bulk,
                                                     const bson_t* query, const bson_t* update,
                                                     bool upsert) {
                bulk_operation_op_called = true;
                REQUIRE(upsert == upsert_option);
                REQUIRE(bson_get_data(query) == filter_doc.view().data());
                REQUIRE(bson_get_data(update) == modification_doc.view().data());
            });

            options::update options;

            SECTION("Default Options") {
            }

            SECTION("Upsert true") {
                upsert_option = true;
                options.upsert(upsert_option);
            }

            SECTION("Upsert false") {
                upsert_option = false;
                options.upsert(upsert_option);
            }

            SECTION("With bypass_document_validation") {
                expect_set_bypass_document_validation_called = true;
                expected_bypass_document_validation = true;
                options.bypass_document_validation(expected_bypass_document_validation);
            }

            SECTION("Write Concern provided") {
                options.write_concern(concern);
                expect_set_write_concern_called = true;
            }

            mongo_coll.update_one(filter_doc, modification_doc, options);
        }

        SECTION("Update Many", "[collection::update_many]") {
            bool upsert_option;

            bulk_operation_update->interpose([&](mongoc_bulk_operation_t* bulk, const bson_t* query,
                                                 const bson_t* update, bool upsert) {
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

            bulk_operation_replace_one->interpose([&](mongoc_bulk_operation_t* bulk,
                                                      const bson_t* query, const bson_t* update,
                                                      bool upsert) {
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
            bulk_operation_remove_one->interpose(
                [&](mongoc_bulk_operation_t* bulk, const bson_t* doc) {
                    bulk_operation_op_called = true;
                    REQUIRE(bson_get_data(doc) == filter_doc.view().data());
                });

            mongo_coll.delete_one(filter_doc);
        }

        SECTION("Delete Many", "[collection::delete_many]") {
            bulk_operation_remove->interpose([&](mongoc_bulk_operation_t* bulk, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.delete_many(filter_doc);
        }

        REQUIRE(bulk_operation_new_called);
        REQUIRE(expect_set_write_concern_called == bulk_operation_set_write_concern_called);
        REQUIRE(expect_set_bypass_document_validation_called == bulk_operation_set_bypass_document_validation_called);
        REQUIRE(bulk_operation_op_called);
        REQUIRE(bulk_operation_set_client_called);
        REQUIRE(bulk_operation_set_database_called);
        REQUIRE(bulk_operation_set_collection_called);
        REQUIRE(bulk_operation_execute_called);
        REQUIRE(bulk_operation_destroy_called);
    }
}
