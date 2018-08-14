// Copyright 2018-present MongoDB Inc.
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

#include <fstream>
#include <iostream>
#include <memory>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using namespace bsoncxx;
using namespace mongocxx;

int64_t as_int64(const document::element& el) {
    if (el.type() == type::k_int32) {
        return static_cast<std::int64_t>(el.get_int32().value);
    } else if (el.type() == type::k_int64) {
        return el.get_int64().value;
    }
    REQUIRE(false);
    return 0;
}

document::value run_count_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::count options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["limit"]) {
        options.limit(arguments["limit"].get_int32().value);
    }

    if (arguments["skip"]) {
        options.skip(arguments["skip"].get_int32().value);
    }

    int64_t count = coll->count(filter, options);

    auto result = builder::basic::document{};

    // The JSON parser reads the counts as int32, so the document will not be equal to the expected
    // result without casting.
    result.append(builder::basic::kvp("result", static_cast<std::int32_t>(count)));

    return result.extract();
}

document::value run_find_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::find options{};

    if (arguments["batchSize"]) {
        options.batch_size(static_cast<std::int32_t>(as_int64(arguments["batchSize"])));
    }

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["limit"]) {
        options.limit(as_int64(arguments["limit"]));
    }

    if (arguments["skip"]) {
        options.skip(as_int64(arguments["skip"]));
    }

    if (arguments["sort"]) {
        options.sort(arguments["sort"].get_document().value);
    }

    if (arguments["modifiers"]) {
        document::view modifiers = arguments["modifiers"].get_document().value;
        if (modifiers["$comment"]) {
            options.comment(modifiers["$comment"].get_utf8().value);
        }

        if (modifiers["$hint"]) {
            hint my_hint(modifiers["$hint"].get_document().value);
            options.hint(my_hint);
        }

        if (modifiers["$max"]) {
            options.max(modifiers["$max"].get_document().value);
        }

        if (modifiers["$maxTimeMS"]) {
            std::chrono::milliseconds my_millis(modifiers["$maxTimeMS"].get_int32().value);
            options.max_time(my_millis);
        }

        if (modifiers["$min"]) {
            options.min(modifiers["$min"].get_document().value);
        }

        if (modifiers["$returnKey"]) {
            options.return_key(modifiers["$returnKey"].get_bool().value);
        }

        if (modifiers["$showDiskLoc"]) {
            options.show_record_id(modifiers["$showDiskLoc"].get_bool().value);
        }
    }

    cursor result_cursor = coll->find(filter, options);

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [&result_cursor](builder::basic::sub_array array) {
        for (auto&& document : result_cursor) {
            array.append(document);
        }
    }));

    return result.extract();
}

document::value run_delete_many_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::delete_options options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    auto result = builder::basic::document{};
    std::int32_t deleted_count = 0;

    if (auto delete_result = coll->delete_many(filter, options)) {
        deleted_count = delete_result->deleted_count();
    }

    result.append(
        builder::basic::kvp("result", [deleted_count](builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("deletedCount", deleted_count));

        }));

    return result.extract();
}

document::value run_delete_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::delete_options options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    auto result = builder::basic::document{};
    std::int32_t deleted_count = 0;

    if (auto delete_result = coll->delete_one(filter, options)) {
        deleted_count = delete_result->deleted_count();
    }

    result.append(
        builder::basic::kvp("result", [deleted_count](builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("deletedCount", deleted_count));

        }));

    return result.extract();
}

document::value run_insert_many_test(collection* coll, document::view operation) {
    using builder::basic::kvp;
    using builder::basic::make_document;
    document::view arguments = operation["arguments"].get_document().value;
    array::view documents = arguments["documents"].get_array().value;
    std::vector<document::view> documents_to_insert{};
    options::insert insert_options;

    if (arguments["options"]) {
        document::view options = arguments["options"].get_document().value;
        if (options["ordered"]) {
            insert_options.ordered(options["ordered"].get_bool().value);
        }
    }

    for (auto&& element : documents) {
        documents_to_insert.push_back(element.get_document().value);
    }

    auto insert_many_result = coll->insert_many(documents_to_insert, insert_options);
    std::map<size_t, document::element> inserted_ids{};

    if (insert_many_result) {
        inserted_ids = insert_many_result->inserted_ids();
    }

    auto result = builder::basic::document{};
    builder::basic::document inserted_ids_builder;

    for (auto&& id_pair : inserted_ids) {
        inserted_ids_builder.append(kvp(std::to_string(id_pair.first), id_pair.second.get_value()));
    }

    result.append(kvp("result", make_document(kvp("insertedIds", inserted_ids_builder.extract()))));
    return result.extract();
}

