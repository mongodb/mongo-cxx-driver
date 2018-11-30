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

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
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
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using namespace bsoncxx;
using namespace mongocxx;
using namespace spec;
using bsoncxx::stdx::string_view;
using bsoncxx::stdx::optional;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

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
    }

    return 0;
}

void test_setup(document::view test,
                document::view test_spec,
                string_view db_name,
                string_view coll_name) {
    // Step 1. "clean up any open transactions from previous test failures"
    client client{uri{}};
    try {
        client["admin"].run_command(make_document(kvp("killAllSessions", make_array())));
    } catch (const operation_exception& e) {
    }

    // Step 2. "Create a collection object from the MongoClient, using the database_name and
    // collection_name fields of the YAML file."
    database db = client[db_name];
    collection coll = db[coll_name];

    // Step 3. "Drop the test collection, using writeConcern 'majority'."
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);
    coll.drop(wc_majority);

    // Step 4. "Execute the 'create' command to recreate the collection"
    coll = db.create_collection(coll_name, {}, wc_majority);

    // Step 5. "If the YAML file contains a data array, insert the documents"
    if (test_spec["data"]) {
        array::view docs = test_spec["data"].get_array().value;
        for (auto&& doc : docs) {
            options::insert insert_opts;
            insert_opts.write_concern(wc_majority);
            coll.insert_one(doc.get_document().value, insert_opts);
        }
    }

    // Step 6. "If failPoint is specified, its value is a configureFailPoint command"
    if (test["failPoint"]) {
        client["admin"].run_command(test["failPoint"].get_document().value);
    }
}

uri get_uri(document::view test) {
    std::string uri_opts = "";
    auto add_opt = [&](std::string opt) {
        if (uri_opts != "") {
            uri_opts += "&" + opt;
        }
        uri_opts = opt;
    };
    if (test["clientOptions"]) {
        if (test["clientOptions"]["retryWrites"]) {
            add_opt(std::string("retryWrites=") +
                    (test["clientOptions"]["retryWrites"].get_bool().value ? "true" : "false"));
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
        if (test["clientOptions"]["readPreference"]) {
            add_opt("readPreference=" +
                    string::to_string(test["clientOptions"]["readPreference"].get_utf8().value));
        }
    }
    return uri{"mongodb://localhost/?" + uri_opts};
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

void run_transactions_tests_in_file(const std::string& test_path) {
    INFO("Test path: " << test_path);
    auto test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);
    auto test_spec_view = test_spec->view();
    auto db_name = test_spec_view["database_name"].get_utf8().value;
    auto coll_name = test_spec_view["collection_name"].get_utf8().value;
    auto tests = test_spec_view["tests"].get_array().value;
    for (auto&& test : tests) {
        auto description = test["description"].get_utf8().value;
        INFO("Test description: " << description);
        if (description.compare("run command fails with explicit secondary read preference") == 0) {
            WARN("Skipping test - read preferences in database::run_command are not supported");
            continue;
        }

        // Steps 1-6.
        test_setup(test.get_document().value, test_spec_view, db_name, coll_name);

        // Step 7. "Create a new MongoClient client, with Command Monitoring listeners enabled."
        options::client client_opts;
        apm_checker apm_checker;
        client_opts.apm_opts(apm_checker.get_apm_opts());
        client client{get_uri(test.get_document().value), client_opts};
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

        client_session session0 = client.start_session(session0_opts);
        client_session session1 = client.start_session(session1_opts);
        document::value session_lsid0(session0.id());
        document::value session_lsid1(session1.id());

        // Step 9. Perform the operations.
        auto operations = test["operations"].get_array().value;
        for (auto&& op : operations) {
            std::string error_msg;
            optional<document::value> server_error;
            optional<operation_exception> exception;
            optional<document::value> actual_result;
            INFO("Operation: " << bsoncxx::to_json(op.get_document().value));
            try {
                auto operation = op.get_document().value;
                database db = client[db_name];
                parse_database_options(operation, &db);
                collection coll = db[coll_name];
                parse_collection_options(operation, &coll);
                operation_runner op_runner{&db, &coll, &session0, &session1};
                actual_result = op_runner.run(operation);
            } catch (const operation_exception& e) {
                error_msg = e.what();
                server_error = e.raw_server_error();
                exception = e;
            }

            // "If the result document has an 'errorContains' field, verify that the method threw an
            // exception or returned an error, and that the value of the 'errorContains' field
            // matches the error string."
            if (op["result"]["errorContains"]) {
                REQUIRE(exception);
                // Do a case insensitive check.
                auto error_contains =
                    test_util::tolowercase(op["result"]["errorContains"].get_utf8().value);
                REQUIRE(test_util::tolowercase(error_msg).find(error_contains) <
                        error_msg.length());
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
                    REQUIRE(exception->has_error_label(label));
                }
            }

            // "If the result document has an 'errorLabelsOmit' field, [...] Verify that none of the
            // error labels in 'errorLabelsOmit' are present."
            if (op["result"]["errorLabelsOmit"]) {
                REQUIRE(exception);
                for (auto&& label_el : op["result"]["errorLabelsOmit"].get_array().value) {
                    auto label = label_el.get_utf8().value;
                    REQUIRE(!exception->has_error_label(label));
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
        }

        // Step 10. "Call session0.endSession() and session1.endSession." (done in destructors).

        // Step 11. Compare APM events.
        test_util::match_visitor visitor = [&](bsoncxx::stdx::string_view key,
                                               bsoncxx::stdx::optional<bsoncxx::types::value> main,
                                               bsoncxx::types::value pattern) {
            if (key.compare("lsid") == 0) {
                REQUIRE(pattern.type() == type::k_utf8);
                REQUIRE(main);
                REQUIRE(main->type() == type::k_document);
                auto session_name = pattern.get_utf8().value;
                if (session_name.compare("session0") == 0) {
                    REQUIRE(test_util::matches(session0.id(), main->get_document().value));
                } else {
                    REQUIRE(test_util::matches(session1.id(), main->get_document().value));
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
            apm_checker.compare(test["expectations"].get_array().value, true, visitor);
        }

        // Step 12. Disable the failpoint.
        if (test["failPoint"]) {
            auto failpoint_name = test["failPoint"]["configureFailPoint"].get_utf8().value;
            client["admin"].run_command(
                make_document(kvp("configureFailPoint", failpoint_name), kvp("mode", "off")));
        }

        // Step 13. Compare the collection outcomes
        if (test["outcome"] && test["outcome"]["collection"]) {
            auto coll = client[db_name][coll_name];
            test_util::check_outcome_collection(&coll,
                                                test["outcome"]["collection"].get_document().value);
        }
    }
}

TEST_CASE("Transactions spec automated tests", "[transactions_spec]") {
    instance::current();

    char* transactions_tests_path = std::getenv("TRANSACTIONS_TESTS_PATH");
    REQUIRE(transactions_tests_path);

    std::string path{transactions_tests_path};
    if (path.back() == '/') {
        path.pop_back();
    }

    client client{uri{}};
    if (!test_util::is_replica_set(client)) {
        WARN("Skipping - not a replica set");
        return;
    } else if (test_util::get_max_wire_version(client) < 7) {
        WARN("Skipping - max wire version is < 7");
        return;
    }

    std::ifstream test_files{path + "/test_files.txt"};
    REQUIRE(test_files.good());

    std::string test_file;
    while (std::getline(test_files, test_file)) {
        run_transactions_tests_in_file(path + "/" + test_file);
    }
}
}  // namespace
