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

#include <iostream>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/instance.hpp>

namespace {
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;
using namespace mongocxx;

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
        std::string result = indexes.create_one(key.view(), options.view());
        REQUIRE(result == "myIndex");
    }

    SECTION("works with document and no options") {
        auto key = make_document(kvp("a", 1), kvp("b", -1));
        std::string result = indexes.create_one(key.view());
        REQUIRE(result == "a_1_b_-1");
    }

    SECTION("with index_model and options") {
        auto key = make_document(kvp("a", 1));
        auto options = make_document(kvp("name", "myIndex"));
        index_model model(key.view(), options.view());
        std::string result = indexes.create_one(model);
        REQUIRE(result == "myIndex");
    }

    SECTION("with index_model and no options") {
        auto key = make_document(kvp("a", 1), kvp("b", -1));
        index_model model(key.view());
        std::string result = indexes.create_one(model);
        REQUIRE(result == "a_1_b_-1");
    }

    SECTION("fails for same keys and options") {
        auto keys1 = make_document(kvp("a", 1));
        auto keys2 = make_document(kvp("a", 1));

        REQUIRE_NOTHROW(indexes.create_one(keys1.view()));
        REQUIRE_THROWS_AS(indexes.create_one(keys2.view()), operation_exception);
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

    std::vector<std::string> names = indexes.create_many(models);

    std::vector<std::string> expected_names{"a_1", "b_1_c_-1", "c_-1"};
    REQUIRE(names == expected_names);
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
        std::string result = indexes.create_one(key.view());

        auto cursor1 = indexes.list();
        REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 2);

        indexes.drop_one(stdx::string_view{result});
        auto cursor2 = indexes.list();
        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("drops index by key and options") {
        auto key = make_document(kvp("a", 1));
        std::string result = indexes.create_one(key.view());

        auto cursor1 = indexes.list();
        REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 2);

        indexes.drop_one(key.view());
        auto cursor2 = indexes.list();
        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("drops index by index_model") {
        auto key = make_document(kvp("a", 1));
        index_model model(key.view());
        std::string result = indexes.create_one(key.view());

        auto cursor1 = indexes.list();
        REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 2);

        indexes.drop_one(model);
        auto cursor2 = indexes.list();
        REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
    }

    SECTION("fails for drop_one on *") {
        REQUIRE_THROWS_AS(indexes.drop_one("*"), operation_exception);
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

    std::vector<index_model> models{index_model(make_document(kvp("a", 1))),
                                    index_model(make_document(kvp("b", 1), kvp("c", -1))),
                                    index_model(make_document(kvp("c", -1)))};

    index_view indexes = coll.indexes();

    std::vector<std::string> names = indexes.create_many(models);
    auto cursor1 = indexes.list();
    REQUIRE(std::distance(cursor1.begin(), cursor1.end()) == 4);

    indexes.drop_all();
    auto cursor2 = indexes.list();

    REQUIRE(std::distance(cursor2.begin(), cursor2.end()) == 1);
}
}