document::value run_insert_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view document = arguments["document"].get_document().value;
    auto result = builder::basic::document{};
    auto insert_one_result = coll->insert_one(document);
    types::value inserted_id{types::b_null{}};

    if (insert_one_result) {
        inserted_id = insert_one_result->inserted_id();
    }

    result.append(builder::basic::kvp("result", [inserted_id](builder::basic::sub_document subdoc) {
        subdoc.append(builder::basic::kvp("insertedId", inserted_id));

    }));

    return result.extract();
}

document::value run_update_many_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view update = arguments["update"].get_document().value;
    options::update options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    if (arguments["arrayFilters"]) {
        options.array_filters(arguments["arrayFilters"].get_array().value);
    }

    std::int32_t matched_count = 0;
    bsoncxx::stdx::optional<std::int32_t> modified_count;
    std::int32_t upserted_count = 0;
    auto update_result = coll->update_many(filter, update, options);
    bsoncxx::stdx::optional<types::value> upserted_id{};

    if (update_result) {
        matched_count = update_result->matched_count();

        // Server versions below 2.4 do not return an `nModified` count.
        try {
            modified_count = update_result->modified_count();
        } catch (const std::exception& e) {
        }

        upserted_count = update_result->result().upserted_count();

        if (auto upserted_element = update_result->upserted_id()) {
            upserted_id = upserted_element->get_value();
        }
    }

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp(
        "result",
        [matched_count, modified_count, upserted_count, upserted_id](
            builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("matchedCount", matched_count));

            if (modified_count) {
                subdoc.append(builder::basic::kvp("modifiedCount", *modified_count));
            }

            subdoc.append(builder::basic::kvp("upsertedCount", upserted_count));

            if (upserted_id) {
                subdoc.append(builder::basic::kvp("upsertedId", *upserted_id));
            }
        }));

    return result.extract();
}

document::value run_update_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view update = arguments["update"].get_document().value;
    options::update options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    if (arguments["arrayFilters"]) {
        options.array_filters(arguments["arrayFilters"].get_array().value);
    }

    std::int32_t matched_count = 0;
    bsoncxx::stdx::optional<std::int32_t> modified_count;
    std::int32_t upserted_count = 0;
    auto update_result = coll->update_one(filter, update, options);
    bsoncxx::stdx::optional<types::value> upserted_id{};

    if (update_result) {
        matched_count = update_result->matched_count();

        // Server versions below 2.4 do not return an `nModified` count.
        try {
            modified_count = update_result->modified_count();
        } catch (const std::exception& e) {
        }

        upserted_count = update_result->result().upserted_count();

        if (auto upserted_element = update_result->upserted_id()) {
            upserted_id = upserted_element->get_value();
        }
    }

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp(
        "result",
        [matched_count, modified_count, upserted_count, upserted_id](
            builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("matchedCount", matched_count));

            if (modified_count) {
                subdoc.append(builder::basic::kvp("modifiedCount", *modified_count));
            }

            subdoc.append(builder::basic::kvp("upsertedCount", upserted_count));

            if (upserted_id) {
                subdoc.append(builder::basic::kvp("upsertedId", *upserted_id));
            }
        }));

    return result.extract();
}

