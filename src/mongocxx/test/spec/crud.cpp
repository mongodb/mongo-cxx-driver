// Copyright 2016 MongoDB Inc.
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

#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
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

bool is_unsupported(bsoncxx::stdx::string_view description) {
    std::set<std::string> unsupported_tests = {"Deprecated count with empty collection",
                                               "Deprecated count with collation",
                                               "Deprecated count without a filter",
                                               "Deprecated count with a filter",
                                               "Deprecated count with skip and limit"};
    if (unsupported_tests.find(std::string(description)) != unsupported_tests.end()) {
        WARN("Skipping unsupported test: " << description);
        return true;
    }
    return false;
}

void run_crud_tests_in_file(std::string test_path) {
    INFO("Test path: " << test_path);
    optional<document::value> test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);

    options::client client_opts;
    apm_checker apm_checker;
    client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));
    client client{uri{}, client_opts};

    document::view test_spec_view = test_spec->view();
    if (should_skip_spec_test(client, test_spec_view)) {
        return;
    }

    for (auto&& test : test_spec_view["tests"].get_array().value) {
        auto description = test["description"].get_utf8().value;
        SECTION(to_string(description)) {
            if (is_unsupported(test["description"].get_utf8().value)) {
                continue;
            }

            if (should_skip_spec_test(client, test.get_document())) {
                continue;
            }

            auto get_value_or_default = [&](std::string key, std::string default_str) {
                if (test_spec_view[key]) {
                    return to_string(test_spec_view[key].get_utf8().value);
                }
                return default_str;
            };

            auto database_name = get_value_or_default("database_name", "crud_test");
            auto collection_name = get_value_or_default("collection_name", "test");

            auto database = client[database_name];
            auto collection = database[collection_name];

            if (test_spec_view["data"]) {
                initialize_collection(&collection, test_spec_view["data"].get_array().value);
            }

            operation_runner op_runner{&database, &collection};

            std::string outcome_collection_name = collection_name;
            if (test["outcome"] && test["outcome"]["collection"]["name"]) {
                outcome_collection_name =
                    to_string(test["outcome"]["collection"]["name"].get_utf8().value);
                auto outcome_collection = database[outcome_collection_name];
                initialize_collection(&outcome_collection, array::view{});
            }

            configure_fail_point(client, test.get_document().value);

            apm_checker.clear();
            auto perform_op =
                [&database, &op_runner, &test, &outcome_collection_name](document::view operation) {
                    optional<document::value> actual_outcome_value;
                    INFO("Operation: " << bsoncxx::to_json(operation));
                    try {
                        actual_outcome_value = op_runner.run(operation);
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
                if (test["expectations"].type() == type::k_array)
                    apm_checker.compare(test["expectations"].get_array().value, true);
                else
                    REQUIRE(test["expectations"].get_document().view().empty());
            }

            if (test["failPoint"]) {
                disable_fail_point(client,
                                   test["failPoint"]["configureFailPoint"].get_utf8().value);
            }
        }
    }
}

TEST_CASE("CRUD spec automated tests", "[crud_spec]") {
    instance::current();

    run_tests_in_suite("CRUD_TESTS_PATH", &run_crud_tests_in_file);
}
}  // namespace
