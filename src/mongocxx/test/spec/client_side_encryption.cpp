// Copyright 2019-present MongoDB Inc.
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

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/operation.hh>
#include <mongocxx/test/spec/util.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {

using namespace bsoncxx;
using namespace mongocxx;
using namespace spec;

void _set_up_key_vault(const client& client, document::view test_spec_view) {
    if (test_spec_view["key_vault_data"]) {
        write_concern wc_majority;
        wc_majority.acknowledge_level(write_concern::level::k_majority);

        auto coll = client["keyvault"]["datakeys"];
        coll.drop(wc_majority);

        for (auto&& doc : test_spec_view["key_vault_data"].get_array().value) {
            options::insert insert_opts;
            insert_opts.write_concern(wc_majority);
            coll.insert_one(doc.get_document().value, insert_opts);
        }
    }
}

void add_auto_encryption_opts(document::view test, options::client* client_opts) {
    if (test["clientOptions"]["autoEncryptOpts"]) {
        auto test_encrypt_opts = test["clientOptions"]["autoEncryptOpts"].get_document().value;

        options::auto_encryption auto_encrypt_opts{};

        if (test_encrypt_opts["bypassAutoEncryption"]) {
            auto_encrypt_opts.bypass_auto_encryption(
                test_encrypt_opts["bypassAutoEncryption"].get_bool().value);
        }

        if (test_encrypt_opts["keyVaultNamespace"]) {
            auto ns_string =
                string::to_string(test_encrypt_opts["keyVaultNamespace"].get_utf8().value);
            auto dot = ns_string.find(".");
            std::string db = ns_string.substr(0, dot);
            std::string coll = ns_string.substr(dot + 1);

            auto_encrypt_opts.key_vault_namespace({db.c_str(), coll.c_str()});
        } else {
            auto_encrypt_opts.key_vault_namespace({"keyvault", "datakeys"});
        }

        if (test_encrypt_opts["schemaMap"]) {
            auto_encrypt_opts.schema_map(test_encrypt_opts["schemaMap"].get_document().value);
        }

        if (test_encrypt_opts["kmsProviders"]) {
            using bsoncxx::builder::basic::kvp;
            using bsoncxx::builder::basic::sub_document;

            auto kms_doc = bsoncxx::builder::basic::document{};

            // Add aws credentials (from the environment)
            if (test_encrypt_opts["kmsProviders"]["aws"]) {
                auto access_key = std::getenv("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY");
                auto key_id = std::getenv("MONGOCXX_TEST_AWS_ACCESS_KEY_ID");

                if (!access_key || !key_id) {
                    FAIL(
                        "Please set environment variables for client side encryption tests:\n"
                        "\tMONGOCXX_TEST_AWS_SECRET_ACCESS_KEY\n"
                        "\tMONGOCXX_TEST_AWS_ACCESS_KEY_ID\n\n");
                }

                kms_doc.append(kvp("aws", [&](sub_document subdoc) {
                    subdoc.append(kvp("secretAccessKey", access_key));
                    subdoc.append(kvp("accessKeyId", key_id));
                }));
            }

            // Add local credentials (from the json file)
            if (test_encrypt_opts["kmsProviders"]["local"]) {
                kms_doc.append(
                    kvp("local", test_encrypt_opts["kmsProviders"]["local"].get_document().value));
            }

            auto_encrypt_opts.kms_providers({kms_doc.extract()});
        }

        char* bypass_spawn = std::getenv("ENCRYPTION_TESTS_BYPASS_SPAWN");
        char* mongocryptd_path = std::getenv("MONGOCRYPTD_PATH");

        if (bypass_spawn || mongocryptd_path) {
            auto cmd = bsoncxx::builder::basic::document{};

            if (bypass_spawn && strcmp(bypass_spawn, "TRUE") == 0) {
                cmd.append(bsoncxx::builder::basic::kvp("mongocryptdBypassSpawn", true));
            }

            if (mongocryptd_path) {
                cmd.append(bsoncxx::builder::basic::kvp("mongocryptdSpawnPath", mongocryptd_path));
            }

            auto_encrypt_opts.extra_options({cmd.extract()});
        }

        client_opts->auto_encryption_opts(std::move(auto_encrypt_opts));
    }
}

