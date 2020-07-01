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
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <mongocxx/test/spec/util.hh>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/find_one_common_options.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/operation.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace bsoncxx;
using namespace mongocxx;
using namespace spec;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::stdx::optional;
using bsoncxx::stdx::string_view;

static const int kMaxIsMasterFailCommands = 7;

uint32_t error_code_from_name(string_view name) {
    if (name.compare("CannotSatisfyWriteConcern") == 0) {
        return 100;
    } else if (name.compare("DuplicateKey") == 0) {
        return 11000;
    } else if (name.compare("NoSuchTransaction") == 0) {
        return 251;
    } else if (name.compare("WriteConflict") == 0) {
        return 112;
    } else if (name.compare("Interrupted") == 0) {
        return 11601;
    } else if (name.compare("MaxTimeMSExpired") == 0) {
        return 50;
    } else if (name.compare("UnknownReplWriteConcern") == 0) {
        return 79;
    } else if (name.compare("UnsatisfiableWriteConcern") == 0) {
        return 100;
    } else if (name.compare("OperationNotSupportedInTransaction") == 0) {
        return 263;
    }

    return 0;
}

/* Called with the entire test file and individual tests. */
bool should_skip_spec_test(const client& client, document::view test) {
    std::set<std::string> unsupported_tests = {
        "CreateIndex and dropIndex omits default write concern",
        "MapReduce omits default write concern"};

    if (test["description"]) {
        std::string description = std::string(test["description"].get_utf8().value);
        if (unsupported_tests.find(description) != unsupported_tests.end()) {
            UNSCOPED_INFO("Test skipped - " << description << "\n"
                                            << "reason: unsupported in C++ driver");
            return true;
        }
    }

    if (test["skipReason"]) {
        UNSCOPED_INFO("Test skipped - " << test["description"].get_utf8().value << "\n"
                                        << "reason: "
                                        << test["skipReason"].get_utf8().value);
        return true;
    }

    std::string server_version = test_util::get_server_version(client);
    std::string topology = test_util::get_topology(client);

    if (test["ignore_if_server_version_greater_than"]) {
        std::string max_server_version = bsoncxx::string::to_string(
            test["ignore_if_server_version_greater_than"].get_utf8().value);
        if (test_util::compare_versions(server_version, max_server_version) > 0) {
            return true;
        }
    }

    auto should_skip = [&](document::view requirements) {
        if (requirements["topology"]) {
            auto topologies = requirements["topology"].get_array().value;
            bool found = false;
            for (auto&& el : topologies) {
                if (std::string(el.get_utf8().value) == topology) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return true;
            }
        }

        if (requirements["minServerVersion"]) {
            auto min_server_version =
                string::to_string(requirements["minServerVersion"].get_utf8().value);
            if (test_util::compare_versions(server_version, min_server_version) < 0) {
                return true;
            }
        }

        if (requirements["maxServerVersion"]) {
            auto max_server_version =
                string::to_string(requirements["maxServerVersion"].get_utf8().value);
            if (test_util::compare_versions(server_version, max_server_version) > 0) {
                return true;
            }
        }

        return false;
    };

    if (test["runOn"]) {
        for (auto&& el : test["runOn"].get_array().value) {
            if (!should_skip(el.get_document())) {
                return false;
            }
        }
    } else if (!should_skip(test)) {
        return false;
    }

    UNSCOPED_INFO("Skipping - unsupported server version '" + server_version + "' with topology '" +
                  topology + "'");
    return true;
}

void configure_fail_point(const client& client, document::view test) {
    if (test["failPoint"]) {
        client["admin"].run_command(test["failPoint"].get_document().value);
    }
}

void disable_fail_point(const client& client, stdx::string_view fail_point) {
    /* Some transactions tests have a failCommand for "isMaster" repeat seven times. */
    for (int i = 0; i < kMaxIsMasterFailCommands; i++) {
        try {
            client["admin"].run_command(
                make_document(kvp("configureFailPoint", fail_point), kvp("mode", "off")));
            break;
        } catch (const std::exception&) {
            /* Tests that fail with isMaster also fail to disable the failpoint
             * (since we run isMaster when opening the connection). Ignore those
             * errors. */
            continue;
        }
    }
}

