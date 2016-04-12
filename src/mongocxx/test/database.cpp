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

#include <mongocxx/database.hpp>

#include <bsoncxx/builder/stream/helpers.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/modify_collection.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/validation_criteria.hpp>

using namespace mongocxx;

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

TEST_CASE("A default constructed database is false-ish", "[database]") {
    instance::current();

    database d;
    REQUIRE(!d);
}

TEST_CASE("A default constructed database cannot perform operations", "[database]") {
    instance::current();

    database d;
    REQUIRE_THROWS_AS(d.name(), mongocxx::logic_error);
}

TEST_CASE("database copy", "[database]") {
    instance::current();

    client mongodb_client{uri{}};

    std::string dbname{"foo"};
    std::string dbname2{"bar"};
    database db = mongodb_client[dbname];

    database db2{db};
    database db3 = mongodb_client[dbname2];
    db3 = db;

    REQUIRE(db2.name() == stdx::string_view{dbname});
    REQUIRE(db3.name() == stdx::string_view{dbname});
}

TEST_CASE("A database", "[database]") {
    stdx::string_view database_name{"database"};
    MOCK_CLIENT
    MOCK_DATABASE

    instance::current();

    client mongo_client{uri{}};

    SECTION("is created by a client") {
        bool called = false;
        get_database->interpose([&](mongoc_client_t*, const char* d_name) {
            called = true;
            REQUIRE(database_name == stdx::string_view{d_name});
            return nullptr;
        });

        database obtained_database = mongo_client[database_name];
        REQUIRE(obtained_database);
        REQUIRE(called);
        REQUIRE(obtained_database.name() == database_name);
    }

    SECTION("cleans up its underlying mongoc database on destruction") {
        bool destroy_called = false;

        database_destroy->interpose([&](mongoc_database_t*) { destroy_called = true; });

        {
            database database = mongo_client["database"];
            REQUIRE(!destroy_called);
        }

        REQUIRE(destroy_called);
    }

    SECTION("is dropped") {
        bool drop_called = false;

        database_drop->interpose([&](mongoc_database_t*, bson_error_t*) {
            drop_called = true;
            return true;
        });

        database database = mongo_client["database"];
        REQUIRE(!drop_called);
        database.drop();
        REQUIRE(drop_called);
    }

    SECTION("throws an exception when dropping causes an error") {
        database_drop->interpose([&](mongoc_database_t*, bson_error_t* error) {
            bson_set_error(error, MONGOC_ERROR_COMMAND, MONGOC_ERROR_COMMAND_INVALID_ARG,
                           "expected error from mock");
            return false;
        });

        database database = mongo_client["database"];
        REQUIRE_THROWS(database.drop());
    }

    SECTION("throws an exception when has_collection causes an error") {
        database_has_collection->interpose(
            [](mongoc_database_t*, const char*, bson_error_t* error) {
                bson_set_error(error, MONGOC_ERROR_COMMAND, MONGOC_ERROR_COMMAND_INVALID_ARG,
                               "expected error from mock");
                return false;
            });
        database database = mongo_client["database"];
        REQUIRE_THROWS(database.has_collection("some_collection"));
    }

    SECTION("supports move operations") {
        bool destroy_called = false;
        database_destroy->interpose([&](mongoc_database_t*) { destroy_called = true; });

        {
            client mongo_client{uri{}};
            database a = mongo_client[database_name];

            database b{std::move(a)};
            REQUIRE(!destroy_called);

            database c = std::move(b);
            REQUIRE(!destroy_called);
        }
        REQUIRE(destroy_called);
    }

    SECTION("Read Concern", "[database]") {
        database mongo_database(mongo_client["database"]);

        auto database_set_rc_called = false;
        read_concern rc{};
        rc.acknowledge_level(read_concern::level::k_majority);

        database_set_read_concern->interpose(
            [&database_set_rc_called](::mongoc_database_t*, const ::mongoc_read_concern_t* rc_t) {
                REQUIRE(rc_t);
                const auto result = libmongoc::read_concern_get_level(rc_t);
                REQUIRE(result);
                REQUIRE(strcmp(result, "majority") == 0);
                database_set_rc_called = true;
            });

        mongo_database.read_concern(rc);
        REQUIRE(database_set_rc_called);
    }

    SECTION("has a read preferences which may be set and obtained") {
        bool destroy_called = false;
        database_destroy->interpose([&](mongoc_database_t*) { destroy_called = true; });

        database mongo_database(mongo_client["database"]);
        read_preference preference{read_preference::read_mode::k_secondary_preferred};

        auto deleter = [](mongoc_read_prefs_t* var) { mongoc_read_prefs_destroy(var); };
        std::unique_ptr<mongoc_read_prefs_t, decltype(deleter)> saved_preference(nullptr, deleter);

        bool called = false;
        database_set_preference->interpose([&](mongoc_database_t*,
                                               const mongoc_read_prefs_t* read_prefs) {
            called = true;
            saved_preference.reset(mongoc_read_prefs_copy(read_prefs));
            REQUIRE(
                mongoc_read_prefs_get_mode(read_prefs) ==
                static_cast<mongoc_read_mode_t>(read_preference::read_mode::k_secondary_preferred));
        });

        database_get_preference->interpose([&](const mongoc_database_t*) {
            return saved_preference.get();
        }).forever();

        mongo_database.read_preference(std::move(preference));
        REQUIRE(called);

        REQUIRE(read_preference::read_mode::k_secondary_preferred ==
                mongo_database.read_preference().mode());
    }

    SECTION("has a write concern which may be set and obtained") {
        bool destroy_called = false;
        database_destroy->interpose([&](mongoc_database_t*) { destroy_called = true; });

        database mongo_database(mongo_client[database_name]);
        write_concern concern;
        concern.majority(std::chrono::milliseconds(100));

        mongoc_write_concern_t* underlying_wc;

        bool set_called = false;
        database_set_concern->interpose(
            [&](mongoc_database_t*, const mongoc_write_concern_t* concern) {
                set_called = true;
                underlying_wc = mongoc_write_concern_copy(concern);
            });

        bool get_called = false;
        database_get_concern->interpose([&](const mongoc_database_t*) {
            get_called = true;
            return underlying_wc;
        });

        mongo_database.write_concern(concern);
        REQUIRE(set_called);

        MOCK_CONCERN
        bool copy_called = false;
        concern_copy->interpose([&](const mongoc_write_concern_t*) {
            copy_called = true;
            return mongoc_write_concern_copy(underlying_wc);
        });

        REQUIRE(concern.majority() == mongo_database.write_concern().majority());

        REQUIRE(get_called);
        REQUIRE(copy_called);

        libmongoc::write_concern_destroy(underlying_wc);
    }

    SECTION("may create a collection") {
        MOCK_COLLECTION
        stdx::string_view collection_name{"dummy_collection"};
        database database = mongo_client[database_name];
        collection obtained_collection = database[collection_name];
        REQUIRE(obtained_collection.name() == collection_name);
    }

    SECTION("supports run_command") {
        bool called = false;

        bsoncxx::document::value doc = bsoncxx::builder::stream::document{}
                                       << "foo" << 5 << bsoncxx::builder::stream::finalize;
        libbson::scoped_bson_t bson_doc{doc.view()};

        database_command_simple->interpose([&](mongoc_database_t*, const bson_t*,
                                               const mongoc_read_prefs_t*, bson_t* reply,
                                               bson_error_t*) {
            called = true;
            ::bson_copy_to(bson_doc.bson(), reply);
            return true;
        });

        database database = mongo_client[database_name];
        bsoncxx::document::value command = bsoncxx::builder::stream::document{}
                                           << "command" << 1 << bsoncxx::builder::stream::finalize;
        auto response = database.run_command(command.view());
        REQUIRE(called);
        REQUIRE(response.view()["foo"].get_int32() == 5);
    }
}

