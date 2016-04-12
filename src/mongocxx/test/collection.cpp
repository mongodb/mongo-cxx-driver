// Copyright 2015 MongoDB Inc.
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

#include <vector>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/insert_many_builder.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/read_concern.hpp>

using namespace bsoncxx::builder::stream;
using namespace mongocxx;

TEST_CASE("A default constructed collection cannot perform operations", "[collection]") {
    instance::current();

    collection c;
    REQUIRE_THROWS_AS(c.name(), mongocxx::logic_error);
}

TEST_CASE("collection copy", "[collection]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];

    std::string collname{"foo"};
    std::string collname2{"bar"};
    collection coll = db[collname];

    collection coll2{coll};
    collection coll3 = db[collname2];
    coll3 = coll;

    REQUIRE(coll2.name() == stdx::string_view{collname});
    REQUIRE(coll3.name() == stdx::string_view{collname});
}

TEST_CASE("collection renaming", "[collection]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];

    std::string collname{"mongo_cxx_driver"};
    collection coll = db[collname];
    coll.insert_one({});  // Ensure that the collection exists.

    REQUIRE(coll.name() == stdx::string_view{collname});

    std::string new_name{"mongo_cxx_newname"};
    coll.rename(new_name, true);

    REQUIRE(coll.name() == stdx::string_view{new_name});
}

