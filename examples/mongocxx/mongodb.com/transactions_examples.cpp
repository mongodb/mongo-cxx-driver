// Copyright 2009-present MongoDB, Inc.
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

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#include <mongocxx/client.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/transaction.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/write_concern.hpp>

#include <examples/macros.hh>

namespace {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace mongocxx;

void transactions_intro_example_1(mongocxx::client& client) {
    // Start Transactions Intro Example 1
    auto update_employee_info = [](client_session& session) {
        auto& client = session.client();
        auto employees = client["hr"]["employees"];
        auto events = client["reporting"]["events"];

        options::transaction txn_opts;
        read_concern rc;
        rc.acknowledge_level(read_concern::level::k_snapshot);
        txn_opts.read_concern(rc);
        write_concern wc;
        wc.acknowledge_level(write_concern::level::k_majority);
        txn_opts.write_concern(wc);
        session.start_transaction(txn_opts);

        try {
            employees.update_one(
                make_document(kvp("employee", 3)),
                make_document(kvp("$set", make_document(kvp("status", "Inactive")))));
            events.insert_one(make_document(
                kvp("employee", 3), kvp("status", make_document(kvp("new", "Inactive"), kvp("old", "Active")))));
        } catch (operation_exception const& oe) {
            std::cout << "Caught exception during transaction, aborting." << std::endl;
            session.abort_transaction();
            throw oe;
        }

        while (true) {
            try {
                session.commit_transaction(); // Uses write concern set at transaction start.
                std::cout << "Transaction committed." << std::endl;
                break;
            } catch (operation_exception const& oe) {
                // Can retry commit.
                if (oe.has_error_label("UnknownTransactionCommitResult")) {
                    std::cout << "UnknownTransactionCommitResult, retrying commit operation ..." << std::endl;
                    continue;
                } else {
                    std::cout << "Error during commit ..." << std::endl;
                    throw oe;
                }
            }
        }
    };
    // End Transactions Intro Example 1

    auto session = client.start_session();
    update_employee_info(session);
}

void transactions_retry_example_1(mongocxx::client& client) {
    // Start Transactions Retry Example 1
    using transaction_func = std::function<void(client_session & session)>;
    auto run_transaction_with_retry = [](transaction_func txn_func, client_session& session) {
        while (true) {
            try {
                txn_func(session); // performs transaction.
                break;
            } catch (operation_exception const& oe) {
                std::cout << "Transaction aborted. Caught exception during transaction." << std::endl;
                // If transient error, retry the whole transaction.
                if (oe.has_error_label("TransientTransactionError")) {
                    std::cout << "TransientTransactionError, retrying transaction ..." << std::endl;
                    continue;
                } else {
                    throw oe;
                }
            }
        }
    };
    // End Transactions Retry Example 1

    auto session = client.start_session();
    run_transaction_with_retry(
        [&client](client_session& session) {
            session.start_transaction();
            auto coll = client["test"]["coll"];
            coll.insert_one(make_document(kvp("x", 1)));
            session.commit_transaction();
        },
        session);
}

void transactions_retry_example_2(mongocxx::client& client) {
    // Start Transactions Retry Example 2
    auto commit_with_retry = [](client_session& session) {
        while (true) {
            try {
                session.commit_transaction(); // Uses write concern set at transaction start.
                std::cout << "Transaction committed." << std::endl;
                break;
            } catch (operation_exception const& oe) {
                // Can retry commit
                if (oe.has_error_label("UnknownTransactionCommitResult")) {
                    std::cout << "UnknownTransactionCommitResult, retrying commit operation ..." << std::endl;
                    continue;
                } else {
                    std::cout << "Error during commit ..." << std::endl;
                    throw oe;
                }
            }
        }
    };
    // End Transactions Retry Example 2

    auto session = client.start_session();
    session.start_transaction();
    auto coll = client["test"]["coll"];
    coll.insert_one(make_document(kvp("x", 1)));
    commit_with_retry(session);
}

void transactions_retry_example_3(mongocxx::client& client) {
    // Start Transactions Retry Example 3
    using transaction_func = std::function<void(client_session & session)>;
    auto run_transaction_with_retry = [](transaction_func txn_func, client_session& session) {
        while (true) {
            try {
                txn_func(session); // performs transaction.
                break;
            } catch (operation_exception const& oe) {
                std::cout << "Transaction aborted. Caught exception during transaction." << std::endl;
                // If transient error, retry the whole transaction.
                if (oe.has_error_label("TransientTransactionError")) {
                    std::cout << "TransientTransactionError, retrying transaction ..." << std::endl;
                    continue;
                } else {
                    throw oe;
                }
            }
        }
    };

    auto commit_with_retry = [](client_session& session) {
        while (true) {
            try {
                session.commit_transaction(); // Uses write concern set at transaction start.
                std::cout << "Transaction committed." << std::endl;
                break;
            } catch (operation_exception const& oe) {
                // Can retry commit
                if (oe.has_error_label("UnknownTransactionCommitResult")) {
                    std::cout << "UnknownTransactionCommitResult, retrying commit operation ..." << std::endl;
                    continue;
                } else {
                    std::cout << "Error during commit ..." << std::endl;
                    throw oe;
                }
            }
        }
    };

    // Updates two collections in a transaction
    auto update_employee_info = [&](client_session& session) {
        auto& client = session.client();
        auto employees = client["hr"]["employees"];
        auto events = client["reporting"]["events"];

        options::transaction txn_opts;
        read_concern rc;
        rc.acknowledge_level(read_concern::level::k_snapshot);
        txn_opts.read_concern(rc);
        write_concern wc;
        wc.acknowledge_level(write_concern::level::k_majority);
        txn_opts.write_concern(wc);

        session.start_transaction(txn_opts);

        try {
            employees.update_one(
                make_document(kvp("employee", 3)),
                make_document(kvp("$set", make_document(kvp("status", "Inactive")))));
            events.insert_one(make_document(
                kvp("employee", 3), kvp("status", make_document(kvp("new", "Inactive"), kvp("old", "Active")))));
        } catch (operation_exception const& oe) {
            std::cout << "Caught exception during transaction, aborting." << std::endl;
            session.abort_transaction();
            throw oe;
        }

        commit_with_retry(session);
    };

    auto session = client.start_session();
    try {
        run_transaction_with_retry(update_employee_info, session);
    } catch (operation_exception const& oe) {
        // Do something with error.
        throw oe;
    }
    // End Transactions Retry Example 3
}

} // namespace

