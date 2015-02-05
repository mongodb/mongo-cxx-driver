#include "catch.hpp"

#include <mongo/bson/builder.hpp>
#include <mongo/bson/types.hpp>
#include <mongo/driver/collection.hpp>
#include <mongo/driver/client.hpp>
#include <mongo/driver/pipeline.hpp>

using namespace mongo;
using namespace mongo::driver;

TEST_CASE("CRUD functionality", "[driver::collection]") {
    client mongodb_client;
    database db = mongodb_client["test"];
    collection coll = db["mongo_cxx_driver"];
    coll.drop();

    SECTION("insert and read single document", "[collection]") {
        bson::builder::document b;
        b << "_id" << bson::oid{bson::oid::init_tag} << "x" << 1;

        REQUIRE(coll.insert_one(b));

        bson::builder::document c;
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
        using namespace bson::builder::helpers;
        bson::builder::document b1;
        b1 << "_id" << 1;

        coll.insert_one(b1);

        auto doc = coll.find_one(bson::document::view());
        REQUIRE(doc);
        REQUIRE(doc->view()["_id"].get_int32() == 1);

        bson::builder::document update_doc;
        update_doc << "$set" << open_doc << "changed" << true << close_doc;

        coll.update_one(b1, update_doc);

        auto updated = coll.find_one(bson::document::view());
        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
    }

    SECTION("insert and update multiple documents", "[collection]") {
        bson::builder::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        bson::builder::document b2;
        b2 << "x" << 2;

        coll.insert_one(b2);

        REQUIRE(coll.count(b1) == 2);

        bson::builder::document bchanged;
        bchanged << "changed" << true;

        bson::builder::document update_doc;
        update_doc << "$set" << bson::types::b_document{bchanged};

        coll.update_many(b1, update_doc);

        REQUIRE(coll.count(bchanged) == 2);
    }

    SECTION("replace document replaces only one document", "[collection]") {
        bson::builder::document doc;
        doc << "x" << 1;

        coll.insert_one(doc);
        coll.insert_one(doc);

        REQUIRE(coll.count(doc) == 2);

        bson::builder::document replacement;
        replacement << "x" << 2;

        coll.replace_one(doc, replacement);
        auto c = coll.count(doc);
        REQUIRE(coll.count(doc) == 1);
    }

    SECTION("non-matching upsert creates document", "[collection]") {
        using namespace bson::builder::helpers;
        bson::builder::document b1;
        b1 << "_id" << 1;

        bson::builder::document update_doc;
        update_doc << "$set" << open_doc << "changed" << true << close_doc;

        options::update options;
        options.upsert(true);

        coll.update_one(b1, update_doc, options);

        auto updated = coll.find_one(bson::document::view());

        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
        REQUIRE(coll.count(bson::document::view()) == (std::int64_t)1);
    }

    SECTION("matching upsert updates document", "[collection]") {
        using namespace bson::builder::helpers;
        bson::builder::document b1;
        b1 << "_id" << 1;

        coll.insert_one(b1);

        bson::builder::document update_doc;
        update_doc << "$set" << open_doc << "changed" << true << close_doc;

        options::update options;
        options.upsert(true);

        coll.update_one(b1, update_doc, options);

        auto updated = coll.find_one(bson::document::view());

        REQUIRE(updated);
        REQUIRE(updated->view()["changed"].get_bool() == true);
        REQUIRE(coll.count(bson::document::view()) == 1);
    }

    // SECTION("matching upsert updates document", "[collection]") {
    // bson::builder::document b1;
    // b1 << "x" << 1;
    // model::insert_many docs{std::initializer_list<bson::document::view>{b1, b1, b1}};
    // coll.insert_many(docs);

    // coll.insert_one(bson::document::view{});
    // REQUIRE(coll.count(b1) == 3);
    // REQUIRE(coll.count() == 4);
    //}

    SECTION("document replacement", "[collection]") {
        bson::builder::document b1;
        b1 << "x" << 1;
        coll.insert_one(b1);

        bson::builder::document b2;
        b2 << "x" << 2;

        coll.replace_one(b1, b2);

        auto replaced = coll.find_one(b2);

        REQUIRE(replaced);
        REQUIRE(coll.count(bson::document::view()) == 1);
    }

    SECTION("filtered document delete one works", "[collection]") {
        bson::builder::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);

        bson::builder::document b2;
        b2 << "x" << 2;

        coll.insert_one(b2);
        coll.insert_one(b2);

        REQUIRE(coll.count(bson::document::view()) == 3);

        coll.delete_one(b2);

        REQUIRE(coll.count(bson::document::view()) == (std::int64_t)2);

        auto cursor = coll.find(bson::document::view());

        unsigned seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 3);

        coll.delete_one(b2);

        REQUIRE(coll.count(bson::document::view()) == 1);

        cursor = coll.find(bson::document::view());

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);

        coll.delete_one(b2);

        REQUIRE(coll.count(bson::document::view()) == 1);

        cursor = coll.find(bson::document::view());

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);
    }

    SECTION("delete many works", "[collection]") {
        bson::builder::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);

        bson::builder::document b2;
        b2 << "x" << 2;

        coll.insert_one(b2);
        coll.insert_one(b2);

        REQUIRE(coll.count(bson::document::view()) == 3);

        coll.delete_many(b2);

        REQUIRE(coll.count(bson::document::view()) == 1);

        auto cursor = coll.find(bson::document::view());

        unsigned seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);

        coll.delete_many(b2);

        REQUIRE(coll.count(bson::document::view()) == 1);

        cursor = coll.find(bson::document::view());

        seen = 0;
        for (auto&& x : cursor) {
            seen |= x["x"].get_int32();
        }

        REQUIRE(seen == 1);
    }

    SECTION("find_one_and_replace works", "[collection]") {
        bson::builder::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        REQUIRE(coll.count(bson::document::view()) == 2);

        bson::builder::document criteria;
        bson::builder::document replacement;

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
            bson::builder::document bad_criteria;
            bad_criteria << "x" << 3;

            auto doc = coll.find_one_and_replace(bad_criteria, replacement);

            REQUIRE(!doc);
        }
    }

    SECTION("find_one_and_update works", "[collection]") {
        using namespace bson::builder::helpers;

        bson::builder::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        REQUIRE(coll.count(bson::document::view()) == 2);

        bson::builder::document criteria;
        bson::builder::document update;

        criteria << "x" << 1;
        update << "$set" << open_doc << "x" << 2 << close_doc;

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
            bson::builder::document bad_criteria;
            bad_criteria << "x" << 3;

            auto doc = coll.find_one_and_update(bad_criteria, update);

            REQUIRE(!doc);
        }
    }

    SECTION("find_one_and_delete works", "[collection]") {
        using namespace bson::builder::helpers;

        bson::builder::document b1;
        b1 << "x" << 1;

        coll.insert_one(b1);
        coll.insert_one(b1);

        REQUIRE(coll.count(bson::document::view()) == 2);

        bson::builder::document criteria;

        criteria << "x" << 1;

        SECTION("delete one deletes one and returns it") {
            auto doc = coll.find_one_and_delete(criteria);

            REQUIRE(doc);

            REQUIRE(doc->view()["x"].get_int32() == 1);
            REQUIRE(coll.count(bson::document::view()) == 1);
        }
    }

    SECTION("aggregate some things", "[collection]") {
        bson::builder::document b1;
        b1 << "x" << 1;

        bson::builder::document b2;
        b2 << "x" << 2;

        coll.insert_one(b1);
        coll.insert_one(b2);
        coll.insert_one(b2);

        pipeline p;
        p.match(b1);

        auto results = coll.aggregate(p);
    }
}
