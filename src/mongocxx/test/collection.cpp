#include "catch.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/pipeline.hpp>

using namespace mongocxx;

TEST_CASE("CRUD functionality", "[driver::collection]") {
    client mongodb_client;
    database db = mongodb_client["test"];
    collection coll = db["mongo_cxx_driver"];

    if (db.has_collection("mongo_cxx_driver"))
        coll.drop();

    SECTION("insert and read single document", "[collection]") {
        bsoncxx::builder::stream::document b;
        b << "_id" << bsoncxx::oid{bsoncxx::oid::init_tag} << "x" << 1;

        REQUIRE(coll.insert_one(b));

        bsoncxx::builder::stream::document c;
        c << "x" << 1;

        REQUIRE(coll.insert_one(c.view()));

        auto cursor = coll.find(b);

        std::size_t i = 0;
        for (auto&& x : cursor) {
            REQUIRE(x["_id"].get_oid().value == b.view()["_id"].get_oid().value);
            i++;
        }

        REQUIRE(i == 1);
    }

    SECTION("insert and update single document", "[collection]") {
        using namespace bsoncxx::builder::stream;
        bsoncxx::builder::stream::document b1;
        b1 << "_id" << 1;

        coll.insert_one(b1);

        auto doc = coll.find_one(bsoncxx::document::view());
        REQUIRE(doc);
        REQUIRE(doc->view()["_id"].get_int32() == 1);

        bsoncxx::builder::stream::document update_doc;
        update_doc << "$set" << open_document << "changed" << true << close_document;

        coll.update_one(b1, update_doc);

        auto updated = coll.find_one(bsoncxx::document::view());
        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
    }

    SECTION("insert and update multiple documents", "[collection]") {
        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        bsoncxx::builder::stream::document b2;
        b2 << "x" << 2;

        coll.insert_one(b2);

        REQUIRE(coll.count(b1) == 2);

        bsoncxx::builder::stream::document bchanged;
        bchanged << "changed" << true;

        bsoncxx::builder::stream::document update_doc;
        update_doc << "$set" << bsoncxx::types::b_document{bchanged};

        coll.update_many(b1, update_doc);

        REQUIRE(coll.count(bchanged) == 2);
    }

    SECTION("replace document replaces only one document", "[collection]") {
        bsoncxx::builder::stream::document doc;
        doc << "x" << 1;

        coll.insert_one(doc);
        coll.insert_one(doc);

        REQUIRE(coll.count(doc) == 2);

        bsoncxx::builder::stream::document replacement;
        replacement << "x" << 2;

        coll.replace_one(doc, replacement);
        auto c = coll.count(doc);
        REQUIRE(coll.count(doc) == 1);
    }

    SECTION("non-matching upsert creates document", "[collection]") {
        using namespace bsoncxx::builder::stream;
        bsoncxx::builder::stream::document b1;
        b1 << "_id" << 1;

        bsoncxx::builder::stream::document update_doc;
        update_doc << "$set" << open_document << "changed" << true << close_document;

        options::update options;
        options.upsert(true);

        coll.update_one(b1, update_doc, options);

        auto updated = coll.find_one(bsoncxx::document::view());

        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
        REQUIRE(coll.count(bsoncxx::document::view()) == (std::int64_t)1);
    }

    SECTION("matching upsert updates document", "[collection]") {
        using namespace bsoncxx::builder::stream;
        bsoncxx::builder::stream::document b1;
        b1 << "_id" << 1;

        coll.insert_one(b1);

        bsoncxx::builder::stream::document update_doc;
        update_doc << "$set" << open_document << "changed" << true << close_document;

        options::update options;
        options.upsert(true);

        coll.update_one(b1, update_doc, options);

        auto updated = coll.find_one(bsoncxx::document::view());

        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
        REQUIRE(coll.count(bsoncxx::document::view()) == 1);
    }

    // SECTION("matching upsert updates document", "[collection]") {
    // bsoncxx::builder::stream::document b1;
    // b1 << "x" << 1;
    // model::insert_many docs{std::initializer_list<bsoncxx::document::view>{b1, b1, b1}};
    // coll.insert_many(docs);

    // coll.insert_one(bsoncxx::document::view{});
    // REQUIRE(coll.count(b1) == 3);
    // REQUIRE(coll.count() == 4);
    //}

    SECTION("document replacement", "[collection]") {
        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;
        coll.insert_one(b1);

        bsoncxx::builder::stream::document b2;
        b2 << "x" << 2;

        coll.replace_one(b1, b2);

        auto replaced = coll.find_one(b2);

        REQUIRE(replaced);
        REQUIRE(coll.count(bsoncxx::document::view()) == 1);
    }

    SECTION("filtered document delete one works", "[collection]") {
        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);

        bsoncxx::builder::stream::document b2;
        b2 << "x" << 2;

        coll.insert_one(b2);
        coll.insert_one(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == 3);

        coll.delete_one(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == (std::int64_t)2);

        auto cursor = coll.find(bsoncxx::document::view());

        unsigned seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 3);

        coll.delete_one(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == 1);

        cursor = coll.find(bsoncxx::document::view());

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);

        coll.delete_one(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == 1);

        cursor = coll.find(bsoncxx::document::view());

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);
    }

    SECTION("delete many works", "[collection]") {
        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);

        bsoncxx::builder::stream::document b2;
        b2 << "x" << 2;

        coll.insert_one(b2);
        coll.insert_one(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == 3);

        coll.delete_many(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == 1);

        auto cursor = coll.find(bsoncxx::document::view());

        unsigned seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);

        coll.delete_many(b2);

        REQUIRE(coll.count(bsoncxx::document::view()) == 1);

        cursor = coll.find(bsoncxx::document::view());

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);
    }

    SECTION("find_one_and_replace works", "[collection]") {
        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        REQUIRE(coll.count(bsoncxx::document::view()) == 2);

        bsoncxx::builder::stream::document criteria;
        bsoncxx::builder::stream::document replacement;

        criteria << "x" << 1;
        replacement << "x" << 2;

        SECTION("without return replacement returns original") {
            auto doc = coll.find_one_and_replace(criteria, replacement);
            REQUIRE(doc->view()["x"].get_int32() == 1);
        }

        SECTION("with return replacement returns new") {
            options::find_one_and_replace options;
            options.return_document(options::return_document::k_after);
            auto doc = coll.find_one_and_replace(criteria, replacement, options);

            REQUIRE(doc->view()["x"].get_int32() == 2);
        }

        SECTION("bad criteria returns negative optional") {
            bsoncxx::builder::stream::document bad_criteria;
            bad_criteria << "x" << 3;

            auto doc = coll.find_one_and_replace(bad_criteria, replacement);

            REQUIRE(!doc);
        }
    }

    SECTION("find_one_and_update works", "[collection]") {
        using namespace bsoncxx::builder::stream;

        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        REQUIRE(coll.count(bsoncxx::document::view()) == 2);

        bsoncxx::builder::stream::document criteria;
        bsoncxx::builder::stream::document update;

        criteria << "x" << 1;
        update << "$set" << open_document << "x" << 2 << close_document;

        SECTION("without return update returns original") {
            auto doc = coll.find_one_and_update(criteria, update);

            REQUIRE(doc);

            REQUIRE(doc->view()["x"].get_int32() == 1);
        }

        SECTION("with return update returns new") {
            options::find_one_and_update options;
            options.return_document(options::return_document::k_after);
            auto doc = coll.find_one_and_update(criteria, update, options);

            REQUIRE(doc);
            REQUIRE(doc->view()["x"].get_int32() == 2);
        }

        SECTION("bad criteria returns negative optional") {
            bsoncxx::builder::stream::document bad_criteria;
            bad_criteria << "x" << 3;

            auto doc = coll.find_one_and_update(bad_criteria, update);

            REQUIRE(!doc);
        }
    }

    SECTION("find_one_and_delete works", "[collection]") {
        using namespace bsoncxx::builder::stream;

        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        REQUIRE(coll.count(bsoncxx::document::view()) == 2);

        bsoncxx::builder::stream::document criteria;

        criteria << "x" << 1;

        SECTION("delete one deletes one and returns it") {
            auto doc = coll.find_one_and_delete(criteria);

            REQUIRE(doc);

            REQUIRE(doc->view()["x"].get_int32() == 1);
            REQUIRE(coll.count(bsoncxx::document::view()) == 1);
        }
    }

    SECTION("aggregate some things", "[collection]") {
        bsoncxx::builder::stream::document b1;
        b1 << "x" << 1;

        bsoncxx::builder::stream::document b2;
        b2 << "x" << 2;

        coll.insert_one(b1);
        coll.insert_one(b2);
        coll.insert_one(b2);

        pipeline p;
        p.match(b1);

        auto results = coll.aggregate(p);
    }
}