TEST_CASE("Database integration tests", "[database]") {
    instance::current();

    client mongo_client{uri{}};
    stdx::string_view database_name{"database"};
    database database = mongo_client[database_name];
    stdx::string_view collection_name{"collection"};

    SECTION("A database may create a collection via create_collection") {
        SECTION("without any options") {
            database[collection_name].drop();

            collection obtained_collection = database.create_collection(collection_name);
            REQUIRE(obtained_collection.name() == collection_name);
        }

        SECTION("with options") {
            database[collection_name].drop();

            options::create_collection opts;
            opts.capped(true);
            opts.size(256);
            opts.max(100);
            opts.no_padding(false);

            collection obtained_collection = database.create_collection(collection_name, opts);
            REQUIRE(obtained_collection.name() == collection_name);
        }

        SECTION("but raises exception when collection already exists") {
            database[collection_name].drop();

            database.create_collection(collection_name);

            REQUIRE_THROWS(database.create_collection(collection_name));
        }

        database[collection_name].drop();
    }

    SECTION("A collection may be modified via modify_collection") {
        database.create_collection(collection_name);

        auto rule = document{} << "email" << open_document << "$exists"
                               << "true" << close_document << finalize;

        validation_criteria criteria;
        criteria.rule(rule.view());

        options::modify_collection opts;
        opts.validation_criteria(criteria);

        auto res = database.modify_collection(collection_name, opts);

        auto cursor = database.list_collections();
        for (auto&& coll : cursor) {
            if (coll["name"].get_utf8().value == collection_name) {
                REQUIRE(coll["options"]["validator"].get_document().value == rule);
            }
        }
    }

    SECTION("A database may be dropped") {
        database[collection_name].drop();

        database.create_collection(collection_name);
        REQUIRE(database.has_collection(collection_name));
        database.drop();
        REQUIRE(!database.has_collection(collection_name));
    }

    SECTION("read_concern is inherited from parent", "[database]") {
        read_concern::level majority = read_concern::level::k_majority;
        read_concern::level local = read_concern::level::k_local;

        read_concern rc{};
        rc.acknowledge_level(majority);
        mongo_client.read_concern(rc);

        mongocxx::database rc_db = mongo_client[database_name];

        SECTION("when parent is a client") {
            REQUIRE(rc_db.read_concern().acknowledge_level() == majority);
        }

        SECTION("except when read_concern is explicitly set") {
            read_concern set_rc{};
            set_rc.acknowledge_level(read_concern::level::k_local);
            rc_db.read_concern(set_rc);

            REQUIRE(rc_db.read_concern().acknowledge_level() == local);
        }
    }
}