void disable_fail_point(std::string uri_string,
                        options::client client_opts,
                        stdx::string_view fail_point) {
    mongocxx::client client = {uri{uri_string}, client_opts};
    disable_fail_point(client, fail_point);
}

void set_up_collection(const client& client,
                       document::view test,
                       string_view database_name,
                       string_view collection_name) {
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);

    auto db = client[test[database_name].get_utf8().value];
    db.drop();

    auto coll_name = test[collection_name].get_utf8().value;
    auto coll = db[coll_name];

    coll.drop(wc_majority);
    coll = db.create_collection(coll_name, {}, wc_majority);

    // Set up JSON schema, if we have one
    if (test["json_schema"]) {
        validation_criteria validation{};
        validation.rule(test["json_schema"].get_document().value);

        auto cmd = bsoncxx::builder::basic::document{};
        cmd.append(kvp("collMod", coll_name));
        cmd.append(kvp("validator", [&](bsoncxx::builder::basic::sub_document subdoc) {
            subdoc.append(kvp("$jsonSchema", test["json_schema"].get_document().value));
        }));

        db.run_command(cmd.extract());
    }

    // Seed collection with data, if we have it
    if (test["data"]) {
        options::insert insert_opts;
        insert_opts.write_concern(wc_majority);

        for (auto&& doc : test["data"].get_array().value) {
            coll.insert_one(doc.get_document().value, insert_opts);
        }
    }
}

void initialize_collection(collection* coll, array::view initial_data) {
    // Deleting all documents from the collection has much better performance than dropping the
    // collection
    coll->delete_many({});

    std::vector<document::view> documents_to_insert;
    for (auto&& document : initial_data) {
        documents_to_insert.push_back(document.get_document().value);
    }

    if (documents_to_insert.size() > 0) {
        write_concern wc_majority;
        wc_majority.acknowledge_level(write_concern::level::k_majority);

        options::insert insert_opts;
        insert_opts.write_concern(wc_majority);

        coll->insert_many(documents_to_insert, insert_opts);
    }
}

void parse_database_options(document::view op, database* out) {
    if (op["databaseOptions"]) {
        auto rc = lookup_read_concern(op["databaseOptions"].get_document());
        if (rc) {
            out->read_concern(*rc);
        }

        auto wc = lookup_write_concern(op["databaseOptions"].get_document());
        if (wc) {
            out->write_concern(*wc);
        }

        auto rp = lookup_read_preference(op["databaseOptions"].get_document());
        if (rp) {
            out->read_preference(*rp);
        }
    }
}

void parse_collection_options(document::view op, collection* out) {
    if (op["collectionOptions"]) {
        auto rc = lookup_read_concern(op["collectionOptions"].get_document());
        if (rc) {
            out->read_concern(*rc);
        }

        auto wc = lookup_write_concern(op["collectionOptions"].get_document());
        if (wc) {
            out->write_concern(*wc);
        }

        auto rp = lookup_read_preference(op["collectionOptions"].get_document());
        if (rp) {
            out->read_preference(*rp);
        }
    }
}

