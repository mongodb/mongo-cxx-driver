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
#include <string>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/operation.hh>
#include <mongocxx/test/spec/util.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/uri.hpp>

namespace {
using namespace mongocxx;
using namespace bsoncxx;
using namespace bsoncxx::string;
using namespace spec;
using namespace test_util;

void run_change_stream_tests_in_file(const std::string& test_path) {
    INFO("Test path: " << test_path);
    auto test_spec_value = test_util::parse_test_file(test_path);
    REQUIRE(test_spec_value);
    auto test_spec = test_spec_value->view();

    client global_client(uri{});

    std::string db1_name = to_string(test_spec["database_name"].get_utf8().value);
    std::string db2_name = to_string(test_spec["database2_name"].get_utf8().value);
    std::string coll1_name = to_string(test_spec["collection_name"].get_utf8().value);
    std::string coll2_name = to_string(test_spec["collection2_name"].get_utf8().value);
    auto tests = test_spec["tests"].get_array().value;

    // This follows the sketch laid out in the change stream spec tests readme:
    // https://github.com/mongodb/specifications/tree/master/source/change-streams/tests#spec-test-runner
    for (auto&& test_el : tests) {
        auto test = test_el.get_document().value;

        if (should_skip_spec_test(global_client, test)) {
            continue;
        }

        // "Use globalClient to [drop and recreate the collections]".
        global_client[db1_name].drop();
        global_client[db2_name].drop();
        {
            using namespace bsoncxx::builder::basic;
            global_client[db1_name][coll1_name].insert_one(make_document());
            global_client[db2_name][coll2_name].insert_one(make_document());
        }

        // "Create a new MongoClient `client`"
        spec::apm_checker apm_checker;
        options::client client_opts;
        // "Begin monitoring all APM events for `client`"
        client_opts.apm_opts(apm_checker.get_apm_opts());
        class client client(uri{}, client_opts);

        INFO("Test description: " << to_string(test["description"].get_utf8().value));
        if (test["description"].get_utf8().value.compare(
                "Change Stream should error when an invalid aggregation stage is passed in") == 0) {
            WARN("Skipping test with invalid pipeline stages. The C++ driver cannot test them.");
            continue;
        }

        options::change_stream cs_opts{};
        if (test["changeStreamOptions"]) {
            auto options = test["changeStreamOptions"].get_document().value;
            if (options["batchSize"]) {
                cs_opts.batch_size(options["batchSize"].get_int32().value);
            }
        }

        // "Using client, create a changeStream changeStream against the specified target"
        pipeline pipeline{};
        auto cs = [&]() {
            if (test["changeStreamPipeline"]) {
                pipeline = build_pipeline(test["changeStreamPipeline"].get_array().value);
            }
            auto target = std::string(test["target"].get_utf8().value);
            if (target == "collection") {
                return client[db1_name][coll1_name].watch(pipeline, cs_opts);
            } else if (target == "database") {
                return client[db1_name].watch(pipeline, cs_opts);
            } else {
                return client.watch(pipeline);
            }
        }();

        // "Using `globalClient`, run every operation in operations in serial against the server."
        if (test["operations"]) {
            for (auto&& operation : test["operations"].get_array().value) {
                std::string operation_name = to_string(operation["name"].get_utf8().value);
                auto dbname = to_string(operation["database"].get_utf8().value);
                auto collname = to_string(operation["collection"].get_utf8().value);
                auto coll = global_client[dbname][collname];
                operation_runner op_runner{&coll};
                op_runner.run(operation.get_document().value);
            }
        }

        // "Wait until either: (1) An error occurs (2) All operations have been successful"
        bool had_error = false;
        auto expected_result = test["result"].get_document().value;
        std::vector<document::value> changes;
        try {
            for (auto&& change : cs) {
                /* store a copy of the event. */
                changes.emplace_back(document::value(change));
            }
        } catch (operation_exception& oe) {
            REQUIRE(expected_result["error"]);
            auto actual_error = oe.raw_server_error();
            REQUIRE(actual_error);
            // "Assert that the error MATCHES results.error"
            REQUIRE(matches(actual_error->view(), expected_result["error"].get_document().value));
            had_error = true;
        };

        // "Assert that the changes received from changeStream MATCH the results in results.success"
        if (!had_error) {
            REQUIRE(expected_result["success"]);
            for (auto&& expected_change : expected_result["success"].get_array().value) {
                auto expected_change_view = expected_change.get_document().value;
                bool found = false;

                for (auto&& change : changes) {
                    if (matches(change.view(), expected_change_view)) {
                        found = true;
                        break;
                    }
                }

                REQUIRE(found);
            }
        }

        // Match captured APM events.
        if (test["expectations"]) {
            apm_checker.compare(test["expectations"].get_array().value, true);
        }
    }
}

TEST_CASE("Change stream spec tests", "[change_stream_spec]") {
    instance::current();

    client client{uri{}};
    if (!test_util::is_replica_set(client)) {
        WARN("Skipping - not a replica set");
        return;
    } else if (test_util::get_max_wire_version(client) < 7) {
        // Change streams require wire version 6, and newer features require 7.
        WARN("Skipping - max wire version is < 7");
        return;
    }

    run_tests_in_suite("CHANGE_STREAM_TESTS_PATH", &run_change_stream_tests_in_file);
}
}  // namespace
