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
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using namespace bsoncxx;
using namespace mongocxx;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

// TODO: rename txn_opts_doc parameter to more generalized opts_doc (used by non-txn funcs)
bsoncxx::stdx::optional<read_concern> get_read_concern(document::view txn_opts_doc) {
    if (txn_opts_doc["readConcern"] && txn_opts_doc["readConcern"]["level"]) {
        read_concern rc;
        rc.acknowledge_string(
            string::to_string(txn_opts_doc["readConcern"]["level"].get_utf8().value));
        return rc;
    }

    return {};
}

// TODO: rename txn_opts_doc parameter to more generalized opts_doc (used by non-txn funcs)
bsoncxx::stdx::optional<write_concern> get_write_concern(document::view txn_opts_doc) {
    if (txn_opts_doc["writeConcern"] && txn_opts_doc["writeConcern"]["w"]) {
        write_concern wc;
        document::element w = txn_opts_doc["writeConcern"]["w"];
        if (w.type() == bsoncxx::type::k_utf8) {
            std::string level = string::to_string(w.get_utf8().value);
            if (level.compare("majority") == 0) {
                wc.acknowledge_level(write_concern::level::k_majority);
            } else if (level.compare("acknowledged") == 0) {
                wc.acknowledge_level(write_concern::level::k_acknowledged);
            } else if (level.compare("unacknowledged") == 0) {
                wc.acknowledge_level(write_concern::level::k_unacknowledged);
            }
            // ... TODO: possibly add other levels
        } else if (w.type() == bsoncxx::type::k_int32) {
            wc.nodes(w.get_int32());
        }
        // ... TODO: possibly check for journal
        return wc;
    }

    return {};
}

// TODO: rename txn_opts_doc to more generalize opts_doc (used by non-txn funcs)
bsoncxx::stdx::optional<read_preference> get_read_preference(document::view txn_opts_doc) {
    if (txn_opts_doc["readPreference"] && txn_opts_doc["readPreference"]["mode"]) {
        read_preference rp;
        std::string mode =
            string::to_string(txn_opts_doc["readPreference"]["mode"].get_utf8().value);
        if (mode.compare("Primary") == 0) {
            rp.mode(read_preference::read_mode::k_primary);
        } else if (mode.compare("Primary Preferred") == 0) {  // TODO: check if proper spelling
            rp.mode(read_preference::read_mode::k_primary_preferred);
        } else if (mode.compare("Secondary") == 0) {
            rp.mode(read_preference::read_mode::k_secondary);
        } else if (mode.compare("Secondary Preferred") == 0) {  // TODO: check if proper spelling
            rp.mode(read_preference::read_mode::k_secondary_preferred);
        } else if (mode.compare("Nearest") == 0) {  // TODO: check if proper spelling
            rp.mode(read_preference::read_mode::k_nearest);
        }
        // ... TODO: possibly add tags and/or max_staleness
        return rp;
    }

    return {};
}

document::value run_start_transaction_test(collection* coll,
                                           document::view operation,
                                           client_session* session) {
    (void)coll;
    options::transaction txn_opts;

    std::cerr << "Starting transaction\n";

    if (operation["arguments"] && operation["arguments"]["options"]) {
        document::view opts = operation["arguments"]["options"].get_document();

        auto rc = get_read_concern(opts);
        if (rc) {
            txn_opts.read_concern(*rc);
        }

        auto wc = get_write_concern(opts);
        if (wc) {
            txn_opts.write_concern(*wc);
        }

        auto rp = get_read_preference(opts);
        if (rp) {
            txn_opts.read_preference(*rp);
        }
    }

    session->start_transaction(txn_opts);

    auto result = builder::basic::document{};
    return result.extract();
}

document::value run_commit_transaction_test(collection* coll,
                                            document::view operation,
                                            client_session* session) {
    (void)coll;
    (void)operation;
    std::cerr << "Committing transaction\n";
    session->commit_transaction();
    auto result = builder::basic::document{};
    return result.extract();
}

document::value run_insert_one_test(collection* coll,
                                    document::view operation,
                                    client_session* session) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view document = arguments["document"].get_document().value;
    auto result = builder::basic::document{};

    bsoncxx::stdx::optional<result::insert_one> insert_one_result;
    if (session) {
        insert_one_result = coll->insert_one(*session, document);
    } else {
        insert_one_result = coll->insert_one(document);
    }

    types::value inserted_id{types::b_null{}};

    if (insert_one_result) {
        inserted_id = insert_one_result->inserted_id();
    }

    result.append(builder::basic::kvp("result", [inserted_id](builder::basic::sub_document subdoc) {
        subdoc.append(builder::basic::kvp("insertedId", inserted_id));

    }));

    return result.extract();
}