void run_operation_check_result(document::view op, make_op_runner_fn make_op_runner) {
    std::string error_msg;
    optional<document::value> server_error;
    optional<operation_exception> op_exception;
    optional<std::exception> exception;
    optional<document::value> actual_result;

    INFO("Operation: " << bsoncxx::to_json(op));
    try {
        auto op_runner = make_op_runner();
        actual_result = op_runner.run(op);
    } catch (const operation_exception& e) {
        error_msg = e.what();
        server_error = e.raw_server_error();
        op_exception = e;
        exception = e;
    } catch (std::exception& e) {
        error_msg = e.what();
        exception = e;
    }

    // "If the result document has an 'errorContains' field, verify that the method threw an
    // exception or returned an error, and that the value of the 'errorContains' field
    // matches the error string."
    if (op["result"]["errorContains"]) {
        REQUIRE(exception);
        INFO("expected error message " << op["result"]["errorContains"].get_utf8().value);
        INFO("got error message" << error_msg);
        // Do a case insensitive check.
        auto error_contains =
            test_util::tolowercase(op["result"]["errorContains"].get_utf8().value);
        REQUIRE(test_util::tolowercase(error_msg).find(error_contains) < error_msg.length());
    } else {
        if (exception) {
            FAIL("operation " << bsoncxx::to_json(op) << " threw an unexpected exception: "
                              << exception->what());
        }
    }

    // "If the result document has an 'errorCodeName' field, verify that the method threw a
    // command failed exception or returned an error, and that the value of the
    // 'errorCodeName' field matches the 'codeName' in the server error response."
    if (op["result"]["errorCodeName"]) {
        REQUIRE(op_exception);
        uint32_t expected = error_code_from_name(op["result"]["errorCodeName"].get_utf8().value);
        REQUIRE(op_exception->code().value() == static_cast<int>(expected));
    }

    // "If the result document has an 'errorLabelsContain' field, [...] Verify that all of
    // the error labels in 'errorLabelsContain' are present"
    if (op["result"]["errorLabelsContain"]) {
        REQUIRE(op_exception);
        for (auto&& label_el : op["result"]["errorLabelsContain"].get_array().value) {
            auto label = label_el.get_utf8().value;
            REQUIRE(op_exception->has_error_label(label));
        }
    }

    // "If the result document has an 'errorLabelsOmit' field, [...] Verify that none of the
    // error labels in 'errorLabelsOmit' are present."
    if (op["result"]["errorLabelsOmit"]) {
        REQUIRE(op_exception);
        for (auto&& label_el : op["result"]["errorLabelsOmit"].get_array().value) {
            auto label = label_el.get_utf8().value;
            REQUIRE(!op_exception->has_error_label(label));
        }
    }

    // "If the operation returns a raw command response, eg from runCommand, then compare
    // only the fields present in the expected result document. Otherwise, compare the
    // method's return value to result using the same logic as the CRUD Spec Tests runner."
    if (!exception && op["result"]) {
        REQUIRE(actual_result);
        REQUIRE(actual_result->view()["result"]);
        INFO("actual result: " << bsoncxx::to_json(actual_result->view()));
        INFO("expected result: " << bsoncxx::to_json(op));

        REQUIRE(test_util::matches(actual_result->view()["result"].get_value(),
                                   op["result"].get_value()));
    }
}

uri get_uri(document::view test) {
    std::string uri_string = "mongodb://localhost/?";
    auto add_opt = [&uri_string](std::string opt) {
        if (uri_string.back() != '?') {
            uri_string += '&';
        }
        uri_string += opt;
    };

    if (test["clientOptions"]) {
        if (test["clientOptions"]["retryWrites"]) {
            add_opt(std::string("retryWrites=") +
                    (test["clientOptions"]["retryWrites"].get_bool().value ? "true" : "false"));
        }
        if (test["clientOptions"]["retryReads"]) {
            add_opt(std::string("retryReads=") +
                    (test["clientOptions"]["retryReads"].get_bool().value ? "true" : "false"));
        }
        if (test["clientOptions"]["readConcernLevel"]) {
            add_opt("readConcernLevel=" +
                    std::string(test["clientOptions"]["readConcernLevel"].get_utf8().value));
        }
        if (test["clientOptions"]["w"]) {
            if (test["clientOptions"]["w"].type() == type::k_int32) {
                add_opt("w=" + std::to_string(test["clientOptions"]["w"].get_int32().value));
            } else {
                add_opt("w=" + string::to_string(test["clientOptions"]["w"].get_utf8().value));
            }
        }
        if (test["clientOptions"]["heartbeatFrequencyMS"]) {
            add_opt(
                "heartbeatFrequencyMS=" +
                std::to_string(test["clientOptions"]["heartbeatFrequencyMS"].get_int32().value));
        }
        if (test["clientOptions"]["readPreference"]) {
            add_opt("readPreference=" +
                    string::to_string(test["clientOptions"]["readPreference"].get_utf8().value));
        }
    }
    return uri{uri_string};
}