TEST_CASE("CRUD functionality", "[driver::collection]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];
    collection coll = db["mongo_cxx_driver"];

    coll.drop();

    SECTION("insert and read single document", "[collection]") {
        auto b = document{} << "_id" << bsoncxx::oid{bsoncxx::oid::init_tag} << "x" << 1
                            << finalize;

        REQUIRE(coll.insert_one(b.view()));

        auto c = document{} << "x" << 1 << finalize;
        REQUIRE(coll.insert_one(c.view()));

        auto cursor = coll.find(b.view());

        std::size_t i = 0;
        for (auto&& x : cursor) {
            REQUIRE(x["_id"].get_oid().value == b.view()["_id"].get_oid().value);
            i++;
        }

        REQUIRE(i == 1);
    }

    SECTION("insert and read multiple documents", "[collection]") {
        document b1;
        document b2;
        document b3;
        document b4;

        b1 << "_id" << bsoncxx::oid{bsoncxx::oid::init_tag} << "x" << 1;
        b2 << "x" << 2;
        b3 << "x" << 3;
        b4 << "_id" << bsoncxx::oid{bsoncxx::oid::init_tag} << "x" << 4;

        std::vector<bsoncxx::document::view> docs{};
        docs.push_back(b1.view());
        docs.push_back(b2.view());
        docs.push_back(b3.view());
        docs.push_back(b4.view());

        auto result = coll.insert_many(docs, options::insert{});

        REQUIRE(result);
        REQUIRE(result->inserted_count() == 4);

        auto cursor = coll.find({});

        std::int32_t i = 0;
        for (auto&& x : cursor) {
            i++;
            REQUIRE(x["x"].get_int32() == i);
        }

        REQUIRE(i == 4);
    }

    SECTION("insert and update single document", "[collection]") {
        auto b1 = document{} << "_id" << 1 << finalize;

        coll.insert_one(b1.view());

        auto doc = coll.find_one({});
        REQUIRE(doc);
        REQUIRE(doc->view()["_id"].get_int32() == 1);

        document update_doc;
        update_doc << "$set" << open_document << "changed" << true << close_document;

        coll.update_one(b1.view(), update_doc.view());

        auto updated = coll.find_one({});
        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
    }

    SECTION("insert and update multiple documents", "[collection]") {
        auto b1 = document{} << "x" << 1 << finalize;

        coll.insert_one(b1.view());
        coll.insert_one(b1.view());

        auto b2 = document{} << "x" << 2 << finalize;

        coll.insert_one(b2.view());

        REQUIRE(coll.count(b1.view()) == 2);

        document bchanged;
        bchanged << "changed" << true;

        document update_doc;
        update_doc << "$set" << bsoncxx::types::b_document{bchanged};

        coll.update_many(b1.view(), update_doc.view());

        REQUIRE(coll.count(bchanged.view()) == 2);
    }

    SECTION("replace document replaces only one document", "[collection]") {
        document doc;
        doc << "x" << 1;

        coll.insert_one(doc.view());
        coll.insert_one(doc.view());

        REQUIRE(coll.count(doc.view()) == 2);

        document replacement;
        replacement << "x" << 2;

        coll.replace_one(doc.view(), replacement.view());
        auto c = coll.count(doc.view());
        REQUIRE(coll.count(doc.view()) == 1);
    }

    SECTION("non-matching upsert creates document", "[collection]") {
        document b1;
        b1 << "_id" << 1;

        document update_doc;
        update_doc << "$set" << open_document << "changed" << true << close_document;

        options::update options;
        options.upsert(true);

        auto result = coll.update_one(b1.view(), update_doc.view(), options);
        REQUIRE(result->upserted_id());

        auto updated = coll.find_one({});

        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
        REQUIRE(coll.count({}) == (std::int64_t)1);
    }

    SECTION("matching upsert updates document", "[collection]") {
        document b1;
        b1 << "_id" << 1;

        coll.insert_one(b1.view());

        document update_doc;
        update_doc << "$set" << open_document << "changed" << true << close_document;

        options::update options;
        options.upsert(true);

        auto result = coll.update_one(b1.view(), update_doc.view(), options);
        REQUIRE(!(result->upserted_id()));

        auto updated = coll.find_one({});

        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
        REQUIRE(coll.count({}) == 1);
    }

    SECTION("test using an insert_many_builder on this collection", "[collection]") {
        auto doc_value = document{} << "x" << 1 << finalize;
        auto doc_view = doc_value.view();

        insert_many_builder insert_many{options::insert()};
        insert_many(doc_view);
        insert_many(doc_view);
        insert_many(doc_view);

        insert_many.insert(&coll);

        coll.insert_one(document{} << "b" << 1 << finalize);

        REQUIRE(coll.count(doc_view) == 3);
        REQUIRE(coll.count({}) == 4);
    }

    SECTION("document replacement", "[collection]") {
        document b1;
        b1 << "x" << 1;
        coll.insert_one(b1.view());

        document b2;
        b2 << "x" << 2;

        coll.replace_one(b1.view(), b2.view());

        auto replaced = coll.find_one(b2.view());

        REQUIRE(replaced);
        REQUIRE(coll.count({}) == 1);
    }

    SECTION("filtered document delete one works", "[collection]") {
        document b1;
        b1 << "x" << 1;

        coll.insert_one(b1.view());

        document b2;
        b2 << "x" << 2;

        coll.insert_one(b2.view());
        coll.insert_one(b2.view());

        REQUIRE(coll.count({}) == 3);

        coll.delete_one(b2.view());

        REQUIRE(coll.count({}) == (std::int64_t)2);

        auto cursor = coll.find({});

        unsigned seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 3);

        coll.delete_one(b2.view());

        REQUIRE(coll.count({}) == 1);

        cursor = coll.find({});

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);

        coll.delete_one(b2.view());

        REQUIRE(coll.count({}) == 1);

        cursor = coll.find({});

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);
    }

    SECTION("delete many works", "[collection]") {
        document b1;
        b1 << "x" << 1;

        coll.insert_one(b1.view());

        document b2;
        b2 << "x" << 2;

        coll.insert_one(b2.view());
        coll.insert_one(b2.view());

        REQUIRE(coll.count({}) == 3);

        coll.delete_many(b2.view());

        REQUIRE(coll.count({}) == 1);

        auto cursor = coll.find({});

        unsigned seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);

        coll.delete_many(b2.view());

        REQUIRE(coll.count({}) == 1);

        cursor = coll.find({});

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);
    }

    SECTION("find works with sort", "[collection]") {
        document b1;
        b1 << "x" << 1;

        document b2;
        b2 << "x" << 2;

        document b3;
        b3 << "x" << 3;

        coll.insert_one(b1.view());
        coll.insert_one(b3.view());
        coll.insert_one(b2.view());

        SECTION("sort ascending") {
            document sort;
            sort << "x" << 1;
            options::find opts{};
            opts.sort(sort.view());

            auto cursor = coll.find({}, opts);

            std::int32_t x = 1;
            for (auto&& doc : cursor) {
                REQUIRE(x == doc["x"].get_int32());
                x++;
            }
        }

        SECTION("sort descending") {
            document sort;
            sort << "x" << -1;
            options::find opts{};
            opts.sort(sort.view());

            auto cursor = coll.find({}, opts);

            std::int32_t x = 3;
            for (auto&& doc : cursor) {
                REQUIRE(x == doc["x"].get_int32());
                x--;
            }
        }
    }

    SECTION("find_one_and_replace works", "[collection]") {
        document b1;
        b1 << "x" << 1;

        coll.insert_one(b1.view());
        coll.insert_one(b1.view());

        REQUIRE(coll.count({}) == 2);

        document criteria;
        document replacement;

        criteria << "x" << 1;
        replacement << "x" << 2;

        SECTION("without return replacement returns original") {
            auto doc = coll.find_one_and_replace(criteria.view(), replacement.view());
            REQUIRE(doc);
            REQUIRE(doc->view()["x"].get_int32() == 1);
        }

        SECTION("with return replacement returns new") {
            options::find_one_and_replace options;
            options.return_document(options::return_document::k_after);
            auto doc = coll.find_one_and_replace(criteria.view(), replacement.view(), options);
            REQUIRE(doc);
            REQUIRE(doc->view()["x"].get_int32() == 2);
        }

        SECTION("bad criteria returns negative optional") {
            document bad_criteria;
            bad_criteria << "x" << 3;

            auto doc = coll.find_one_and_replace(bad_criteria.view(), replacement.view());

            REQUIRE(!doc);
        }
    }

    SECTION("find_one_and_update works", "[collection]") {
        document b1;
        b1 << "x" << 1;

        coll.insert_one(b1.view());
        coll.insert_one(b1.view());

        REQUIRE(coll.count({}) == 2);

        document criteria;
        document update;

        criteria << "x" << 1;
        update << "$set" << open_document << "x" << 2 << close_document;

        SECTION("without return update returns original") {
            auto doc = coll.find_one_and_update(criteria.view(), update.view());

            REQUIRE(doc);

            REQUIRE(doc->view()["x"].get_int32() == 1);
        }

        SECTION("with return update returns new") {
            options::find_one_and_update options;
            options.return_document(options::return_document::k_after);
            auto doc = coll.find_one_and_update(criteria.view(), update.view(), options);

            REQUIRE(doc);
            REQUIRE(doc->view()["x"].get_int32() == 2);
        }

        SECTION("bad criteria returns negative optional") {
            document bad_criteria;
            bad_criteria << "x" << 3;

            auto doc = coll.find_one_and_update(bad_criteria.view(), update.view());

            REQUIRE(!doc);
        }
    }

    SECTION("find_one_and_delete works", "[collection]") {
        document b1;
        b1 << "x" << 1;

        coll.insert_one(b1.view());
        coll.insert_one(b1.view());

        REQUIRE(coll.count({}) == 2);

        document criteria;

        criteria << "x" << 1;

        SECTION("delete one deletes one and returns it") {
            auto doc = coll.find_one_and_delete(criteria.view());

            REQUIRE(doc);

            REQUIRE(doc->view()["x"].get_int32() == 1);
            REQUIRE(coll.count({}) == 1);
        }
    }

    SECTION("aggregate some things", "[collection]") {
        document b1;
        b1 << "x" << 1;

        document b2;
        b2 << "x" << 2;

        coll.insert_one(b1.view());
        coll.insert_one(b2.view());
        coll.insert_one(b2.view());

        pipeline p;
        p.match(b1.view());

        auto results = coll.aggregate(p);
    }

    SECTION("aggregation $lookup operator", "[collection]") {
        auto people_coll_name = "people_on_the_block";
        auto people_coll = db.create_collection(people_coll_name);
        auto houses_coll_name = "houses_on_the_block";
        auto houses_coll = db.create_collection(houses_coll_name);

        // populate one collection with names
        document name1;
        name1 << "firstname"
              << "Tasha"
              << "lastname"
              << "Brown";
        document name2;
        name2 << "firstname"
              << "Logan"
              << "lastname"
              << "Brown";
        document name3;
        name3 << "firstname"
              << "Tasha"
              << "lastname"
              << "Johnson";

        people_coll.insert_one(name1.view());
        people_coll.insert_one(name2.view());
        people_coll.insert_one(name3.view());

        // populate the other with addresses
        document address1;
        address1 << "household"
                 << "Brown"
                 << "address"
                 << "23 Prince St";
        document address2;
        address2 << "household"
                 << "Johnson"
                 << "address"
                 << "15 Prince St";

        houses_coll.insert_one(address1.view());
        houses_coll.insert_one(address2.view());

        // perform a $lookup
        document lookup_doc;
        lookup_doc << "from" << people_coll_name << "localField"
                   << "household"
                   << "foreignField"
                   << "lastname"
                   << "as"
                   << "residents";

        pipeline stages;
        stages.lookup(lookup_doc.view());

        auto results = houses_coll.aggregate(stages);

        // Should have two result documents, one per household
        REQUIRE(std::distance(results.begin(), results.end()) == 2);

        houses_coll.drop();
        people_coll.drop();
    }

    SECTION("distinct works", "[collection]") {
        auto distinct_cname = "distinct_coll";
        auto distinct_coll = db[distinct_cname];
        distinct_coll.drop();

        auto doc1 = document{} << "foo"
                               << "baz"
                               << "garply" << 1 << finalize;
        auto doc2 = document{} << "foo"
                               << "bar"
                               << "garply" << 2 << finalize;
        auto doc3 = document{} << "foo"
                               << "baz"
                               << "garply" << 2 << finalize;
        auto doc4 = document{} << "foo"
                               << "quux"
                               << "garply" << 9 << finalize;

        options::bulk_write bulk_opts;
        bulk_opts.ordered(false);
        bulk_write bulk{bulk_opts};

        bulk.append(model::insert_one{std::move(doc1)});
        bulk.append(model::insert_one{std::move(doc2)});
        bulk.append(model::insert_one{std::move(doc3)});
        bulk.append(model::insert_one{std::move(doc4)});

        distinct_coll.bulk_write(bulk);

        auto distinct_results = distinct_coll.distinct("foo", {});

        // copy into a vector.
        std::vector<bsoncxx::document::value> results;
        for (auto&& result : distinct_results) {
            results.emplace_back(result);
        }

        REQUIRE(results.size() == std::size_t{1});

        auto res_doc = results[0].view();
        auto values_array = res_doc["values"].get_array().value;

        std::vector<stdx::string_view> distinct_values;
        for (auto&& value : values_array) {
            distinct_values.push_back(value.get_utf8().value);
        }

        const auto assert_contains_one = [&](stdx::string_view val) {
            REQUIRE(std::count(distinct_values.begin(), distinct_values.end(), val) == 1);
        };

        assert_contains_one("baz");
        assert_contains_one("bar");
        assert_contains_one("quux");
    }
}

