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

#include <set>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/index_view.hpp>
#include <mongocxx/options/modify_collection.hpp>
#include <mongocxx/private/conversions.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/validation_criteria.hpp>

namespace {
using namespace mongocxx;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

bool check_for_collections(cursor cursor, std::set<std::string> expected_colls) {
    for (auto&& coll : cursor) {
        // Skip system collections which the MMAPv1 storage engine returns,
        // while WiredTiger does not.
        auto name_string = bsoncxx::string::to_string(coll["name"].get_utf8().value);
        auto pos = name_string.find("system.");
        if (pos != std::string::npos && pos == 0) {
            continue;
        }

        auto iter = expected_colls.find(name_string);
        if (iter == expected_colls.end()) {
            return false;
        }
        expected_colls.erase(iter);
    }
    return expected_colls.empty();
}

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

TEST_CASE("mongocxx::database copy constructor", "[database]") {
    instance::current();

    client client{uri{}};

    SECTION("constructing from valid") {
        database database_a = client["a"];
        database database_b{database_a};
        REQUIRE(database_b);
        REQUIRE(database_b.name() == stdx::string_view{"a"});
    }

    SECTION("constructing from invalid") {
        database database_a;
        database database_b{database_a};
        REQUIRE(!database_b);
    }
}

TEST_CASE("mongocxx::database copy assignment operator", "[database]") {
    instance::current();

    client client{uri{}};

    SECTION("assigning valid to valid") {
        database database_a = client["a1"];
        database database_b = client["b1"];
        database_b = database_a;
        REQUIRE(database_b);
        REQUIRE(database_b.name() == stdx::string_view{"a1"});
    }

    SECTION("assigning invalid to valid") {
        database database_a;
        database database_b = client["b2"];
        database_b = database_a;
        REQUIRE(!database_b);
    }

    SECTION("assigning valid to invalid") {
        database database_a = client["a3"];
        database database_b;
        database_b = database_a;
        REQUIRE(database_b);
        REQUIRE(database_b.name() == stdx::string_view{"a3"});
    }

    SECTION("assigning invalid to invalid") {
        database database_a;
        database database_b;
        database_b = database_a;
        REQUIRE(!database_b);
    }
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

    SECTION("throws an exception when has_collection causes an error") {
        database_has_collection->interpose(
            [](mongoc_database_t*, const char*, bson_error_t* error) {
                bson_set_error(error,
                               MONGOC_ERROR_COMMAND,
                               MONGOC_ERROR_COMMAND_INVALID_ARG,
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
        read_preference preference{};
        preference.mode(read_preference::read_mode::k_secondary_preferred);

        auto deleter = [](mongoc_read_prefs_t* var) { mongoc_read_prefs_destroy(var); };
        std::unique_ptr<mongoc_read_prefs_t, decltype(deleter)> saved_preference(nullptr, deleter);

        bool called = false;
        database_set_preference->interpose(
            [&](mongoc_database_t*, const mongoc_read_prefs_t* read_prefs) {
                called = true;
                saved_preference.reset(mongoc_read_prefs_copy(read_prefs));
                REQUIRE(mongoc_read_prefs_get_mode(read_prefs) ==
                        libmongoc::conversions::read_mode_t_from_read_mode(
                            read_preference::read_mode::k_secondary_preferred));
            });

        database_get_preference
            ->interpose([&](const mongoc_database_t*) { return saved_preference.get(); })
            .forever();

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

        bsoncxx::document::value doc = make_document(kvp("foo", 5));
        libbson::scoped_bson_t bson_doc{doc.view()};

        database_command_with_opts->interpose([&](mongoc_database_t*,
                                                  const bson_t*,
                                                  const mongoc_read_prefs_t*,
                                                  const bson_t*,
                                                  bson_t* reply,
                                                  bson_error_t*) {
            called = true;
            ::bson_copy_to(bson_doc.bson(), reply);
            return true;
        });

        database database = mongo_client[database_name];
        bsoncxx::document::value command = make_document(kvp("command", 1));
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

    auto case_insensitive_collation = make_document(kvp("locale", "en_US"), kvp("strength", 2));

    SECTION("A database may create a collection via create_collection") {
        SECTION("without any options") {
            stdx::string_view collection_name{"collection_create_no_opts"};
            database[collection_name].drop();

            collection obtained_collection = database.create_collection(collection_name);
            REQUIRE(obtained_collection.name() == collection_name);
        }

        SECTION("with options") {
            stdx::string_view collection_name{"collection_create_with_opts"};
            database[collection_name].drop();

            options::create_collection opts;
            opts.capped(true);
            opts.size(256);
            opts.max(100);
            opts.no_padding(false);

            collection obtained_collection = database.create_collection(collection_name, opts);
            REQUIRE(obtained_collection.name() == collection_name);
        }

        SECTION("with collation") {
            stdx::string_view collection_name{"collection_create_with_collation"};
            database[collection_name].drop();

            options::create_collection opts;
            opts.collation(case_insensitive_collation.view());

            if (test_util::supports_collation(mongo_client)) {
                collection obtained_collection = database.create_collection(collection_name, opts);
                REQUIRE(obtained_collection.insert_one(make_document(kvp("x", "foo"))));
                REQUIRE(obtained_collection.find_one(make_document(kvp("x", "FOO"))));
            } else {
                // The server doesn't support collation.
                REQUIRE_THROWS_AS(database.create_collection(collection_name, opts),
                                  operation_exception);
            }
        }

        SECTION("but raises exception when collection already exists") {
            stdx::string_view collection_name{"collection_create_existing"};
            database[collection_name].drop();

            database.create_collection(collection_name);

            REQUIRE_THROWS(database.create_collection(collection_name));
        }
    }

    SECTION("A collection may be modified via modify_collection") {
        SECTION("index can be modified") {
            stdx::string_view collection_name{"collection_modify_index"};
            database[collection_name].drop();
            database.create_collection(collection_name);

            auto key_pattern = make_document(kvp("a", 1));

            database[collection_name].create_index(
                key_pattern.view(), options::index{}.expire_after(std::chrono::seconds{1}));

            options::modify_collection opts;
            opts.index(key_pattern.view(), std::chrono::seconds{2});

            database.modify_collection_deprecated(collection_name, opts);

            auto cursor = database[collection_name].list_indexes();
            for (auto&& index : cursor) {
                if (index["key"].get_document().value == key_pattern.view()) {
                    auto expire_after_seconds_ele = index["expireAfterSeconds"];
                    if (expire_after_seconds_ele.type() == bsoncxx::type::k_int32) {
                        REQUIRE(expire_after_seconds_ele.get_int32().value == 2);
                    } else {
                        REQUIRE(expire_after_seconds_ele.type() == bsoncxx::type::k_int64);
                        REQUIRE(expire_after_seconds_ele.get_int64().value == 2);
                    }
                }
            }
        }

        SECTION("validation_criteria can be modified") {
            stdx::string_view collection_name{"collection_modify_validation"};
            database[collection_name].drop();
            database.create_collection(collection_name);

            auto rule = make_document(kvp("email", make_document(kvp("$exists", "true"))));

            validation_criteria criteria;
            criteria.rule(rule.view());

            options::modify_collection opts;
            opts.validation_criteria(criteria);

            if (test_util::get_max_wire_version(mongo_client) >= 4) {
                // The server supports document validation.
                REQUIRE_NOTHROW(database.modify_collection_deprecated(collection_name, opts));

                auto cursor = database.list_collections();
                for (auto&& coll : cursor) {
                    if (coll["name"].get_utf8().value == collection_name) {
                        REQUIRE(coll["options"]["validator"].get_document().value == rule);
                    }
                }
            } else {
                // The server does not support document validation.
                REQUIRE_THROWS_AS(database.modify_collection_deprecated(collection_name, opts),
                                  operation_exception);
            }
        }
    }

    SECTION("A database may be dropped") {
        stdx::string_view collection_name{"collection_drop"};
        database[collection_name].drop();

        database.create_collection(collection_name);
        REQUIRE(database.has_collection(collection_name));
        database.drop();
        REQUIRE(!database.has_collection(collection_name));
    }

    SECTION("read_concern is inherited from parent", "[database]") {
        stdx::string_view collection_name{"collection_read_concern"};

        read_concern::level majority = read_concern::level::k_majority;
        read_concern::level local = read_concern::level::k_local;

        read_concern rc{};
        rc.acknowledge_level(majority);
        mongo_client.read_concern_deprecated(rc);

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

    SECTION("A database may create a view via create_view") {
        SECTION("View creation with a pipeline") {
            stdx::string_view collection_name{"collection_view_with_pipeline"};
            stdx::string_view view_name{"view"};
            database[collection_name].drop();
            database[view_name].drop();

            database[collection_name].insert_one(make_document(kvp("x", "foo")));
            database[collection_name].insert_one(make_document(kvp("x", "bar")));

            collection view = database.create_view(
                view_name,
                collection_name,
                options::create_view().pipeline(std::move(pipeline({}).limit(1))));

            if (test_util::get_max_wire_version(mongo_client) >= 5) {
                // The server supports views.
                REQUIRE(view.count_documents(bsoncxx::document::view{}) == 1);
            } else {
                // The server doesn't support views. On these versions of the server, view creation
                // requests are treated as ordinary collection creation requests.
                REQUIRE(view.count_documents(bsoncxx::document::view{}) == 0);
            }
        }

        SECTION("View creation with collation") {
            stdx::string_view collection_name{"collection_view_with_collation"};
            stdx::string_view view_name{"view"};
            database[collection_name].drop();
            database[view_name].drop();

            database[collection_name].insert_one(make_document(kvp("x", "foo")));
            database[collection_name].insert_one(make_document(kvp("x", "bar")));

            options::create_view opts;
            opts.collation(case_insensitive_collation.view());

            if (test_util::supports_collation(mongo_client)) {
                collection obtained_view = database.create_view(view_name, collection_name, opts);
                REQUIRE(obtained_view.find_one(make_document(kvp("x", "FOO"))));
            } else {
                // The server doesn't support collation.
                REQUIRE_THROWS_AS(database.create_view(view_name, collection_name, opts),
                                  operation_exception);
            }
        }
    }

    SECTION("list_collections returns a correct result") {
        class database db = mongo_client["list_collections"];
        db.drop();

        options::create_collection opts;
        opts.capped(true);
        opts.size(256);

        collection default_collection = db.create_collection("list_collections_default");
        collection capped_collection = db.create_collection("list_collections_capped", opts);

        index_view default_index = default_collection.indexes();
        index_view capped_index = capped_collection.indexes();

        default_index.create_one(make_document(kvp("a", 1)));
        capped_index.create_one(make_document(kvp("a", 1)));

        default_collection.insert_one(make_document(kvp("a", 5)));
        capped_collection.insert_one(make_document(kvp("a", 5)));

        auto cursor1 = db.list_collections();
        std::set<std::string> expected_colls1{"list_collections_default",
                                              "list_collections_capped"};
        REQUIRE(check_for_collections(std::move(cursor1), expected_colls1));

        auto cursor2 = db.list_collections(make_document(kvp("options.capped", true)));
        std::set<std::string> expected_colls2{"list_collections_capped"};
        REQUIRE(check_for_collections(std::move(cursor2), expected_colls2));
    }

    SECTION("list_collection_names returns a correct result") {
        class database db = mongo_client["list_collection_names"];
        db.drop();

        std::set<std::string> expected_colls;
        for (std::size_t i = 0; i < 10; ++i) {
            std::string coll_str = "list_collection_names_coll" + std::to_string(i);
            db.create_collection(coll_str);
            expected_colls.insert(coll_str);
        }

        auto names = db.list_collection_names();
        for (auto const& name : names) {
            REQUIRE(expected_colls.erase(name) == 1);
        }

        REQUIRE(expected_colls.size() == 0);
    }
}

}  // namespace
