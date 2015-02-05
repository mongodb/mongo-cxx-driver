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

#include "catch.hpp"
#include "helpers.hpp"

#include <mongo/driver/private/libmongoc.hpp>

#include <mongo/driver/client.hpp>
#include <mongo/driver/database.hpp>

using namespace mongo;
using namespace mongo::driver;

TEST_CASE("A database", "[database]") {
    const std::string database_name("database");
    MOCK_CLIENT
    MOCK_DATABASE
    client mongo_client;

    SECTION("is created by a client") {
        bool called = false;
        get_database->interpose([&](mongoc_client_t* client, const char* c_name) {
            called = true;
            REQUIRE(database_name == c_name);
            return nullptr;
        });

        database obtained_database = mongo_client[database_name];
        REQUIRE(called);
        REQUIRE(obtained_database.name() == database_name);
    }
    SECTION("cleans up its underlying mongoc database on destruction") {
        bool destroy_called = false;

        database_destroy->interpose([&](mongoc_database_t* client) { destroy_called = true; });

        {
            database database = mongo_client["database"];
            REQUIRE(!destroy_called);
        }

        REQUIRE(destroy_called);
    }

    SECTION("supports move operations") {
        bool destroy_called = false;
        database_destroy->interpose([&](mongoc_database_t* client) { destroy_called = true; });

        {
            client mongo_client;
            database a = mongo_client[database_name];

            database b{std::move(a)};
            REQUIRE(!destroy_called);

            database c = std::move(b);
            REQUIRE(!destroy_called);
        }
        REQUIRE(destroy_called);
    }

    SECTION("has a read preferences which may be set and obtained") {
        bool destroy_called = false;
        database_destroy->interpose([&](mongoc_database_t* client) { destroy_called = true; });

        database mongo_database(mongo_client["database"]);
        read_preference preference{read_preference::read_mode::k_secondary_preferred};

        auto deleter = [](mongoc_read_prefs_t* var) { mongoc_read_prefs_destroy(var); };
        std::unique_ptr<mongoc_read_prefs_t, decltype(deleter)> saved_preference(nullptr, deleter);

        bool called = false;
        database_set_preference->interpose([&](mongoc_database_t* db,
                                               const mongoc_read_prefs_t* read_prefs) {
            called = true;
            saved_preference.reset(mongoc_read_prefs_copy(read_prefs));
            REQUIRE(
                mongoc_read_prefs_get_mode(read_prefs) ==
                static_cast<mongoc_read_mode_t>(read_preference::read_mode::k_secondary_preferred));
        });

        database_get_preference->interpose([&](const mongoc_database_t* client) {
                                               return saved_preference.get();
                                           }).forever();

        mongo_database.read_preference(std::move(preference));
        REQUIRE(called);

        REQUIRE(read_preference::read_mode::k_secondary_preferred ==
                mongo_database.read_preference().mode());
    }

    SECTION("has a write concern which may be set and obtained") {
        bool destroy_called = false;
        database_destroy->interpose([&](mongoc_database_t* client) { destroy_called = true; });

        database mongo_database(mongo_client[database_name]);
        write_concern concern;
        concern.majority(std::chrono::milliseconds(100));

        mongoc_write_concern_t* underlying_wc;

        bool set_called = false;
        database_set_concern->interpose(
            [&](mongoc_database_t* client, const mongoc_write_concern_t* concern) {
                set_called = true;
                underlying_wc = mongoc_write_concern_copy(concern);
            });

        bool get_called = false;
        database_get_concern->interpose([&](const mongoc_database_t* client) {
            get_called = true;
            return underlying_wc;
        });

        mongo_database.write_concern(concern);
        REQUIRE(set_called);

        MOCK_CONCERN
        bool copy_called = false;
        concern_copy->interpose([&](const mongoc_write_concern_t* concern) {
            copy_called = true;
            return mongoc_write_concern_copy(underlying_wc);
        });

        REQUIRE(concern.majority() == mongo_database.write_concern().majority());

        REQUIRE(get_called);
        REQUIRE(copy_called);
    }

    SECTION("may create a collection") {
        MOCK_COLLECTION
        const std::string collection_name("collection");
        database database = mongo_client[database_name];
        collection obtained_collection = database[collection_name];
        REQUIRE(obtained_collection.name() == collection_name);
    }
}