void run_tests_in_suite(std::string ev, test_runner cb, std::set<std::string> unsupported_tests) {
    char* tests_path = std::getenv(ev.c_str());
    INFO("checking for path from environment variable: " << ev);
    REQUIRE(tests_path);

    std::string path{tests_path};
    if (path.back() == '/') {
        path.pop_back();
    }

    std::ifstream test_files{path + "/test_files.txt"};
    REQUIRE(test_files.good());

    std::string test_file;
    while (std::getline(test_files, test_file)) {
        if (unsupported_tests.find(test_file) != unsupported_tests.end()) {
            WARN("Skipping unsupported test file: " << test_file);
            continue;
        }
        cb(path + "/" + test_file);
    }
}

void run_tests_in_suite(std::string ev, test_runner cb) {
    std::set<std::string> empty;
    run_tests_in_suite(ev, cb, empty);
}

void test_setup(document::view test, document::view test_spec) {
    // Step 1. "clean up any open transactions from previous test failures"
    client client{uri{}};
    try {
        client["admin"].run_command(make_document(kvp("killAllSessions", make_array())));
    } catch (const operation_exception& e) {
    }

    // Steps 2 - 5, set up new collection
    set_up_collection(client, test_spec);

    // Step 6. "If failPoint is specified, its value is a configureFailPoint command"
    configure_fail_point(client, test);
}

void parse_session_opts(document::view session_opts, options::client_session* out) {
    options::transaction txn_opts;
    if (session_opts["defaultTransactionOptions"]) {
        auto rc = lookup_read_concern(session_opts["defaultTransactionOptions"].get_document());
        if (rc) {
            txn_opts.read_concern(*rc);
        }

        auto wc = lookup_write_concern(session_opts["defaultTransactionOptions"].get_document());
        if (wc) {
            txn_opts.write_concern(*wc);
        }

        auto rp = lookup_read_preference(session_opts["defaultTransactionOptions"].get_document());
        if (rp) {
            txn_opts.read_preference(*rp);
        }
    }

    out->default_transaction_opts(txn_opts);
}