document::value run_insert_many_test(collection* coll,
                                     document::view operation,
                                     client_session* session) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    document::view arguments = operation["arguments"].get_document().value;
    array::view documents = arguments["documents"].get_array().value;
    std::vector<document::view> documents_to_insert{};

    for (auto&& element : documents) {
        documents_to_insert.push_back(element.get_document().value);
    }

    bsoncxx::stdx::optional<result::insert_many> insert_many_result;
    if (session) {
        insert_many_result = coll->insert_many(*session, documents_to_insert);
    } else {
        insert_many_result = coll->insert_many(documents_to_insert);
    }

    std::map<size_t, document::element> inserted_ids{};

    if (insert_many_result) {
        inserted_ids = insert_many_result->inserted_ids();
    }

    auto result = builder::basic::document{};
    bsoncxx::builder::basic::document inserted_ids_builder;

    for (auto&& id_pair : inserted_ids) {
        inserted_ids_builder.append(kvp(std::to_string(id_pair.first), id_pair.second.get_value()));
    }

    result.append(kvp("result", make_document(kvp("insertedIds", inserted_ids_builder.extract()))));
    return result.extract();
}

std::map<std::string,
         std::function<document::value(collection*, document::view, client_session* session)>>
    transactions_test_runners = {{"startTransaction", run_start_transaction_test},
                                 {"commitTransaction", run_commit_transaction_test},
                                 {"insertOne", run_insert_one_test},
                                 {"insertMany", run_insert_many_test}};

