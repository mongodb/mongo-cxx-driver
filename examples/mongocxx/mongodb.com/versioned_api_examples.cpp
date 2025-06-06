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

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/string/to_string.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/server_api.hpp>
#include <mongocxx/uri.hpp>

#include <examples/macros.hh>

namespace {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace mongocxx;

// API Version 1 was introduced in 5.0.
bool has_api_version_1() {
    mongocxx::client client{mongocxx::uri{}};

    auto const reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
    auto const max_wire_version = reply.view()["maxWireVersion"];

    if (!max_wire_version) {
        return false;
    }

    if (max_wire_version.type() != bsoncxx::type::k_int32) {
        throw std::logic_error{"max wire version is not int32"};
    }

    if (max_wire_version.get_int32().value < 13) {
        return false;
    }

    return true;
}

bool has_api_version_1_with_count() {
    if (!has_api_version_1()) {
        return false;
    }

    mongocxx::client client{mongocxx::uri{}};

    auto const status = client["admin"].run_command(make_document(kvp("serverStatus", 1)));
    auto const version = bsoncxx::string::to_string(status.view()["version"].get_string().value);

    // mongocxx::test_util::compare_versions
    auto compare_versions = [](std::string version1, std::string version2) -> std::int32_t {
        // mongocxx::test_util::parse_version
        auto const parse_version = [](std::string version) -> std::vector<std::int32_t> {
            std::vector<std::int32_t> elements;
            std::stringstream ss{version};
            std::string element;

            while (std::getline(ss, element, '.')) {
                elements.push_back(std::stoi(element));
            }

            return elements;
        };

        std::vector<std::int32_t> v1 = parse_version(version1);
        std::vector<std::int32_t> v2 = parse_version(version2);

        for (std::size_t i = 0; i < std::min(v1.size(), v2.size()); ++i) {
            std::int32_t difference = v1[i] - v2[i];

            if (difference != 0) {
                return difference;
            }
        }

        return 0;
    };

    // BACKPORT-12171: count command was backported to 5.0.9.
    if (compare_versions(version, "5.0") == 0 && compare_versions(version, "5.0.9") >= 0) {
        return true;
    }

    // BACKPORT-12170: count command was backported to 5.3.2.
    if (compare_versions(version, "5.3") == 0 && compare_versions(version, "5.3.2") >= 0) {
        return true;
    }

    // mongocxx::test_util::get_server_version
    auto const get_server_version = [&status]() {
        return bsoncxx::string::to_string(status.view()["version"].get_string().value);
    };

    // SERVER-63850: count command was added in 6.0.
    return (compare_versions(get_server_version(), version) >= 0);
}

// We'll format many of these examples by hand
// clang-format off

void versioned_api_example_1() {
    if (!has_api_version_1()) {
        std::cerr << "Skipping: Versioned API Example 1 requires MongoDB 5.0 or newer" << std::endl;
        return;
    }

    // Start Versioned API Example 1
    using namespace mongocxx;
    uri client_uri{"mongodb://localhost"};
    // Create an option set for API v1
    const auto server_api_opts =
        options::server_api{options::server_api::version_from_string("1")};
    // Store it in the set of client options
    const auto client_opts =
        options::client{}
            .server_api_opts(server_api_opts); // Set the version
    // Create a new client with the options
    mongocxx::client client{client_uri, client_opts};
    // End Versioned API Example 1
}

void versioned_api_example_2() {
    if (!has_api_version_1()) {
        std::cerr << "Skipping: Versioned API Example 2 requires MongoDB 5.0 or newer" << std::endl;
        return;
    }

    // Start Versioned API Example 2
    using namespace mongocxx;
    uri client_uri{"mongodb://localhost"};
    // Create an option set for API v1
    const auto server_api_opts =
        options::server_api{options::server_api::version_from_string("1")}
            .strict(true);  // Enable strict mode for the server API
    // Store it in the set of client options
    const auto client_opts =
        options::client{}
            .server_api_opts(server_api_opts); // Set the version and options
    // Create a new client with the options
    mongocxx::client client{client_uri, client_opts};
    // End Versioned API Example 2
}

void versioned_api_example_3() {
    if (!has_api_version_1()) {
        std::cerr << "Skipping: Versioned API Example 3 requires MongoDB 5.0 or newer" << std::endl;
        return;
    }

    // Start Versioned API Example 3
    using namespace mongocxx;
    uri client_uri{"mongodb://localhost"};
    // Create an option set for API v1
    const auto server_api_opts =
        options::server_api{options::server_api::version_from_string("1")}
            .strict(false);  // Explicitly disable strict mode for the server API
    // Store it in the set of client options
    const auto client_opts =
        options::client{}
            .server_api_opts(server_api_opts); // Set the version and options
    // Create a new client with the options
    mongocxx::client client{client_uri, client_opts};
    // End Versioned API Example 3
}

void versioned_api_example_4() {
    if (!has_api_version_1()) {
        std::cerr << "Skipping: Versioned API Example 4 requires MongoDB 5.0 or newer" << std::endl;
        return;
    }

    // Start Versioned API Example 4
    using namespace mongocxx;
    uri client_uri{"mongodb://localhost"};
    // Create an option set for API v1
    const auto server_api_opts =
        options::server_api{options::server_api::version_from_string("1")}
            .deprecation_errors(true);  // Enable deprecation errors
    // Store it in the set of client options
    const auto client_opts =
        options::client{}
            .server_api_opts(server_api_opts); // Set the version and options
    // Create a new client with the options
    mongocxx::client client{client_uri, client_opts};
    // End Versioned API Example 4
}
// clang-format on

/// Not actually a version function. Just used to appear in the documentation examples "as if" we
/// were creating a date from a timestamp string
bsoncxx::types::b_date iso_string_to_bson_datetime(std::string const&) {
    return bsoncxx::types::b_date(std::chrono::milliseconds{0});
}

void versioned_api_example_5() {
    if (!has_api_version_1()) {
        std::cerr << "Skipping: Versioned API Example 5 requires MongoDB 5.0 or newer" << std::endl;
        return;
    }

    // Do *not* run this test if count command is included in API Version 1.
    if (has_api_version_1_with_count()) {
        std::cerr << "Skipping: Versioned API Example 5 requires the 'count' command is NOT supported" << std::endl;
        return;
    }

    using namespace mongocxx;
    mongocxx::client client{
        uri{},
        options::client{}.server_api_opts(options::server_api{options::server_api::version::k_version_1}.strict(true))};
    using namespace bsoncxx::builder::basic;

    // Drop in case we have stale data
    client.database("db").collection("sales").drop();

    // clang-format off
    // Start Versioned API Example 5
    auto db = client.database("db");
    auto sales = db.collection("sales");
    auto make_sale =
        [](int id, std::string item_name, int price, int quanitity, std::string time) {
            bsoncxx::types::b_date bson_datetime = iso_string_to_bson_datetime(time);
            return make_document(kvp("_id", id),
                                 kvp("item", item_name),
                                 kvp("price", price),
                                 kvp("quantity", quanitity),
                                 kvp("date", bson_datetime));
        };
    auto items = {
        //       _id  item    price  qty  date
        make_sale(1,  "abc",  10,    2,   "2021-01-01T08:00:00Z"),
        make_sale(2,  "jkl",  20,    1,   "2021-02-03T09:00:00Z"),
        make_sale(3,  "xyz",  5,     5,   "2021-02-03T09:05:00Z"),
        make_sale(4,  "abc",  10,    10,  "2021-02-15T08:00:00Z"),
        make_sale(5,  "xyz",  5,     10,  "2021-02-15T09:05:00Z"),
        make_sale(6,  "xyz",  5,     5,   "2021-02-15T12:05:10Z"),
        make_sale(7,  "xyz",  5,     10,  "2021-02-15T14:12:12Z"),
        make_sale(8,  "abc",  10,    5,   "2021-03-16T20:20:13Z"),
    };
    sales.insert_many(items);
    // End Versioned API Example 5
    // clang-format on

    try {
        db.run_command(make_document(kvp("count", "sales")));
        throw std::logic_error{"Did not throw for apiStrict:true usage of old command"};
    } catch (mongocxx::operation_exception const& error) {
        auto const what = std::string(error.what());
        auto const value = error.code().value();

        std::cout << what << std::endl;

        if (value != 323) {
            throw std::logic_error{"expected error code to be 323, got: " + std::to_string(value)};
        }

        if (what.find("Provided apiStrict:true, but the command count is not in API Version 1.") != 0) {
            throw std::logic_error{"expected error message to start with 'Provided apiStrict:true'"};
        }
    }

#if 0
    /// This block not evaluated, but is inserted into documentation to represent the above "catch" block.
    /// Don't delete me!
    // clang-format off
    // Begin Versioned API Example 6
    } catch (mongocxx::operation_exception const& error) {
        cerr << error.what();
        // Prints:
        //    Provided apiStrict:true, but the command count is not in API Version 1.
    }
    // End Versioned API Example 6
    // clang-format on
#endif

    // Start Versioned API Example 7
    std::int64_t num_documents = sales.count_documents(make_document());
    // End Versioned API Example 7

    // Start Versioned API Example 8
    if (num_documents != 8) {
        throw std::logic_error{"expected the number of documents to equal 8"};
    }
    // End Versioned API Example 8
}

} // namespace

int EXAMPLES_CDECL main() {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance const inst{};

    try {
        versioned_api_example_1();
        versioned_api_example_2();
        versioned_api_example_3();
        versioned_api_example_4();
        versioned_api_example_5();
    } catch (std::logic_error const& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