using bsoncxx::stdx::string_view;
void run_transaction_operations(document::view test,
                                client* client,
                                string_view db_name,
                                string_view coll_name,
                                client_session* session0,
                                client_session* session1,
                                bool* fail_point_enabled,
                                bool throw_on_error = false) {
    auto operations = test["operations"].get_array().value;

    REQUIRE(session0);
    REQUIRE(session1);

    for (auto&& op : operations) {
        *fail_point_enabled =
            *fail_point_enabled || op.get_document().value["arguments"]["failPoint"];
        std::string error_msg;
        optional<document::value> server_error;
        optional<operation_exception> exception;
        optional<document::value> actual_result;
        std::error_code ec;
        INFO("Operation: " << bsoncxx::to_json(op.get_document().value));

        auto operation = op.get_document().value;

        // Handle with_transaction separately.
        if (operation["name"].get_utf8().value.compare("withTransaction") == 0) {
            auto session = [&]() {
                if (operation["object"].get_utf8().value.compare("session0") == 0) {
                    return session0;
                } else {
                    return session1;
                }
            }();

            auto with_txn_test_cb = [&](client_session*) {
                // This will get called from client_session::with_transaction.
                // Run the nested operation(s) inside here.
                REQUIRE(operation["arguments"]);
                REQUIRE(operation["arguments"]["callback"]);
                auto callback = operation["arguments"]["callback"].get_document().value;
                run_transaction_operations(callback,
                                           client,
                                           db_name,
                                           coll_name,
                                           session0,
                                           session1,
                                           fail_point_enabled,
                                           true);
            };

            try {
                session->with_transaction(with_txn_test_cb);
            } catch (const operation_exception& e) {
                error_msg = e.what();
                server_error = e.raw_server_error();
                exception = e;
                ec = e.code();
            }
        } else {
            try {
                database db = client->database(db_name);
                parse_database_options(operation, &db);
                collection coll = db[coll_name];
                parse_collection_options(operation, &coll);
                operation_runner op_runner{&db, &coll, session0, session1, client};
                actual_result = op_runner.run(operation);
            } catch (const operation_exception& e) {
                error_msg = e.what();
                server_error = e.raw_server_error();
                exception = e;
                ec = e.code();
            }
        }

        // "If the result document has an 'errorContains' field, verify that the method threw an
        // exception or returned an error, and that the value of the 'errorContains' field
        // matches the error string."
        if (op["result"]["errorContains"]) {
            REQUIRE(exception);
            // Do a case insensitive check.
            auto error_contains =
                test_util::tolowercase(op["result"]["errorContains"].get_utf8().value);
            REQUIRE(test_util::tolowercase(error_msg).find(error_contains) < error_msg.length());
        }

        // "If the result document has an 'errorCodeName' field, verify that the method threw a
        // command failed exception or returned an error, and that the value of the
        // 'errorCodeName' field matches the 'codeName' in the server error response."
        if (op["result"]["errorCodeName"]) {
            REQUIRE(exception);
            REQUIRE(server_error);
            uint32_t expected =
                error_code_from_name(op["result"]["errorCodeName"].get_utf8().value);
            REQUIRE(exception->code().value() == static_cast<int>(expected));
        }

        // "If the result document has an 'errorLabelsContain' field, [...] Verify that all of
        // the error labels in 'errorLabelsContain' are present"
        if (op["result"]["errorLabelsContain"]) {
            REQUIRE(exception);
            for (auto&& label_el : op["result"]["errorLabelsContain"].get_array().value) {
                auto label = label_el.get_utf8().value;
                if (!exception->has_error_label(label)) {
                    FAIL("Expected exception to contain the label '" << label
                                                                     << "' but it did not.\n");
                }
            }
        }

        // "If the result document has an 'errorLabelsOmit' field, [...] Verify that none of the
        // error labels in 'errorLabelsOmit' are present."
        if (op["result"]["errorLabelsOmit"]) {
            REQUIRE(exception);
            for (auto&& label_el : op["result"]["errorLabelsOmit"].get_array().value) {
                auto label = label_el.get_utf8().value;
                if (exception->has_error_label(label)) {
                    FAIL("Expected exception to NOT contain the label '" << label
                                                                         << "' but it did.\n");
                }
            }
        }

        // "If the operation returns a raw command response, eg from runCommand, then compare
        // only the fields present in the expected result document. Otherwise, compare the
        // method's return value to result using the same logic as the CRUD Spec Tests runner."
        if (!exception && op["result"]) {
            REQUIRE(actual_result);
            REQUIRE(actual_result->view()["result"]);
            INFO("actual result" << bsoncxx::to_json(actual_result->view()));
            INFO("expected result" << bsoncxx::to_json(op.get_document().value));
            REQUIRE(test_util::matches(actual_result->view()["result"].get_value(),
                                       op["result"].get_value()));
        }

        // If we are running inside with_transaction, we need to rethrow what we caught.
        if (throw_on_error && exception) {
            throw operation_exception(ec, std::move(*server_error), error_msg);
        }
    }
}

