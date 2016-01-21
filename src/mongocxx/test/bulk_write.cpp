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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/write_concern.hpp>

using namespace mongocxx;

TEST_CASE("a bulk_write will setup a mongoc bulk operation", "[bulk_write]") {
    instance::current();

    auto construct = libmongoc::bulk_operation_new.create_instance();
    bool construct_called = false;
    bool ordered_value = false;

    construct->visit([&](bool ordered) {
        construct_called = true;
        ordered_value = ordered;
    });

    SECTION("with an ordered bulk write") {
        { bulk_write bw; }
        REQUIRE(construct_called);
        REQUIRE(ordered_value);
    }

    SECTION("with an unordered bulk write") {
        options::bulk_write bw_opts;
        bw_opts.ordered(false);
        { bulk_write bw(bw_opts); }
        REQUIRE(construct_called);
        REQUIRE(!ordered_value);
    }
}

TEST_CASE("destruction of a bulk_write will destroy mongoc operation", "[bulk_write]") {
    instance::current();

    auto destruct = libmongoc::bulk_operation_destroy.create_instance();
    bool destruct_called = false;

    destruct->visit([&destruct_called](mongoc_bulk_operation_t*) { destruct_called = true; });

    { bulk_write bw; }
    REQUIRE(destruct_called);
}

class SingleDocumentFun {
   public:
    SingleDocumentFun(bool& called, bsoncxx::document::view document)
        : _called{called}, _document{document} {
        _called = false;
    }

    void operator()(mongoc_bulk_operation_t*, const bson_t* document) {
        _called = true;
        REQUIRE(bson_get_data(document) == _document.data());
    }

    bool called() const {
        return _called;
    }

   private:
    bool& _called;
    bsoncxx::document::view _document;
};

class FilteredDocumentFun : public SingleDocumentFun {
   public:
    FilteredDocumentFun(bool& called, bsoncxx::document::view filter,
                        bsoncxx::document::view document)
        : SingleDocumentFun(called, document), _expected_upsert(false), _filter{filter} {
    }

    void operator()(mongoc_bulk_operation_t* bulk, const bson_t* filter, const bson_t* document,
                    bool upsert) {
        SingleDocumentFun::operator()(bulk, document);
        REQUIRE(bson_get_data(filter) == _filter.data());
        REQUIRE(upsert == _expected_upsert);
    }

    void upsert(bool upsert) {
        _expected_upsert = upsert;
    }

   private:
    bool _expected_upsert;
    bsoncxx::document::view _filter;
};

TEST_CASE("passing write operations to append calls corresponding C function", "[bulk_write]") {
    instance::current();

    bulk_write bw;
    bsoncxx::builder::stream::document filter_builder, doc_builder, update_doc_builder;
    filter_builder << "_id" << 1;
    doc_builder << "_id" << 2;
    update_doc_builder << "$set" << bsoncxx::builder::stream::open_document << "_id" << 2
                       << bsoncxx::builder::stream::close_document;

    bsoncxx::document::view filter = filter_builder.view();
    bsoncxx::document::view doc = doc_builder.view();
    bsoncxx::document::view update_doc = update_doc_builder.view();

    bool single_doc_fun_called;
    SingleDocumentFun single_doc_fun(single_doc_fun_called, doc);
    bool filtered_doc_fun_called;
    FilteredDocumentFun filtered_doc_fun(filtered_doc_fun_called, filter, doc);
    bool update_filtered_doc_fun_called;
    FilteredDocumentFun update_filtered_doc_fun(update_filtered_doc_fun_called, filter, update_doc);

    SECTION("insert_one invokes mongoc_bulk_operation_insert") {
        auto bulk_insert = libmongoc::bulk_operation_insert.create_instance();
        bool bulk_insert_called = false;
        bulk_insert->visit(single_doc_fun);

        bw.append(model::insert_one(doc));
        REQUIRE(single_doc_fun.called());
    }

    SECTION("update_one invokes mongoc_bulk_operation_update_one") {
        auto bulk_insert = libmongoc::bulk_operation_update_one.create_instance();
        bool bulk_update_one_called = false;
        bulk_insert->visit(update_filtered_doc_fun);

        bw.append(model::update_one(filter, update_doc));
        REQUIRE(update_filtered_doc_fun.called());
    }

    SECTION("update_one with upsert invokes mongoc_bulk_operation_update_one with upsert true") {
        auto bulk_insert = libmongoc::bulk_operation_update_one.create_instance();
        bool bulk_update_one_called = false;
        update_filtered_doc_fun.upsert(true);
        bulk_insert->visit(update_filtered_doc_fun);

        model::update_one uo(filter, update_doc);
        uo.upsert(true);
        bw.append(uo);
        REQUIRE(update_filtered_doc_fun.called());
    }

    SECTION("update_many invokes mongoc_bulk_operation_update") {
        auto bulk_insert = libmongoc::bulk_operation_update.create_instance();
        bool bulk_update_called = false;
        bulk_insert->visit(update_filtered_doc_fun);

        bw.append(model::update_many(filter, update_doc));
        REQUIRE(update_filtered_doc_fun.called());
    }

    SECTION("update_many with upsert invokes mongoc_bulk_operation_update with upsert true") {
        auto bulk_insert = libmongoc::bulk_operation_update.create_instance();
        bool bulk_update_called = false;
        update_filtered_doc_fun.upsert(true);
        bulk_insert->visit(update_filtered_doc_fun);

        model::update_many um(filter, update_doc);
        um.upsert(true);
        bw.append(um);
        REQUIRE(update_filtered_doc_fun.called());
    }

    SECTION("delete_one invokes mongoc_bulk_operation_remove_one") {
        auto bulk_insert = libmongoc::bulk_operation_remove_one.create_instance();
        bool bulk_remove_one_called = false;
        bulk_insert->visit(single_doc_fun);

        bw.append(model::delete_one(doc));
        REQUIRE(single_doc_fun.called());
    }

    SECTION("delete_many invokes mongoc_bulk_operation_remove") {
        auto bulk_insert = libmongoc::bulk_operation_remove.create_instance();
        bool bulk_remove_called = false;
        bulk_insert->visit(single_doc_fun);

        bw.append(model::delete_many(doc));
        REQUIRE(single_doc_fun.called());
    }

    SECTION("replace_one invokes mongoc_bulk_operation_replace_one") {
        auto bulk_insert = libmongoc::bulk_operation_replace_one.create_instance();
        bool bulk_replace_one_called = false;
        bulk_insert->visit(filtered_doc_fun);

        bw.append(model::replace_one(filter, doc));
        REQUIRE(filtered_doc_fun.called());
    }

    SECTION("replace_one with upsert invokes mongoc_bulk_operation_replace_one with upsert true") {
        auto bulk_insert = libmongoc::bulk_operation_replace_one.create_instance();
        bool bulk_replace_one_called = false;
        filtered_doc_fun.upsert(true);
        bulk_insert->visit(filtered_doc_fun);

        model::replace_one ro(filter, doc);
        ro.upsert(true);
        bw.append(ro);
        REQUIRE(filtered_doc_fun.called());
    }
}