void run_encryption_tests_in_file(const std::string& test_path) {
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

    class client setup_client {
        uri {}
    };

    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);

    for (auto&& test : tests) {
        auto description = test["description"].get_utf8().value;
        INFO("Test description: " << description);
        if (should_skip_spec_test(client{uri{}}, test.get_document().value)) {
            continue;
        }

        options::client client_opts;

        apm_checker apm_checker;
        client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));

        add_auto_encryption_opts(test.get_document().value, &client_opts);

        if (strcmp(test["description"].get_utf8().value.data(),
                   "operation fails with maxWireVersion < 8") == 0) {
            // We cannot create a client with auto encryption enabled on 4.0,
            // and it fails in different ways on Windows and POSIX, so rather
            // than running this test, skip it.
            continue;
        }

        bool check_results_logging = false;
        if (strcmp(test["description"].get_utf8().value.data(),
                   "Insert with deterministic encryption, then find it") == 0) {
            // CDRIVER-3566 Remove this once windows is debugged.
            check_results_logging = true;
        }

        class client client {
            get_uri(test.get_document().value), std::move(client_opts)
        };

        auto db = client[db_name];
        auto test_coll = db[coll_name];

        _set_up_key_vault(setup_client, test_spec_view);
        set_up_collection(setup_client, test_spec_view);

        for (auto&& op : test["operations"].get_array().value) {
            if (check_results_logging) {
                fprintf(stdout,
                        "about to run operation %s\n",
                        to_json(op.get_document().value).c_str());
                fprintf(stdout, "collection contents before: \n");
                auto cursor = test_coll.find({});
                for (auto&& doc : cursor) {
                    fprintf(stdout, "%s\n", to_json(doc).c_str());
                }
                fprintf(stdout, "\n\n");
            }

            run_operation_check_result(op.get_document().value, [&]() {
                return operation_runner{&db, &test_coll};
            });

            if (check_results_logging) {
                fprintf(stdout, "after running operation, collection contents:\n");
                auto cursor = test_coll.find({});
                for (auto&& doc : cursor) {
                    fprintf(stdout, "%s\n", to_json(doc).c_str());
                }
                fprintf(stdout, "\n\n");
            }
        }

        if (test["expectations"]) {
            // remove this if statement
            if (!check_results_logging) {
                apm_checker.compare(test["expectations"].get_array().value, true);
            }
        }

        if (test["outcome"] && test["outcome"]["collection"]) {
            class client plaintext_client {
                uri {}
            };

            read_preference rp;
            read_concern rc;
            rp.mode(read_preference::read_mode::k_primary);
            rc.acknowledge_level(read_concern::level::k_local);

            auto outcome_coll = plaintext_client[db_name][coll_name];
            outcome_coll.read_concern(rc);
            outcome_coll.read_preference(std::move(rp));

            test_util::check_outcome_collection(&outcome_coll,
                                                test["outcome"]["collection"].get_document().value);
        }
    }
}

TEST_CASE("Client side encryption spec automated tests", "[client_side_encryption_spec]") {
    instance::current();
    /* Tests that use operations that the C++ driver does not have. */
    std::set<std::string> unsupported_tests = {"count.json", "unsupportedCommand.json"};

    char* encryption_tests_path = std::getenv("ENCRYPTION_TESTS_PATH");
    REQUIRE(encryption_tests_path);

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    std::string path{encryption_tests_path};
    if (path.back() == '/') {
        path.pop_back();
    }

    std::ifstream test_files{path + "/test_files.txt"};
    REQUIRE(test_files.good());

    std::string test_file;
    while (std::getline(test_files, test_file)) {
        if (std::find(unsupported_tests.begin(), unsupported_tests.end(), test_file) !=
            unsupported_tests.end()) {
            WARN("skipping " << test_file << " due to unsupported operation");
            continue;
        }
        run_encryption_tests_in_file(path + "/" + test_file);
    }
}

}  // namespace
