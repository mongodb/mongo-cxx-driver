// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/operation.hh>
#include <mongocxx/test/spec/util.hh>

namespace {

using namespace bsoncxx::stdx;
using namespace bsoncxx::string;
using namespace mongocxx::spec;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

const std::string kMongohouseURI = "mongodb://mhuser:pencil@localhost";

void run_mongohouse_tests_in_file(std::string test_path) {
    INFO("Test path: " << test_path);
    optional<document::value> test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);

    options::client client_opts;
    apm_checker apm_checker;
    client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));
    client client{uri{kMongohouseURI}, client_opts};

    document::view test_spec_view = test_spec->view();
    if (should_skip_spec_test(client, test_spec_view)) {
        return;
    }

    apm_checker.clear();

    for (auto&& test : test_spec_view["tests"].get_array().value) {
        auto description = test["description"].get_string().value;
        SECTION(to_string(description)) {
            auto get_value_or_default = [&](std::string key, std::string default_str) {
                if (test_spec_view[key]) {
                    return to_string(test_spec_view[key].get_string().value);
                }
                return default_str;
            };

            auto database_name = get_value_or_default("database_name", "crud_test");
            auto collection_name = get_value_or_default("collection_name", "test");

            auto database = client[database_name];
            auto collection = database[collection_name];

            operation_runner op_runner{&database, &collection, nullptr, nullptr, &client};

            std::string outcome_collection_name = collection_name;
            if (test["outcome"] && test["outcome"]["collection"]["name"]) {
                outcome_collection_name =
                    to_string(test["outcome"]["collection"]["name"].get_string().value);
                auto outcome_collection = database[outcome_collection_name];
                initialize_collection(&outcome_collection, array::view{});
            }

            configure_fail_point(client, test.get_document().value);

            auto perform_op =
                [&database, &op_runner, &test, &outcome_collection_name](document::view operation) {
                    optional<document::value> actual_outcome_value;
                    INFO("Operation: " << bsoncxx::to_json(operation));
                    try {
                        actual_outcome_value = op_runner.run(operation);  // here
                    } catch (const mongocxx::operation_exception& e) {
                        REQUIRE([&operation, &test, &e]() {
                            if (operation["error"]) { /* v2 tests expect tests[i].operation.error */
                                return operation["error"].get_bool().value;
                            } else if (test["outcome"] && test["outcome"]["error"]) {
                                /* v1 tests expect tests[i].outcome.error (but some tests may
                                 have "outcome" without a nested "error") */
                                return test["outcome"]["error"].get_bool().value;
                            } else {
                                WARN("Caught operation exception: " << e.what());
                                return false;
                            }
                        }());
                        return; /* do not check results if error is expected */
                    } catch (const std::exception& e) {
                        WARN("Caught exception: " << e.what());
                    } catch (...) {
                        WARN("Caught unknown exception");
                    }

                    if (test["outcome"]) {
                        if (test["outcome"]["collection"]) {
                            auto outcome_collection = database[outcome_collection_name];
                            test_util::check_outcome_collection(
                                &outcome_collection,
                                test["outcome"]["collection"].get_document().value);
                        }

                        if (test["outcome"]["result"]) {
                            // wrap the result, since it might not be a document.
                            bsoncxx::document::view actual_outcome = actual_outcome_value->view();
                            auto actual_result_wrapped =
                                make_document(kvp("result", actual_outcome["result"].get_value()));
                            auto expected_result_wrapped =
                                make_document(kvp("result", test["outcome"]["result"].get_value()));
                            REQUIRE_BSON_MATCHES(actual_result_wrapped, expected_result_wrapped);
                        }
                    }
                };

            if (test["operations"]) {
                /* v2 tests expect a tests[i].operations array */
                for (auto&& operation : test["operations"].get_array().value) {
                    perform_op(operation.get_document().value);
                }
            } else if (test["operation"]) {
                /* v1 tests expect a single document, tests[i].operation */
                perform_op(test["operation"].get_document().value);
            }

            if (test["expectations"]) {
                /* some tests use empty documents, instead of arrays */
                if (test["expectations"].type() == type::k_array) {
                    apm_checker.compare(test["expectations"].get_array().value, true);
                } else {
                    REQUIRE(test["expectations"].get_document().view().empty());
                }
            }

            if (test["failPoint"]) {
                disable_fail_point(client,
                                   test["failPoint"]["configureFailPoint"].get_string().value);
            }
        }
    }
}

