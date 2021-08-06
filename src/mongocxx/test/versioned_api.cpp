// Copyright 2021 MongoDB Inc.
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

#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/server_api.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/uri.hpp>

using namespace mongocxx;

// We'll format many of these examples by hand
// clang-format off

static bool is_server_v5_or_newer() {
    using namespace bsoncxx::builder::basic;
    client cl{uri{}};
    auto status = cl["admin"].run_command(make_document(kvp("serverStatus", 1)));
    auto version = bsoncxx::string::to_string(status["version"].get_string().value);
    auto dot_pos = version.find('.');
    REQUIRE(dot_pos != version.npos);
    auto major_digit_str = version.substr(0, dot_pos);
    auto major_version = std::stoul(major_digit_str);
    return major_version >= 5;
}

TEST_CASE("Versioned API, non-strict") {
    instance::current();
    if (!is_server_v5_or_newer()) {
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

TEST_CASE("Versioned API, strict") {
    instance::current();
    if (!is_server_v5_or_newer()) {
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

TEST_CASE("Versioned API, non-strict, for commands/features outside versioned API") {
    instance::current();
    if (!is_server_v5_or_newer()) {
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

TEST_CASE("Versioned API, non-strict with deprecation errors") {
    instance::current();
    if (!is_server_v5_or_newer()) {
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

TEST_CASE("Versioned API, with insert-many for 'count' migration") {
    instance::current();
    if (!is_server_v5_or_newer()) {
        return;
    }

    using namespace mongocxx;
    mongocxx::client client{
        uri{},
        options::client{}.server_api_opts(
            options::server_api{options::server_api::version::k_version_1}.strict(true))};
    using namespace bsoncxx::builder::basic;

    // Drop in case we have stale data
    client.database("db").collection("sales").drop();

    // clang-format off
    // Start Versioned API Example 5
    auto db = client.database("db");
    auto sales = db.collection("sales");
    auto make_sale =
        [](int id, std::string item_name, int price, int quanitity, std::string time) {
            return make_document(kvp("_id", id),
                                 kvp("item", item_name),
                                 kvp("price", price),
                                 kvp("quantity", quanitity),
                                 kvp("date", time));
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
        FAIL_CHECK("Did not throw for apiStrict:true usage of old command");
    } catch (const mongocxx::operation_exception& error) {
        INFO(error.what());
        CHECK(error.code().value() == 323);
        CHECK_THAT(error.what(),
                   Catch::StartsWith(
                       "Provided apiStrict:true, but the command count is not in API Version 1."));
    }

#if 0
    /// This block not evaluated, but is inserted into documentation to represent the above "catch" block.
    /// Don't delete me!
    // clang-format off
    // Begin Versioned API Example 6
    } catch (const mongocxx::operation_exception& error) {
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
    CHECK(num_documents == 8);
    // End Versioned API Example 8
}