int EXAMPLES_CDECL main() {
    if (char const* const topology_env = std::getenv("MONGOCXX_TEST_TOPOLOGY")) {
        auto const topology = std::string(topology_env);
        if (topology != "replica") {
            std::cerr << "Skipping: transactions example requires a replica set" << std::endl;
            return 0;
        }
    }

    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance const inst{};

    std::function<void(mongocxx::client&)> const examples[] = {
        transactions_intro_example_1,
        transactions_retry_example_1,
        transactions_retry_example_2,
        transactions_retry_example_3,
    };

    client client{uri{}};

    {
        auto const reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
        auto const max_wire_version = reply.view()["maxWireVersion"];
        if (!max_wire_version) {
            std::cerr << "Skipping: could not determine max wire version" << std::endl;
            return EXIT_SUCCESS;
        }
        if (max_wire_version.type() != bsoncxx::type::k_int32) {
            throw std::logic_error{"max wire version is not int32"};
        }
    }

    for (auto const& example : examples) {
        /* Create necessary collections. */
        client["hr"]["employees"].drop();
        client["hr"]["employees"].insert_one(make_document(kvp("employee", 3), kvp("status", "Active")));
        client["reporting"]["events"].drop();
        client["reporting"]["events"].insert_one(make_document(kvp("employee", 3), kvp("status", "Active")));

        try {
            example(client);
        } catch (std::logic_error const& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
