// Copyright 2014 MongoDB Inc.
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

#include "helpers.hpp"

#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/private/conversions.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/uri.hpp>

namespace {
using namespace mongocxx;

TEST_CASE("A default constructed client is false-ish", "[client]") {
    MOCK_CLIENT

    instance::current();

    client a;
    REQUIRE(!a);
}

TEST_CASE("A default constructed client cannot perform operations", "[client]") {
    instance::current();

    client a;
    REQUIRE_THROWS_AS(a.list_databases(), mongocxx::logic_error);
}

TEST_CASE("A client constructed with a URI is truthy", "[client]") {
    MOCK_CLIENT

    instance::current();

    client a{uri{}};
    REQUIRE(a);
}

TEST_CASE("A client connects to a provided mongodb uri", "[client]") {
    MOCK_CLIENT

    instance::current();

    std::string expected_url("mongodb://mongodb.example.com:9999");
    uri mongodb_uri(expected_url);
    std::string actual_url{};

    client_new->visit(
        [&](const mongoc_uri_t* url) { actual_url = std::string(mongoc_uri_get_string(url)); });

    client a{mongodb_uri};

    REQUIRE(a);
    REQUIRE(expected_url == actual_url);
}

TEST_CASE("A client throws if its underlying mongoc client is NULL", "[client]") {
    MOCK_CLIENT

    instance::current();

    client_new->interpose([](const mongoc_uri_t*) { return (mongoc_client_t*)nullptr; });

    REQUIRE_THROWS_AS(client{uri{}}, mongocxx::exception);
}

TEST_CASE("A client cleans up its underlying mongoc client on destruction", "[client]") {
    MOCK_CLIENT

    instance::current();

    bool destroy_called = false;
    client_destroy->visit([&](mongoc_client_t*) { destroy_called = true; });

    {
        client object{uri{}};
        REQUIRE(!destroy_called);
    }

    REQUIRE(destroy_called);
}

TEST_CASE("A client supports move operations", "[client]") {
    MOCK_CLIENT

    instance::current();

    client a{uri{}};

    bool called = false;
    client_new->visit([&](const mongoc_uri_t*) { called = true; });

    client b{std::move(a)};
    REQUIRE(!called);

    client c = std::move(b);
    REQUIRE(!called);
}

TEST_CASE("A client has a settable Read Concern", "[client]") {
    MOCK_CLIENT

    instance::current();

    client mongo_client{uri{}};

    auto client_set_rc_called = false;
    read_concern rc{};
    rc.acknowledge_level(read_concern::level::k_majority);

    client_set_read_concern->interpose(
        [&client_set_rc_called](::mongoc_client_t*, const ::mongoc_read_concern_t* rc_t) {
            REQUIRE(rc_t);
            const auto result = libmongoc::read_concern_get_level(rc_t);
            REQUIRE(result);
            REQUIRE(strcmp(result, "majority") == 0);
            client_set_rc_called = true;
        });

    mongo_client.read_concern_deprecated(rc);
    REQUIRE(client_set_rc_called);
}

TEST_CASE("A client's read preferences may be set and obtained", "[client]") {
    MOCK_CLIENT

    instance::current();

    client mongo_client{uri{}};
    read_preference preference{};
    preference.mode(read_preference::read_mode::k_secondary_preferred);

    bool called_set = false;
    auto deleter = [](mongoc_read_prefs_t* var) { mongoc_read_prefs_destroy(var); };
    std::unique_ptr<mongoc_read_prefs_t, decltype(deleter)> saved_preference(nullptr, deleter);

    client_set_preference->interpose([&](mongoc_client_t*, const mongoc_read_prefs_t* read_prefs) {
        called_set = true;
        saved_preference.reset(mongoc_read_prefs_copy(read_prefs));
        REQUIRE(mongoc_read_prefs_get_mode(read_prefs) ==
                libmongoc::conversions::read_mode_t_from_read_mode(
                    read_preference::read_mode::k_secondary_preferred));
    });

    client_get_preference->interpose([&](const mongoc_client_t*) { return saved_preference.get(); })
        .forever();

    mongo_client.read_preference_deprecated(std::move(preference));
    REQUIRE(called_set);

    REQUIRE(read_preference::read_mode::k_secondary_preferred ==
            mongo_client.read_preference().mode());
}

TEST_CASE("A client's write concern may be set and obtained", "[client]") {
    MOCK_CLIENT

    instance::current();

    client mongo_client{uri{}};
    write_concern concern;
    concern.majority(std::chrono::milliseconds(100));

    mongoc_write_concern_t* underlying_wc;

    bool set_called = false;
    client_set_concern->interpose([&](mongoc_client_t*, const mongoc_write_concern_t* concern) {
        set_called = true;
        underlying_wc = mongoc_write_concern_copy(concern);
    });

    bool get_called = false;
    client_get_concern->interpose([&](const mongoc_client_t*) {
        get_called = true;
        return underlying_wc;
    });

    mongo_client.write_concern_deprecated(concern);
    REQUIRE(set_called);

    MOCK_CONCERN
    bool copy_called = false;
    concern_copy->interpose([&](const mongoc_write_concern_t*) {
        copy_called = true;
        return mongoc_write_concern_copy(underlying_wc);
    });

    REQUIRE(concern.majority() == mongo_client.write_concern().majority());

    REQUIRE(get_called);
    REQUIRE(copy_called);

    libmongoc::write_concern_destroy(underlying_wc);
}

TEST_CASE("A client can create a named database object", "[client]") {
    MOCK_CLIENT

    instance::current();

    auto database_get = libmongoc::client_get_database.create_instance();
    database_get->interpose([](mongoc_client_t*, const char*) { return nullptr; }).forever();
    auto database_destroy = libmongoc::database_destroy.create_instance();
    database_destroy->interpose([](mongoc_database_t*) {}).forever();
    auto database_set_preference = libmongoc::database_set_read_prefs.create_instance();
    database_set_preference->interpose([](mongoc_database_t*, const mongoc_read_prefs_t*) {})
        .forever();
    auto database_set_concern = libmongoc::database_set_write_concern.create_instance();
    database_set_concern->interpose([](mongoc_database_t*, const mongoc_write_concern_t*) {})
        .forever();

    stdx::string_view name("database");

    client mongo_client{uri{}};
    database obtained_database = mongo_client[name];
    REQUIRE(obtained_database.name() == name);
}

TEST_CASE("integration tests for client metadata handshake feature") {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    std::string app_name{"xyz"};
    uri uri{"mongodb://localhost/?appName=" + app_name};
    instance::current();

    auto run_test = [app_name](const client& client) {
        mongocxx::database db = client["admin"];
        auto current_op = db.run_command(make_document(kvp("currentOp", 1)));
        auto current_op_view = current_op.view();

        auto in_prog = current_op_view["inprog"].get_array().value;
        bool found_op = false;

        for (auto&& it : in_prog) {
            auto op_view = it.get_document().view();

            if (!op_view["appName"] ||
                op_view["appName"].get_utf8().value != stdx::string_view(app_name)) {
                continue;
            }

            found_op = true;

            std::string server_version = test_util::get_server_version(client);

            // clientMetadata not returned until 3.5.8.
            if (test_util::compare_versions(server_version, "3.5.8") >= 0) {
                REQUIRE(op_view["clientMetadata"]);
                auto metadata = op_view["clientMetadata"].get_document();
                auto metadata_view = metadata.view();

                REQUIRE(metadata_view["application"]);
                auto application = metadata_view["application"].get_document();
                REQUIRE(application.view()["name"].get_utf8().value == stdx::string_view(app_name));

                REQUIRE(metadata_view["driver"]);
                auto driver = metadata_view["driver"].get_document();
                auto driver_view = driver.view();
                REQUIRE(driver_view["name"].get_utf8().value ==
                        stdx::string_view{"mongoc / mongocxx"});
                auto version = bsoncxx::string::to_string(driver_view["version"].get_utf8().value);
                REQUIRE(version.find(MONGOCXX_VERSION_STRING) != std::string::npos);

                REQUIRE(metadata_view["os"]);
                REQUIRE(metadata_view["os"].get_document().view()["type"]);
            }

            break;
        }
        REQUIRE(found_op);
    };

    SECTION("with client") {
        mongocxx::client client{uri};
        run_test(client);
    }

    SECTION("with pool") {
        mongocxx::pool pool{uri};
        auto client = pool.acquire();
        run_test(*client);
    }
}
}  // namespace