void run_transactions_tests_in_file(const std::string& test_path) {
    INFO("Test path: " << test_path);
    auto test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);
    auto test_spec_view = test_spec->view();
    auto db_name = test_spec_view["database_name"].get_utf8().value;
    auto coll_name = test_spec_view["collection_name"].get_utf8().value;
    auto tests = test_spec_view["tests"].get_array().value;

    /* we may not have a supported topology */
    if (should_skip_spec_test(client{uri{}}, test_spec_view)) {
        WARN("File skipped - " + test_path);
        return;
    }

    for (auto&& test : tests) {
        bool fail_point_enabled = (bool)test["failPoint"];
        auto description = test["description"].get_utf8().value;
        INFO("Test description: " << description);
        if (should_skip_spec_test(client{uri{}}, test.get_document().value)) {
            continue;
        }

        // Steps 1-6.
        test_setup(test.get_document().value, test_spec_view);

        // Step 7. "Create a new MongoClient client, with Command Monitoring listeners enabled."
        options::client client_opts;
        apm_checker apm_checker;
        client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));
        client client;
        if (test["useMultipleMongoses"]) {
            client = {uri{"mongodb://localhost:27017,localhost:27018"}, client_opts};
        } else {
            client = {get_uri(test.get_document().value), client_opts};
        }

        /* individual test may contain a skipReason */
        if (should_skip_spec_test(client, test.get_document())) {
            continue;
        }

        options::client_session session0_opts;
        options::client_session session1_opts;

        // Step 8: "Call client.startSession twice to create ClientSession objects"
        if (test["sessionOptions"] && test["sessionOptions"]["session0"]) {
            parse_session_opts(test["sessionOptions"]["session0"].get_document().value,
                               &session0_opts);
        }
        if (test["sessionOptions"] && test["sessionOptions"]["session1"]) {
            parse_session_opts(test["sessionOptions"]["session1"].get_document().value,
                               &session1_opts);
        }

        document::value session_lsid0{{}};
        document::value session_lsid1{{}};

        // We wrap this section in its own scope as a way to control when the client_session
        // objects created inside get destroyed. On destruction, client_sessions can send
        // an abortTransaction that some of the spec tests look for.

        {
            client_session session0 = client.start_session(session0_opts);
            client_session session1 = client.start_session(session1_opts);
            session_lsid0.reset(session0.id());
            session_lsid1.reset(session1.id());

            // Step 9. Perform the operations.
            apm_checker.clear();

            run_transaction_operations(test.get_document().value,
                                       &client,
                                       db_name,
                                       coll_name,
                                       &session0,
                                       &session1,
                                       &fail_point_enabled);

            // Step 10. "Call session0.endSession() and session1.endSession." (done in destructors).
        }

        // Step 11. Compare APM events.
        test_util::match_visitor visitor = [&](
            bsoncxx::stdx::string_view key,
            bsoncxx::stdx::optional<bsoncxx::types::bson_value::view> main,
            bsoncxx::types::bson_value::view pattern) {
            if (key.compare("lsid") == 0) {
                REQUIRE(pattern.type() == type::k_utf8);
                REQUIRE(main);
                REQUIRE(main->type() == type::k_document);
                auto session_name = pattern.get_utf8().value;
                if (session_name.compare("session0") == 0) {
                    REQUIRE(test_util::matches(session_lsid0, main->get_document().value));
                } else {
                    REQUIRE(test_util::matches(session_lsid1, main->get_document().value));
                }
                return test_util::match_action::k_skip;
            } else if (pattern.type() == type::k_null) {
                if (main) {
                    return test_util::match_action::k_not_equal;
                }
                return test_util::match_action::k_skip;
            }
            return test_util::match_action::k_skip;
        };

        if (test["expectations"]) {
            apm_checker.compare(test["expectations"].get_array().value, false, visitor);
        }

        // Step 12. Disable the failpoint.
        if (fail_point_enabled) {
            disable_fail_point("mongodb://localhost:27017", client_opts);
            if (test["useMultipleMongoses"]) {
                disable_fail_point("mongodb://localhost:27018", client_opts);
            }
        }

        // Step 13. Compare the collection outcomes
        if (test["outcome"] && test["outcome"]["collection"]) {
            auto outcome_coll_name = coll_name;
            if (test["outcome"]["collection"]["name"]) {
                outcome_coll_name = test["outcome"]["collection"]["name"].get_utf8().value;
            }
            auto coll = client[db_name][outcome_coll_name];
            test_util::check_outcome_collection(&coll,
                                                test["outcome"]["collection"].get_document().value);
        }
    }
}

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
