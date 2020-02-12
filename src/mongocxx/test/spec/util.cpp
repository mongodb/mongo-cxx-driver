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

#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
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

static const int kMaxIsMasterFailCommands = 7;

bool should_skip_spec_test(const client& client, document::view test) {
    if (test["skipReason"]) {
        WARN("Test skipped - " << test["description"].get_utf8().value << "\n"
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

    WARN("Skipping - unsupported server version '" + server_version + "' with topology '" +
         topology + "'");
    return true;
}

void disable_fail_point(std::string uri_string, options::client client_opts) {
    mongocxx::client client = {uri{uri_string}, client_opts};
    /* Some transactions tests have a failCommand for "isMaster" repeat seven times. */
    for (int i = 0; i < kMaxIsMasterFailCommands; i++) {
        try {
            client["admin"].run_command(
                make_document(kvp("configureFailPoint", "failCommand"), kvp("mode", "off")));
            break;
        } catch (const std::exception& e) {
            /* Tests that fail with isMaster also fail to disable the failpoint
             * (since we run isMaster when opening the connection). Ignore those
             * errors. */
            continue;
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

void run_tests_in_suite(std::string ev, test_runner cb) {
    char* tests_path = std::getenv(ev.c_str());
    REQUIRE(tests_path);

    std::string path{tests_path};
    if (path.back() == '/') {
        path.pop_back();
    }

    std::ifstream test_files{path + "/test_files.txt"};
    REQUIRE(test_files.good());

    std::string test_file;
    while (std::getline(test_files, test_file)) {
        cb(path + "/" + test_file);
    }
}

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