TEST_CASE("read_concern is inherited from parent", "[collection]") {
    client mongo_client{uri{}};
    database db = mongo_client["test"];

    read_concern::level majority = read_concern::level::k_majority;
    read_concern::level local = read_concern::level::k_local;

    read_concern rc{};
    rc.acknowledge_level(majority);
    db.read_concern(rc);

    collection coll = db["rc"];

    SECTION("when parent is a database") {
        REQUIRE(coll.read_concern().acknowledge_level() == read_concern::level::k_majority);
    }

    SECTION("except when read_concern is explicitly set") {
        read_concern set_rc{};
        set_rc.acknowledge_level(read_concern::level::k_local);
        coll.read_concern(set_rc);

        REQUIRE(coll.read_concern().acknowledge_level() == local);
    }
}

TEST_CASE("create_index returns index name", "[collection]") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["test"];
    collection coll = db["collection"];
    coll.insert_one({});  // Ensure that the collection exists.

    bsoncxx::document::value index = bsoncxx::builder::stream::document{}
                                     << "a" << 1 << bsoncxx::builder::stream::finalize;

    std::string indexName{"myName"};
    options::index options{};
    options.name(indexName);

    auto response = coll.create_index(index.view(), options);
    REQUIRE(response.view()["name"].get_utf8().value == stdx::string_view{indexName});

    bsoncxx::document::value index2 = bsoncxx::builder::stream::document{}
                                      << "b" << 1 << "c" << -1
                                      << bsoncxx::builder::stream::finalize;

    auto response2 = coll.create_index(index2.view(), options::index{});
    REQUIRE(response2.view()["name"].get_utf8().value == stdx::string_view{"b_1_c_-1"});
}
