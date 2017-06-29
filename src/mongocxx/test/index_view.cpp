// Copyright 2017 MongoDB Inc.
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

#include <chrono>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/index_view.hpp>
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;
using namespace mongocxx;

bool test_commands_enabled(const client& conn) {
    auto result = conn["admin"].run_command(
        make_document(kvp("getParameter", 1), kvp("enableTestCommands", 1)));
    auto result_view = result.view();

    if (!result_view["enableTestCommands"]) {
        return false;
    }

    auto server_version = test_util::get_server_version(conn);

    if (test_util::compare_versions(server_version, "3.2") >= 0) {
        return result_view["enableTestCommands"].get_bool();
    }

    return result_view["enableTestCommands"].get_int32() == 1;
}

bool fail_with_max_timeout(const client& conn) {
    if (!test_commands_enabled(conn)) {
        return false;
    }

    conn["admin"].run_command(
        make_document(kvp("configureFailPoint", "maxTimeAlwaysTimeOut"), kvp("mode", "alwaysOn")));

    return true;
}

TEST_CASE("create_one", "[index_view]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];
    collection coll = db["index_view_create_one"];
    coll.drop();
    coll.insert_one({});  // Ensure that the collection exists.
    index_view indexes = coll.indexes();

    SECTION("works with document and options") {
        auto key = make_document(kvp("a", 1));
        auto options = make_document(kvp("name", "myIndex"));
        stdx::optional<std::string> result = indexes.create_one(key.view(), options.view());

        REQUIRE(result);
        REQUIRE(*result == "myIndex");
    }

    SECTION("works with document and no options") {
        auto key = make_document(kvp("a", 1), kvp("b", -1));
        stdx::optional<std::string> result = indexes.create_one(key.view());

        REQUIRE(result);
        REQUIRE(*result == "a_1_b_-1");
    }

    SECTION("with index_model and options") {
        auto key = make_document(kvp("a", 1));
        auto options = make_document(kvp("name", "myIndex"));
        index_model model(key.view(), options.view());
        stdx::optional<std::string> result = indexes.create_one(model);

        REQUIRE(result);
        REQUIRE(*result == "myIndex");
    }

    SECTION("with index_model and no options") {
        auto key = make_document(kvp("a", 1), kvp("b", -1));
        index_model model(key.view());
        stdx::optional<std::string> result = indexes.create_one(model);

        REQUIRE(result);
        REQUIRE(*result == "a_1_b_-1");
    }

    SECTION("tests maxTimeMS option works") {
        auto key = make_document(kvp("aaa", 1));
        index_model model(key.view());
        options::index_view options;
        options.max_time(std::chrono::milliseconds(1));

        if (fail_with_max_timeout(mongodb_client)) {
            REQUIRE_THROWS_AS(indexes.create_one(model, options), operation_exception);
        }
    }

    SECTION("fails for same keys and options") {
        auto keys = make_document(kvp("a", 1));

        REQUIRE(indexes.create_one(keys.view()));
        REQUIRE(!indexes.create_one(keys.view()));
    }

    SECTION("fails for same name") {
        auto keys1 = make_document(kvp("a", 1));
        auto keys2 = make_document(kvp("a", -1));
        auto options = make_document(kvp("name", "myIndex"));

        REQUIRE_NOTHROW(indexes.create_one(keys1.view(), options.view()));
        REQUIRE_THROWS_AS(indexes.create_one(keys2.view(), options.view()), operation_exception);
    }
}

TEST_CASE("create_many", "[index_view]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];
    collection coll = db["index_view_create_many"];
    coll.drop();
    coll.insert_one({});  // Ensure that the collection exists.

    std::vector<index_model> models{index_model(make_document(kvp("a", 1))),
                                    index_model(make_document(kvp("b", 1), kvp("c", -1))),
                                    index_model(make_document(kvp("c", -1)))};

    index_view indexes = coll.indexes();

    SECTION("test maxTimeMS option") {
        options::index_view options;
        options.max_time(std::chrono::milliseconds(1));

        if (fail_with_max_timeout(mongodb_client)) {
            REQUIRE_THROWS_AS(indexes.create_many(models, options), operation_exception);
        }
    }

    SECTION("create three") {
        bsoncxx::document::value result = indexes.create_many(models);
        bsoncxx::document::view result_view = result.view();
        REQUIRE((result_view["numIndexesAfter"].get_int32() -
                 result_view["numIndexesBefore"].get_int32()) == 3);

        std::vector<std::string> expected_names{"a_1", "b_1_c_-1", "c_-1"};
        std::int8_t found = 0;

        for (auto&& index : indexes.list()) {
            auto name = index["name"].get_utf8();

            for (auto expected : expected_names) {
                if (stdx::string_view{expected} == name.value) {
                    found++;
                }
            }
        }
        REQUIRE(found == 3);
    }
}

TEST_CASE("drop_one", "[index_view]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];
    collection coll = db["index_view_drop_one"];
    coll.drop();
    coll.insert_one({});  // Ensure that the collection exists.

    index_view indexes = coll.indexes();
    auto cursor = indexes.list();
    REQUIRE(std::distance(cursor.begin(), cursor.end()) == 1);

    SECTION("drops index by name") {
        auto key = make_document(kvp("a", 1));
        stdx::optional<std::string> result = indexes.create_one(key.view());

        auto cursor1 = indexes.list();
        REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 2);
        REQUIRE(result);

        indexes.drop_one(*result);
        auto cursor2 = indexes.list();
        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("drops index by key and options") {
        auto key = make_document(kvp("a", 1));
        stdx::optional<std::string> result = indexes.create_one(key.view());

        auto cursor1 = indexes.list();
        REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 2);
        REQUIRE(result);

        indexes.drop_one(key.view());
        auto cursor2 = indexes.list();
        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("drops index by index_model") {
        auto key = make_document(kvp("a", 1));
        index_model model(key.view());
        stdx::optional<std::string> result = indexes.create_one(key.view());

        auto cursor1 = indexes.list();
        REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 2);
        REQUIRE(result);

        indexes.drop_one(model);
        auto cursor2 = indexes.list();
        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("fails for drop_one on *") {
        REQUIRE_THROWS_AS(indexes.drop_one("*"), logic_error);
    }

    SECTION("fails for index that doesn't exist") {
        REQUIRE_THROWS_AS(indexes.drop_one("foo"), operation_exception);
    }
}

TEST_CASE("drop_all", "[index_view]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];
    collection coll = db["index_view_drop_all"];
    coll.drop();
    coll.insert_one({});  // Ensure that the collection exists.

    std::vector<index_model> models{index_model{make_document(kvp("a", 1))},
                                    index_model{make_document(kvp("b", 1), kvp("c", -1))},
                                    index_model{make_document(kvp("c", -1))}};

    index_view indexes = coll.indexes();

    bsoncxx::document::value result = indexes.create_many(models);
    bsoncxx::document::view result_view = result.view();

    auto cursor1 = indexes.list();
    REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == models.size() + 1);
    REQUIRE((result_view["numIndexesAfter"].get_int32() -
             result_view["numIndexesBefore"].get_int32()) == models.size());

    SECTION("drop normally") {
        indexes.drop_all();
        auto cursor2 = indexes.list();

        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("test maxTimeMS option") {
        options::index_view options;
        options.max_time(std::chrono::milliseconds(1));

        if (fail_with_max_timeout(mongodb_client)) {
            REQUIRE_THROWS_AS(indexes.drop_all(options), operation_exception);
        }
    }
}
}