void run_transactions_tests_in_file(std::string test_path) {
    INFO("Test path: " << test_path);
    bsoncxx::stdx::optional<document::value> test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);

    std::cerr << "In run_transactions_tests_in_file\n";
    document::view test_spec_view = test_spec->view();

    std::string db_name = string::to_string(test_spec_view["database_name"].get_utf8().value);
    std::string coll_name = string::to_string(test_spec_view["collection_name"].get_utf8().value);

    array::view tests = test_spec_view["tests"].get_array().value;
    for (auto&& test : tests) {
        std::string description = bsoncxx::string::to_string(test["description"].get_utf8().value);
        INFO("Test description: " << description);

        // Create client and clean up any open transactions from previous test failures
        client tmp_client{uri{}};
        try {
            tmp_client["admin"].run_command(make_document(kvp("killAllSessions", make_array())));
        } catch (const operation_exception& e) {
            // TODO: possibly handle this exception if need be
        };

        // Create collection object from name fields in YAML file
        database tmp_db = tmp_client[db_name];
        collection tmp_coll = tmp_db[coll_name];

        // Drop and create the test collection using majority write concern
        write_concern drop_wc;
        drop_wc.acknowledge_level(write_concern::level::k_majority);
        tmp_coll.drop(drop_wc);
        tmp_coll = tmp_db.create_collection(coll_name);

        // If YAML contains a "data" array, insert those documents into the collection
        if (test_spec_view["data"]) {
            array::view docs = test_spec_view["data"].get_array().value;
            for (auto&& doc : docs) {
                tmp_coll.insert_one(doc.get_document().value);
            }
        }

        // Enable fail point if specified
        if (test["failPoint"]) {
            tmp_client["admin"].run_command(test["failPoint"].get_document().value);
        }

        // TODO: Uncomment below to create the new client with Command Monitoring listeners enabled
        // options::apm apm_opts;
        // TODO: define lambdas
        // apm_opts.on_command_started(...);
        // apm_opts.on_command_failed(...);
        // apm_opts.on_command_succeeded(...);
        // options::client client_opts;
        // client_opts.apm_opts(apm_opts);
        // client client{uri{}, client_opts};

        client client{uri{}};
        options::client_session client_session_opts0;
        options::client_session client_session_opts1;

        // Set session options if specified in YAML
        if (test["sessionOptions"] && test["sessionOptions"]["session0"]) {
            options::transaction txn_opts;
            document::view opts_doc = test["sessionOptions"]["session0"].get_document();

            if (opts_doc["defaultTransactionOptions"]) {
                auto rc = get_read_concern(opts_doc["defaultTransactionOptions"].get_document());
                if (rc) {
                    txn_opts.read_concern(*rc);
                }

                auto wc = get_write_concern(opts_doc["defaultTransactionOptions"].get_document());
                if (wc) {
                    txn_opts.write_concern(*wc);
                }

                auto rp = get_read_preference(opts_doc["defaultTransactionOptions"].get_document());
                if (rp) {
                    txn_opts.read_preference(*rp);
                }
            }

            client_session_opts0.default_transaction_opts(txn_opts);
        }
        if (test["sessionOptions"] && test["sessionOptions"]["session1"]) {
            options::transaction txn_opts;
            document::view opts_doc = test["sessionOptions"]["session1"].get_document();

            if (opts_doc["defaultTransactionOptions"]) {
                auto rc = get_read_concern(opts_doc["defaultTransactionOptions"].get_document());
                if (rc) {
                    txn_opts.read_concern(*rc);
                }

                auto wc = get_write_concern(opts_doc["defaultTransactionOptions"].get_document());
                if (wc) {
                    txn_opts.write_concern(*wc);
                }

                auto rp = get_read_preference(opts_doc["defaultTransactionOptions"].get_document());
                if (rp) {
                    txn_opts.read_preference(*rp);
                }
            }

            client_session_opts1.default_transaction_opts(txn_opts);
        }

        // Create two client_session objects and save their lsid's
        client_session session0 = client.start_session(client_session_opts0);
        client_session session1 = client.start_session(client_session_opts1);
        document::value session_lsid0(session0.id());
        document::value session_lsid1(session1.id());

        array::view operations = test["operations"].get_array().value;
        for (auto&& op : operations) {
            try {
                //                std::cerr << to_json(op.get_document().value) << std::endl;

                database db = client[db_name];
                collection coll = db[coll_name];

                try {
                    db.create_collection(coll_name);
                } catch (const operation_exception& e) {
                }

                std::cerr << "Collection created\n";
                if (op["collectionOptions"]) {
                    auto rc = get_read_concern(op["collectionOptions"].get_document());
                    if (rc) {
                        coll.read_concern(*rc);
                    }

                    auto wc = get_write_concern(op["collectionOptions"].get_document());
                    if (wc) {
                        coll.write_concern(*wc);
                    }

                    auto rp = get_read_preference(op["collectionOptions"].get_document());
                    if (rp) {
                        coll.read_preference(*rp);
                    }
                }

                std::string op_name = bsoncxx::string::to_string(op["name"].get_utf8().value);
                document::view op_doc = op.get_document().value;
                auto run_test = transactions_test_runners[op_name];
                bsoncxx::stdx::optional<document::value> actual_result;

                // If one of the two sessions is an argument to the function, or if it is the
                // object that the function is called on, then we surely need to pass it.
                // NOTES: Order of checks is important here. We start with the most likely place
                //        to find a session requirement for the function. We also assume that the
                //        session's role as an object or an argument is entirely derivative of the
                //        function's name (e.g. sessions can only be objects for startTransaction,
                //        commitTransaction and abortTransaction, and are presumed to be arguments
                //        for all other functions like insertOne, find, etc.).
                if (op["arguments"] && op["arguments"]["session"]) {
                    if (op["arguments"]["session"].get_utf8().value.compare("session0") == 0) {
                        actual_result = run_test(&coll, op_doc, &session0);
                    } else if (op["arguments"]["session"].get_utf8().value.compare("session1") ==
                               0) {
                        actual_result = run_test(&coll, op_doc, &session1);
                    } else {
                        INFO("Unidentified session function argument has been listed.\n");
                        REQUIRE(false);  // the assumptions made here don't follow the spec test
                    }
                } else if (op["object"]) {
                    if (op["object"].get_utf8().value.compare("session0") == 0) {
                        actual_result = run_test(&coll, op_doc, &session0);
                    } else if (op["object"].get_utf8().value.compare("session1") == 0) {
                        actual_result = run_test(&coll, op_doc, &session1);
                    } else if (op["object"].get_utf8().value.compare("collection") == 0) {
                        actual_result = run_test(&coll, op_doc, {});
                    } else {
                        INFO("Unidentified function object has been listed.\n");
                        REQUIRE(false);  // the assumptions made here don't follow the spec test
                    }
                } else {
                    INFO("Neither an object nor arguments provided for given function.\n");
                    REQUIRE(false);  // the assumptions made here don't follow the spec test
                }
            } catch (const operation_exception& e) {
                if (op["result"] && op["result"]["errorContains"]) {
                    REQUIRE(op["result"]["errorContains"].get_utf8().value.compare(
                                std::string(e.what())) == 0);
                }
                if (op["result"] && op["result"]["errorCodeName"]) {
                    // TODO
                }
                if (op["result"] && op["result"]["errorLabelContain"]) {
                    REQUIRE(e.has_error_label(op["result"]["errorLabelContain"].get_utf8().value));
                }
                if (op["result"] && op["result"]["errorLabelsOmit"]) {
                    // TODO
                }
            }
        }

        // Disable fail point in case it was enabled prior
        client["admin"].run_command(
            make_document(kvp("configureFailPoint", "failCommand"), kvp("mode", "off")));
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

    std::ifstream test_files{path + "/test_files.txt"};

    REQUIRE(test_files.good());

    std::string test_file;

    while (std::getline(test_files, test_file)) {
        // TODO: implement remaining functions used in other .json files
        if (test_file.compare("./insert.json") == 0) {
            // TODO: currently hangs on db.create_collection()
            // run_transactions_tests_in_file(path + "/" + test_file);
        }
    }
}
}  // namespace
