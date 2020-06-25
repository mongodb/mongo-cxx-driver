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

#include <mongocxx/test/spec/util.hh>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <third_party/catch/include/catch.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace mongocxx;
using namespace bsoncxx;
using bsoncxx::builder::basic::kvp;
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
    }

    return 0;
}

bool should_skip_spec_test(const client& client, document::view test) {
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
        coll->insert_many(documents_to_insert);
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

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