document::value run_bulk_write_test(collection* coll, document::view operation) {
    using builder::basic::kvp;
    using builder::basic::make_document;

    options::bulk_write options;
    std::vector<model::write> writes;
    if (operation["collectionOptions"]) {
        // Set write concern from collection options.
        document::view collection_options = operation["collectionOptions"].get_document().value;
        document::view write_concern_options =
            collection_options["writeConcern"].get_document().value;
        int32_t w_option = write_concern_options["w"].get_int32().value;
        write_concern w;
        w.nodes(w_option);
        options.write_concern(w);
    }
    auto arguments = operation["arguments"].get_document().value;
    if (arguments["options"]) {
        document::view options_doc = arguments["options"].get_document().value;
        if (options_doc["ordered"]) {
            options.ordered(options_doc["ordered"].get_bool().value);
        }
    }
    auto requests = arguments["requests"].get_array().value;
    for (auto&& request_element : requests) {
        auto request = request_element.get_document().value;
        auto request_arguments = request["arguments"].get_document().value;
        auto operation_name = request["name"].get_utf8().value;
        if (operation_name.compare("updateOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            document::view update = request_arguments["update"].get_document().value;
            model::update_one update_one(filter, update);

            if (request_arguments["collation"]) {
                update_one.collation(request_arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                update_one.upsert(request_arguments["upsert"].get_bool().value);
            }

            if (request_arguments["arrayFilters"]) {
                update_one.array_filters(request_arguments["arrayFilters"].get_array().value);
            }

            writes.emplace_back(update_one);
        } else if (operation_name.compare("updateMany") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            document::view update = request_arguments["update"].get_document().value;
            model::update_many update_many(filter, update);

            if (request_arguments["collation"]) {
                update_many.collation(request_arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                update_many.upsert(request_arguments["upsert"].get_bool().value);
            }

            if (request_arguments["arrayFilters"]) {
                update_many.array_filters(request_arguments["arrayFilters"].get_array().value);
            }

            writes.emplace_back(update_many);
        } else if (operation_name.compare("replaceOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            document::view replacement = request_arguments["replacement"].get_document().value;
            model::replace_one replace_one(filter, replacement);
            if (request_arguments["collation"]) {
                replace_one.collation(arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                replace_one.upsert(arguments["upsert"].get_bool().value);
            }

            writes.emplace_back(replace_one);
        } else if (operation_name.compare("insertOne") == 0) {
            document::view document = request_arguments["document"].get_document().value;
            model::insert_one insert_one(document);
            writes.emplace_back(insert_one);
        } else if (operation_name.compare("deleteOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            model::delete_one delete_one(filter);
            if (request_arguments["collation"]) {
                delete_one.collation(arguments["collation"].get_document().value);
            }

            writes.emplace_back(delete_one);
        } else if (operation_name.compare("deleteMany") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            model::delete_many delete_many(filter);
            if (request_arguments["collation"]) {
                delete_many.collation(arguments["collation"].get_document().value);
            }

            writes.emplace_back(delete_many);
        } else {
            /* should not happen. */
            REQUIRE(false);
        }
    }

    std::int32_t deleted_count = 0;
    std::int32_t matched_count = 0;
    std::int32_t modified_count = 0;
    std::int32_t upserted_count = 0;
    result::bulk_write::id_map upserted_ids;
    std::int32_t inserted_count = 0;
    auto bulk_write_result = coll->bulk_write(writes, options);
    if (bulk_write_result) {
        matched_count = bulk_write_result->matched_count();
        modified_count = bulk_write_result->modified_count();
        upserted_count = bulk_write_result->upserted_count();
        upserted_ids = bulk_write_result->upserted_ids();
        inserted_count = bulk_write_result->inserted_count();
        deleted_count = bulk_write_result->deleted_count();
    }
    builder::basic::document upserted_ids_builder;
    for (auto&& index_and_id : upserted_ids) {
        upserted_ids_builder.append(
            kvp(std::to_string(index_and_id.first), index_and_id.second.get_document().value));
    }
    auto upserted_ids_doc = upserted_ids_builder.extract();
    auto result = builder::basic::document{};
    // Construct the result document.
    // Note: insertedIds is currently hard coded as an empty document, because result::bulk_write
    // provides no way to see inserted ids. This is compliant with the CRUD spec, as insertedIds
    // are: "NOT REQUIRED: Drivers may choose to not provide this property." So just add an empty
    // document for insertedIds. There are no current bulk write tests testing insert operations.
    // The insertedIds field in current bulk write spec tests is always an empty document.
    result.append(kvp("result",
                      make_document(kvp("matchedCount", matched_count),
                                    kvp("modifiedCount", modified_count),
                                    kvp("upsertedCount", upserted_count),
                                    kvp("deletedCount", deleted_count),
                                    kvp("insertedCount", inserted_count),
                                    kvp("insertedIds", make_document()),
                                    kvp("upsertedIds", upserted_ids_doc))));
    return result.extract();
}

std::map<std::string, std::function<document::value(collection*, document::view)>> test_runners = {
    {"count", run_count_test},
    {"find", run_find_test},
    {"deleteMany", run_delete_many_test},
    {"deleteOne", run_delete_one_test},
    {"insertMany", run_insert_many_test},
    {"insertOne", run_insert_one_test},
    {"updateMany", run_update_many_test},
    {"updateOne", run_update_one_test},
    {"bulkWrite", run_bulk_write_test}};

bool is_subset_helper(types::value main, types::value sub) {
    if (sub.type() == type::k_int64 && sub.get_int32().value == 42) {
        return true;
    }

    if (main.type() != sub.type()) {
        // Compare 64 and 32 ints if possible.
        if (main.type() == type::k_int64 && sub.type() == type::k_int32) {
            return sub.get_int32().value == main.get_int64().value;
        }

        if (main.type() == type::k_int32 && sub.type() == type::k_int64) {
            return sub.get_int64().value == main.get_int32().value;
        }

        if (main.type() == type::k_double && sub.type() == type::k_int32) {
            return sub.get_int32().value == main.get_double().value;
        }

        if (main.type() == type::k_double && sub.type() == type::k_int64) {
            return sub.get_int64().value == main.get_double().value;
        }

        return false;
    }

    if (sub.type() == type::k_document) {
        document::view main_doc = main.get_document().value;
        for (document::element el : sub.get_document().value) {
            bsoncxx::stdx::string_view key{el.key()};
            if (!is_subset_helper(main_doc[key].get_value(), el.get_value())) {
                return false;
            }
        }
        return true;

    } else if (sub.type() == type::k_array) {
        array::view main_array = main.get_array().value;
        array::view sub_array = sub.get_array().value;

        if (main_array.length() < sub_array.length()) {
            return false;
        }

        auto main_iter = main_array.begin();
        for (array::element el : sub.get_array().value) {
            if (!is_subset_helper((*main_iter).get_value(), el.get_value())) {
                return false;
            }
            main_iter++;
        }
        return true;

    } else {
        return main == sub;
    }
}

bool is_subset(document::view doc, document::view sub) {
    for (document::element el : sub) {
        bsoncxx::stdx::string_view key{el.key()};

        // Just check if the write error is there. No need to compare message or error number.
        if (key.compare("writeErrors") == 0) {
            if (doc["writeErrors"]) {
                continue;
            } else {
                return false;
            }
        }

        if (!is_subset_helper(doc[key].get_value(), el.get_value())) {
            return false;
        }
    }

    return true;
}

void initialize_collection(document::view test_spec_view) {
    std::string db_name = string::to_string(test_spec_view["database_name"].get_utf8().value);
    std::string col_name = string::to_string(test_spec_view["collection_name"].get_utf8().value);

    client temp_client{uri{}};
    collection temp_col = temp_client[db_name][col_name];

    temp_col.drop();

    if (test_spec_view["data"]) {
        // insert this data
        array::view arr = test_spec_view["data"].get_array().value;

        for (array::element el : arr) {
            temp_col.insert_one(el.get_document().value);
        }
    }
}

void run_command_monitoring_tests_in_file(std::string test_path) {
    INFO("Test path: " << test_path);
    bsoncxx::stdx::optional<document::value> test_spec = test_util::parse_test_file(test_path);

    REQUIRE(test_spec);

    document::view test_spec_view = test_spec->view();

    std::string db_name = string::to_string(test_spec_view["database_name"].get_utf8().value);
    std::string col_name = string::to_string(test_spec_view["collection_name"].get_utf8().value);

    array::view tests = test_spec_view["tests"].get_array().value;

    for (auto&& test : tests) {
        initialize_collection(test_spec_view);
        std::string description = string::to_string(test["description"].get_utf8().value);
        INFO("Test description: " << description);
        array::view expectations = test["expectations"].get_array().value;

        if (test["ignore_if_server_version_greater_than"]) {
            client temp_client{uri{}};
            std::string server_version = test_util::get_server_version(temp_client);
            std::string max_server_version = bsoncxx::string::to_string(
                test["ignore_if_server_version_greater_than"].get_utf8().value);
            if (test_util::compare_versions(server_version, max_server_version) > 0) {
                return;
            }
        }

        // Used by the listeners
        auto events = expectations.begin();

        options::client client_opts;
        options::apm apm_opts;

        ///////////////////////////////////////////////////////////////////////
        // Begin command listener lambdas
        ///////////////////////////////////////////////////////////////////////

        // COMMAND STARTED
        apm_opts.on_command_started([&](const events::command_started_event& event) {
            if (event.command_name().compare("endSessions") == 0) {
                return;
            }

            auto expected = (*events).get_document().value;
            events++;

            for (auto ele : expected["command_started_event"].get_document().value) {
                bsoncxx::stdx::string_view field{ele.key()};
                types::value value{ele.get_value()};

                if (field.compare("command_name") == 0) {
                    REQUIRE(event.command_name() == value.get_utf8().value);
                } else if (field.compare("command") == 0) {
                    document::view expected_command = value.get_document().value;
                    document::view command = event.command();
                    REQUIRE(is_subset(command, expected_command));
                } else if (field.compare("database_name") == 0) {
                    REQUIRE(event.database_name() == value.get_utf8().value);
                } else {
                    // Should not happen.
                    REQUIRE(false);
                }
            }
        });

        // COMMAND FAILED
        apm_opts.on_command_failed([&](const events::command_failed_event& event) {
            auto expected = (*events).get_document().value;
            events++;

            for (auto ele : expected["command_failed_event"].get_document().value) {
                bsoncxx::stdx::string_view field{ele.key()};
                types::value value{ele.get_value()};

                if (field.compare("command_name") == 0) {
                    REQUIRE(event.command_name() == value.get_utf8().value);
                } else {
                    // Should not happen.
                    REQUIRE(false);
                }
            }
        });

        // COMMAND SUCCESS
        apm_opts.on_command_succeeded([&](const events::command_succeeded_event& event) {
            if (event.command_name().compare("endSessions") == 0) {
                return;
            }

            auto expected = (*events).get_document().value;
            events++;

            for (auto ele : expected["command_succeeded_event"].get_document().value) {
                bsoncxx::stdx::string_view field{ele.key()};
                types::value value{ele.get_value()};

                if (field.compare("command_name") == 0) {
                    REQUIRE(event.command_name() == value.get_utf8().value);
                } else if (field.compare("reply") == 0) {
                    document::view expected_reply = value.get_document().value;
                    document::view reply = event.reply();
                    REQUIRE(is_subset(reply, expected_reply));
                } else {
                    // Should not happen.
                    REQUIRE(false);
                }
            }
        });

        ///////////////////////////////////////////////////////////////////////
        // End command listener lambdas
        ///////////////////////////////////////////////////////////////////////

        // Apply listeners, and run operations.
        client_opts.apm_opts(apm_opts);
        client client{uri{}, client_opts};

        collection col = client[db_name][col_name];

        document::view operation = test["operation"].get_document().value;
        std::string operation_name = string::to_string(operation["name"].get_utf8().value);
        auto test_function = test_runners[operation_name];

        try {
            test_function(&col, operation);
        } catch (mongocxx::exception& e) {
            // do nothing.
        }

        REQUIRE(events == expectations.end());
    }
}

TEST_CASE("Command Monitoring Spec Tests", "[command_monitoring_spec]") {
    instance::current();
    char* command_monitoring_tests_path = std::getenv("COMMAND_MONITORING_TESTS_PATH");
    REQUIRE(command_monitoring_tests_path);

    std::string path{command_monitoring_tests_path};

    if (path.back() == '/') {
        path.pop_back();
    }

    std::ifstream test_files{path + "/test_files.txt"};

    REQUIRE(test_files.good());

    std::string test_file;

    while (std::getline(test_files, test_file)) {
        run_command_monitoring_tests_in_file(path + "/" + test_file);
    }
}
}  // namespace