// Test that the driver properly constructs and issues a killCursors command to Atlas Data Lake.
void test_kill_cursors() {
    instance::current();

    options::client client_opts;
    apm_checker apm_checker;
    client_opts.apm_opts(apm_checker.get_apm_opts(false));
    client client{uri{kMongohouseURI}, client_opts};

    // Issue a query that will leave a cursor open on the server.
    options::find find_opts;
    find_opts.batch_size(2);
    find_opts.limit(3);

    bsoncxx::types::bson_value::value cursor_id_val{bsoncxx::types::bson_value::view{}};
    std::string cursor_ns;
    bool find_command_found = false;

    {
        auto cursor = client["test"]["driverdata"].find({}, find_opts);

        // Call begin() to run the find on the server.
        cursor.begin();

        // Observe the CommandSucceededEvent event for the find command.
        for (auto&& doc : apm_checker) {
            auto event = doc.view();
            if (event.find("command_succeeded_event") == event.end()) {
                continue;
            }

            if (event["command_succeeded_event"]["command_name"].get_string().value != "find") {
                continue;
            }

            // Grab cursor information.
            auto reply = event["command_succeeded_event"]["reply"].get_document();
            auto cursor_doc = reply.view()["cursor"].get_document();
            cursor_id_val = cursor_doc.view()["id"].get_owning_value();
            cursor_ns = std::string{cursor_doc.view()["ns"].get_string().value};

            find_command_found = true;

            break;
        }

        REQUIRE(find_command_found);

        apm_checker.clear();

        // Destroy the cursor with scope end.
    }

    bool cmd_started_validated = false;
    bool cmd_succeeded_validated = false;

    // Observe events for the killCursors command.
    for (auto&& doc : apm_checker) {
        auto event = doc.view();

        // Use the command started event for killCursors to validate cursor info.
        if (event.find("command_started_event") != event.end()) {
            if (event["command_started_event"]["command_name"].get_string().value !=
                "killCursors") {
                continue;
            }

            // Validate namespace
            auto db = event["command_started_event"]["database_name"].get_string().value;
            auto coll = event["command_started_event"]["command"]["killCursors"].get_string().value;
            std::string cmd_ns{db};
            cmd_ns += ".";
            cmd_ns += std::string{coll};

            if (cmd_ns.compare(cursor_ns) != 0) {
                continue;
            }

            auto cursors_killed =
                event["command_started_event"]["command"]["cursors"].get_array().value;

            if (cursors_killed.has_value(cursor_id_val.view())) {
                cmd_started_validated = true;
            }
        }

        // Use the command succeeded event to confirm that the cursor was killed.
        if (event.find("command_succeeded_event") != event.end()) {
            if (event["command_succeeded_event"]["command_name"].get_string().value !=
                "killCursors") {
                continue;
            }

            auto cursors_killed_elem = event["command_succeeded_event"]["reply"]["cursorsKilled"];
            auto cursors_killed_val = cursors_killed_elem.get_value();
            auto cursors_killed_arr = cursors_killed_elem.get_array();
            auto cursors_killed = cursors_killed_arr.value;

            if (cursors_killed.has_value(cursor_id_val.view())) {
                cmd_succeeded_validated = true;
            }
        }
    }

    REQUIRE(cmd_started_validated);
    REQUIRE(cmd_succeeded_validated);
}

// Test that the driver can establish a connection with Atlas Data Lake without authentication.
// For these tests, create a MongoClient using a valid connection string without auth
// credentials and execute a ping command.
void test_connection_without_auth() {
    instance::current();

    client client{uri{}};
    client["admin"].run_command(make_document(kvp("ping", 1)));
}

// Test that the driver can establish a connection with Atlas Data Lake with authentication.
// For these tests, create a MongoClient using a valid connection string with SCRAM-SHA-1
// and credentials from the drivers-evergreen-tools ADL configuration and execute a ping
// command. Repeat this test using SCRAM-SHA-256.
void test_auth_with_scram_sha() {
    instance::current();

    client client1{uri{"mongodb://mhuser:pencil@localhost/?authMechanism=SCRAM-SHA-1"}};
    client1["admin"].run_command(make_document(kvp("ping", 1)));

    client client256{uri{"mongodb://mhuser:pencil@localhost/?authMechanism=SCRAM-SHA-256"}};
    client256["admin"].run_command(make_document(kvp("ping", 1)));
}

TEST_CASE("Test mongohouse", "[mongohouse]") {
    instance::current();

    run_tests_in_suite("MONGOHOUSE_TESTS_PATH", &run_mongohouse_tests_in_file);

    // Run prose tests
    test_kill_cursors();
    test_connection_without_auth();
    test_auth_with_scram_sha();
}

}  // namespace
