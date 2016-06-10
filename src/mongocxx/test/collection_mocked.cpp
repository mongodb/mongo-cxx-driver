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

#include <chrono>
#include <string>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/private/helpers.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/stdx.hpp>

using namespace mongocxx;
using namespace bsoncxx;

TEST_CASE("A default constructed collection is false-ish", "[collection]") {
    instance::current();

    collection c;
    REQUIRE(!c);
}

TEST_CASE("Collection", "[collection]") {
    instance::current();

    // dummy_collection is the name the mocked collection_get_name returns
    const std::string collection_name("dummy_collection");
    const std::string database_name("test");

    MOCK_CLIENT
    MOCK_DATABASE
    MOCK_COLLECTION
    MOCK_FAM
    MOCK_BULK
    MOCK_CURSOR

    client mongo_client{uri{}};
    write_concern concern;
    database mongo_db = mongo_client[database_name];
    collection mongo_coll = mongo_db[collection_name];
    REQUIRE(mongo_coll);

    SECTION("Read Concern", "[collection]") {
        auto collection_set_rc_called = false;
        read_concern rc{};
        rc.acknowledge_level(read_concern::level::k_majority);

        collection_set_read_concern->interpose([&collection_set_rc_called](
            ::mongoc_collection_t*, const ::mongoc_read_concern_t* rc_t) {
            REQUIRE(rc_t);
            const auto result = libmongoc::read_concern_get_level(rc_t);
            REQUIRE(result);
            REQUIRE(strcmp(result, "majority") == 0);
            collection_set_rc_called = true;
        });

        mongo_coll.read_concern(rc);
        REQUIRE(collection_set_rc_called);
    }

    SECTION("Collection Rename", "[collection]") {
        std::string expected_rename;
        bool expected_drop;

        collection_rename->interpose([&expected_rename, &expected_drop](
            ::mongoc_collection_t*, const char*, const char* new_name,
            bool drop_target_before_rename, ::bson_error_t*) {
            REQUIRE(expected_rename == std::string{new_name});
            REQUIRE(expected_drop == drop_target_before_rename);
            return true;
        });

        SECTION("with drop_target_before_rename false") {
            expected_rename = "the_best_collection";
            expected_drop = false;
            mongo_coll.rename(expected_rename, expected_drop);
        }

        SECTION("with drop_target_before_rename true") {
            expected_rename = "brand_new_name";
            expected_drop = true;
            mongo_coll.rename(expected_rename, expected_drop);
        }
    }

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

        collection_aggregate->interpose([&](mongoc_collection_t*, mongoc_query_flags_t flags,
                                            const bson_t* pipeline, const bson_t* options,
                                            const mongoc_read_prefs_t*) -> mongoc_cursor_t* {
            collection_aggregate_called = true;
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

            if (opts.max_time())
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

            return NULL;
        });

        pipe.match(builder::stream::document{} << "foo"
                                               << "bar" << builder::stream::finalize);
        pipe.sort(builder::stream::document{} << "foo" << 1 << builder::stream::finalize);

        SECTION("With default options") {
        }

        SECTION("With some options") {
            opts.allow_disk_use(expected_allow_disk_use);
            opts.max_time(std::chrono::milliseconds{expected_max_time_ms});
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

        const bson_t* expected_opts = nullptr;

        collection_count_with_opts->interpose([&](mongoc_collection_t*, mongoc_query_flags_t flags,
                                                  const bson_t* query, int64_t skip, int64_t limit,
                                                  const bson_t* cmd_opts,
                                                  const mongoc_read_prefs_t*, bson_error_t* error) {
            collection_count_called = true;
            REQUIRE(flags == MONGOC_QUERY_NONE);
            REQUIRE(bson_get_data(query) == filter_doc.view().data());
            REQUIRE(skip == expected_skip);
            REQUIRE(limit == expected_limit);
            if (expected_opts) {
                REQUIRE(bson_equal(cmd_opts, expected_opts));
            }

            if (success) return 123;

            // The caller expects the bson_error_t to have been
            // initialized by the call to count in the event of an
            // error.
            bson_set_error(error, MONGOC_ERROR_COMMAND, MONGOC_ERROR_COMMAND_INVALID_ARG,
                           "expected error from mock");

            return -1;
        });

        SECTION("Succeeds with defaults") {
            REQUIRE_NOTHROW(mongo_coll.count(filter_doc.view()));
        }

        SECTION("Succeeds with options") {
            options::count opts;
            opts.skip(expected_skip);
            opts.limit(expected_limit);
            REQUIRE_NOTHROW(mongo_coll.count(filter_doc.view(), opts));
        }

        SECTION("Succeeds with hint") {
            options::count opts;
            hint index_hint("a_1");
            opts.hint(index_hint);

            // set our expected_opts so we check against that
            bsoncxx::document::value doc =
                bsoncxx::builder::stream::document{}
                << bsoncxx::builder::stream::concatenate(index_hint.to_document())
                << bsoncxx::builder::stream::finalize;
            libbson::scoped_bson_t cmd_opts{std::move(doc)};
            expected_opts = cmd_opts.bson();

            REQUIRE_NOTHROW(mongo_coll.count(filter_doc.view(), opts));
        }

        SECTION("Succeeds with read_prefs") {
            options::count opts;
            read_preference rp;
            rp.mode(read_preference::read_mode::k_secondary);
            opts.read_preference(rp);
            REQUIRE_NOTHROW(mongo_coll.count(filter_doc.view(), opts));
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.count(filter_doc.view()), operation_exception);
        }

        REQUIRE(collection_count_called);
    }

    SECTION("Create Index", "collection::create_index") {
        bool collection_create_index_called = false;
        bool success;
        bool expected_unique = true;
        auto expected_expire_after = std::chrono::seconds(500);
        std::string expected_config_string = "block_allocation=first";
        std::string expected_name = "index name";
        options::index options{};

        auto index_spec = builder::stream::document{} << "_id"
                                                      << "wow"
                                                      << "foo"
                                                      << "bar" << builder::stream::finalize;

        collection_create_index->interpose([&](mongoc_collection_t*, const bson_t*,
                                               const mongoc_index_opt_t* opt, bson_error_t* error) {
            collection_create_index_called = true;
            if (options.unique()) {
                REQUIRE(opt->unique == expected_unique);
            }
            if (options.expire_after()) {
                const auto count = static_cast<std::int32_t>(options.expire_after()->count());
                REQUIRE(opt->expire_after_seconds == count);
            }
            if (options.name()) {
                REQUIRE(opt->name == expected_name);
            }

            if (!success)
                bson_set_error(error, MONGOC_ERROR_COMMAND, MONGOC_ERROR_COMMAND_INVALID_ARG,
                               "expected error from mock");

            return success;
        });

        SECTION("Succeeds") {
            success = true;
            REQUIRE_NOTHROW(mongo_coll.create_index(index_spec.view()));
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.create_index(index_spec.view()), operation_exception);
        }

        SECTION("Succeeds With Options") {
            success = true;
            options.unique(expected_unique);
            options.expire_after(expected_expire_after);
            options.name(expected_name);
            REQUIRE_NOTHROW(mongo_coll.create_index(index_spec.view(), options));
        }

        SECTION("Fails with Options") {
            success = false;
            expected_expire_after =
                std::chrono::seconds(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1);
            options.expire_after(expected_expire_after);
            REQUIRE_THROWS_AS(mongo_coll.create_index(index_spec.view(), options), logic_error);

            expected_expire_after = std::chrono::seconds(-1);
            options.expire_after(expected_expire_after);
            REQUIRE_THROWS_AS(mongo_coll.create_index(index_spec.view(), options), logic_error);

            collection_create_index_called = true;  // mock for this section
        }

        SECTION("Succeeds With Storage Engine Options") {
            success = true;
            std::unique_ptr<options::index::wiredtiger_storage_options> wt_options =
                mongocxx::stdx::make_unique<options::index::wiredtiger_storage_options>();
            wt_options->config_string(expected_config_string);
            REQUIRE_NOTHROW(options.storage_options(std::move(wt_options)));
            REQUIRE_NOTHROW(mongo_coll.create_index(index_spec.view(), options));
        }

        REQUIRE(collection_create_index_called);
    }

    SECTION(
        "Drop"
        "[collection::drop]") {
        auto collection_drop_called = false;
        bool success;

        collection_drop->interpose([&](mongoc_collection_t*, bson_error_t* error) {
            collection_drop_called = true;

            if (!success)
                bson_set_error(error, MONGOC_ERROR_COMMAND, MONGOC_ERROR_COMMAND_INVALID_ARG,
                               "expected error from mock");

            return success;
        });

        SECTION("Succeeds") {
            success = true;
            REQUIRE_NOTHROW(mongo_coll.drop());
        }

        SECTION("Fails") {
            success = false;
            REQUIRE_THROWS_AS(mongo_coll.drop(), operation_exception);
        }

        REQUIRE(collection_drop_called);
    }

    SECTION("Find", "[collection::find]") {
        auto collection_find_called = false;
        auto find_doc = builder::stream::document{} << "a" << 1 << builder::stream::finalize;
        auto doc = find_doc.view();
        mongocxx::stdx::optional<bsoncxx::document::view> expected_sort{};
        mongocxx::stdx::optional<bsoncxx::document::view> expected_hint{};
        mongocxx::stdx::optional<bsoncxx::stdx::string_view> expected_comment{};
        mongocxx::stdx::optional<mongocxx::cursor::type> expected_cursor_type{};
        int expected_flags = 0;

        collection_find->interpose([&](mongoc_collection_t*, mongoc_query_flags_t flags,
                                       uint32_t skip, uint32_t limit, uint32_t batch_size,
                                       const bson_t* query, const bson_t* fields,
                                       const mongoc_read_prefs_t* read_prefs) {
            collection_find_called = true;

            REQUIRE(skip == skip);
            REQUIRE(limit == limit);
            REQUIRE(batch_size == batch_size);

            bsoncxx::document::view query_view{bson_get_data(query), query->len};

            REQUIRE(query_view["$query"].get_document() == doc);

            if (expected_sort) {
                REQUIRE(query_view["$orderby"].get_document() == *expected_sort);
            }
            if (expected_hint) {
                REQUIRE(query_view["$hint"].get_utf8() ==
                        expected_hint->operator[]("$hint").get_utf8());
            }
            if (expected_comment) {
                REQUIRE(query_view["$comment"].get_utf8().value == *expected_comment);
            }
            if (expected_cursor_type) {
                REQUIRE(flags == expected_flags);
            } else {
                REQUIRE(flags == ::MONGOC_QUERY_NONE);
            }
            REQUIRE(fields == NULL);
            REQUIRE(read_prefs == NULL);

            mongoc_cursor_t* cursor = NULL;
            return cursor;
        });

        SECTION("find succeeds") {
            REQUIRE_NOTHROW(mongo_coll.find(doc));
        }

        SECTION("Succeeds with hint") {
            options::find opts;
            hint index_hint("a_1");
            opts.hint(index_hint);

            // set our expected_hint so we check against that
            bsoncxx::document::value hint_doc = index_hint.to_document();
            expected_hint = hint_doc.view();

            REQUIRE_NOTHROW(mongo_coll.find(doc, opts));
        }

        SECTION("find with sort succeeds") {
            options::find opts{};
            auto sort_doc = builder::stream::document{} << "x" << -1 << builder::stream::finalize;
            expected_sort = sort_doc.view();
            opts.sort(*expected_sort);
            REQUIRE_NOTHROW(mongo_coll.find(doc, opts));
        }

        SECTION("Succeeds with comment") {
            expected_comment.emplace("my comment");
            options::find opts;
            opts.comment(*expected_comment);

            REQUIRE_NOTHROW(mongo_coll.find(doc, opts));
        }

        SECTION("Succeeds with cursor type") {
            options::find opts;
            expected_cursor_type = mongocxx::cursor::type::k_tailable;
            expected_flags = ::MONGOC_QUERY_TAILABLE_CURSOR;
            opts.cursor_type(*expected_cursor_type);

            REQUIRE_NOTHROW(mongo_coll.find(doc, opts));
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
            [&](mongoc_bulk_operation_t*, bool bypass) {
                bulk_operation_set_bypass_document_validation_called = true;
                REQUIRE(expected_bypass_document_validation == bypass);
            });

        bulk_operation_set_client->interpose(
            [&](mongoc_bulk_operation_t*, void*) { bulk_operation_set_client_called = true; });

        bulk_operation_set_database->interpose([&](mongoc_bulk_operation_t*, const char* db) {
            bulk_operation_set_database_called = true;
            REQUIRE(database_name == db);
        });

        bulk_operation_set_collection->interpose([&](mongoc_bulk_operation_t*, const char* coll) {
            bulk_operation_set_collection_called = true;
            REQUIRE(collection_name == coll);
        });

        bulk_operation_set_write_concern->interpose(
            [&](mongoc_bulk_operation_t*, const mongoc_write_concern_t*) {
                bulk_operation_set_write_concern_called = true;
                // TODO: actually test the write concern setting is correct or default
            });

        bulk_operation_execute->interpose(
            [&](mongoc_bulk_operation_t*, bson_t* reply, bson_error_t*) {
                bulk_operation_execute_called = true;
                bson_init(reply);
                return 1;
            });

        bulk_operation_destroy->interpose(
            [&](mongoc_bulk_operation_t*) { bulk_operation_destroy_called = true; });

        SECTION("Insert One", "[collection::insert_one]") {
            bulk_operation_insert->interpose([&](mongoc_bulk_operation_t*, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.insert_one(filter_doc.view());
        }

        SECTION("Insert One Bypassing Validation", "[collection::insert_one]") {
            bulk_operation_insert->interpose([&](mongoc_bulk_operation_t*, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            expect_set_bypass_document_validation_called = true;
            SECTION("...set to false") {
                expected_bypass_document_validation = false;
            }
            SECTION("...set to true") {
                expected_bypass_document_validation = true;
            }
            options::insert opts{};
            opts.bypass_document_validation(expected_bypass_document_validation);
            mongo_coll.insert_one(filter_doc.view(), opts);
        }

        SECTION("Update One", "[collection::update_one]") {
            bool upsert_option = false;

            bulk_operation_update_one->interpose([&](mongoc_bulk_operation_t*, const bson_t* query,
                                                     const bson_t* update, bool upsert) {
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

            mongo_coll.update_one(filter_doc.view(), modification_doc.view(), options);
        }

        SECTION("Update Many", "[collection::update_many]") {
            bool upsert_option;

            bulk_operation_update->interpose([&](mongoc_bulk_operation_t*, const bson_t* query,
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

            mongo_coll.update_many(filter_doc.view(), modification_doc.view(), options);
        }

        SECTION("Replace One", "[collection::replace_one]") {
            bool upsert_option;

            bulk_operation_replace_one->interpose([&](mongoc_bulk_operation_t*, const bson_t* query,
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

            mongo_coll.replace_one(filter_doc.view(), modification_doc.view(), options);
        }

        SECTION("Delete One", "[collection::delete_one]") {
            bulk_operation_remove_one->interpose([&](mongoc_bulk_operation_t*, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.delete_one(filter_doc.view());
        }

        SECTION("Delete Many", "[collection::delete_many]") {
            bulk_operation_remove->interpose([&](mongoc_bulk_operation_t*, const bson_t* doc) {
                bulk_operation_op_called = true;
                REQUIRE(bson_get_data(doc) == filter_doc.view().data());
            });

            mongo_coll.delete_many(filter_doc.view());
        }

        REQUIRE(bulk_operation_new_called);
        REQUIRE(expect_set_write_concern_called == bulk_operation_set_write_concern_called);
        REQUIRE(expect_set_bypass_document_validation_called ==
                bulk_operation_set_bypass_document_validation_called);
        REQUIRE(bulk_operation_op_called);
        REQUIRE(bulk_operation_set_client_called);
        REQUIRE(bulk_operation_set_database_called);
        REQUIRE(bulk_operation_set_collection_called);
        REQUIRE(bulk_operation_execute_called);
        REQUIRE(bulk_operation_destroy_called);
    }

    SECTION("Find and Modify") {
        using builder::stream::close_document;
        using builder::stream::finalize;
        using builder::stream::open_document;
        document::view expected_filter;
        auto fam_called = false;
        auto return_doc = builder::stream::document{} << "value" << open_document << "key"
                                                      << "val" << close_document << finalize;
        libbson::scoped_bson_t return_bson{return_doc.view()};
        bsoncxx::stdx::optional<bsoncxx::document::value> fam_result;

        collection_find_and_modify_with_opts->interpose(
            [&](::mongoc_collection_t*, const ::bson_t* filter,
                const ::mongoc_find_and_modify_opts_t*, ::bson_t* reply, ::bson_error_t*) {
                fam_called = true;
                document::view filter_view{bson_get_data(filter), filter->len};
                REQUIRE(expected_filter == filter_view);
                ::bson_copy_to(return_bson.bson(), reply);
                return true;
            });

        SECTION("Delete", "[collection::find_one_and_delete]") {
            options::find_one_and_delete opts{};
            builder::stream::document filter{};
            filter << "x" << 1;
            expected_filter = filter.view();
            expected_find_and_modify_opts_flags = ::MONGOC_FIND_AND_MODIFY_REMOVE;

            builder::stream::document sort{};
            sort << "name.last" << 1 << "name.first" << 1;
            expected_find_and_modify_opts_sort = sort.view();
            opts.sort(expected_find_and_modify_opts_sort);

            fam_result = mongo_coll.find_one_and_delete(expected_filter, opts);
        }

        SECTION("Replace", "[collection::find_one_and_replace]") {
            options::find_one_and_replace opts{};
            builder::stream::document filter{};
            filter << "y" << 2;
            expected_filter = filter.view();

            builder::stream::document replace{};
            replace << "newdoc" << true;
            expected_find_and_modify_opts_update = replace.view();

            builder::stream::document projection{};
            projection << "_id" << false << "oldval" << true;
            expected_find_and_modify_opts_fields = projection.view();
            opts.projection(expected_find_and_modify_opts_fields);

            expected_find_and_modify_opts_flags = ::MONGOC_FIND_AND_MODIFY_NONE;

            expected_find_and_modify_opts_bypass_document_validation = false;
            opts.bypass_document_validation(
                expected_find_and_modify_opts_bypass_document_validation);

            fam_result = mongo_coll.find_one_and_replace(
                expected_filter, expected_find_and_modify_opts_update, opts);
        }

        SECTION("Update", "[collection::find_one_and_update]") {
            options::find_one_and_update opts{};
            builder::stream::document filter{};
            filter << "z" << 3;
            expected_filter = filter.view();

            builder::stream::document update{};
            update << "newdoc" << true;
            expected_find_and_modify_opts_update = update.view();

            expected_find_and_modify_opts_flags = static_cast<::mongoc_find_and_modify_flags_t>(
                ::MONGOC_FIND_AND_MODIFY_UPSERT | ::MONGOC_FIND_AND_MODIFY_RETURN_NEW);

            expected_find_and_modify_opts_bypass_document_validation = true;
            opts.bypass_document_validation(
                expected_find_and_modify_opts_bypass_document_validation);

            opts.upsert(true);
            opts.return_document(options::return_document::k_after);

            fam_result = mongo_coll.find_one_and_update(expected_filter,
                                                        expected_find_and_modify_opts_update, opts);
        }

        REQUIRE(fam_called);
        REQUIRE(fam_result);
        REQUIRE(fam_result->view() == return_doc.view()["value"].get_document());
    }
}
