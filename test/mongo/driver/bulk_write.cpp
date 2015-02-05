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

#include <mongo/bson/builder.hpp>
#include <mongo/bson/types.hpp>

#include <mongo/driver/private/libmongoc.hpp>
#include <mongo/driver/bulk_write.hpp>
#include <mongo/driver/write_concern.hpp>

using namespace mongo;
using namespace mongo::driver;

TEST_CASE("a bulk_write will setup a mongoc bulk operation", "[bulk_write]") {
    auto construct = libmongoc::bulk_operation_new.create_instance();
    bool construct_called = false;
    bool ordered_value = false;
    construct->visit([&](bool ordered) {
        construct_called = true;
        ordered_value = ordered;
    });

    SECTION("with an ordered bulk write") {
        bulk_write(true);
        REQUIRE(construct_called);
        REQUIRE(ordered_value);
    }

    SECTION("with an unordered bulk write") {
        bulk_write(false);
        REQUIRE(construct_called);
        REQUIRE(!ordered_value);
    }
}
TEST_CASE("the destruction of a bulk_write will destroy the mongoc operation",
          "[bulk_write]") {
    auto destruct = libmongoc::bulk_operation_destroy.create_instance();
    bool destruct_called = false;
    destruct->visit([&destruct_called](mongoc_bulk_operation_t* op) { destruct_called = true; });
    bulk_write(true);
    REQUIRE(destruct_called);
}

// TEST_CASE("bulk_write has a write_concern", "[bulk_write]") {
// bulk_write bw(true);
// CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(bw, write_concern, write_concern());
//}

class SingleDocumentFun {
   public:
    SingleDocumentFun(bool& called, bson::document::view document)
        : _called{called}, _document{document} {
        _called = false;
    }
    void operator()(mongoc_bulk_operation_t* bulk, const bson_t* document) {
        _called = true;
        REQUIRE(bson_get_data(document) == _document.get_buf());
    }
    bool called() const {
        return _called;
    }

   private:
    bool& _called;
    bson::document::view _document;
};

class FilteredDocumentFun : public SingleDocumentFun {
   public:
    FilteredDocumentFun(bool& called, bson::document::view filter, bson::document::view document)
        : SingleDocumentFun(called, document), _expected_upsert(false), _filter{filter} {
    }
    void operator()(mongoc_bulk_operation_t* bulk, const bson_t* filter, const bson_t* document,
                    bool upsert) {
        SingleDocumentFun::operator()(bulk, document);
        REQUIRE(bson_get_data(filter) == _filter.get_buf());
        REQUIRE(upsert == _expected_upsert);
    }
    void upsert(bool upsert) {
        _expected_upsert = upsert;
    }

   private:
    bool _expected_upsert;
    bson::document::view _filter;
};

TEST_CASE("passing valid write operations to append calls the corresponding C function",
          "[bulk_write]") {
    bulk_write bw(true);
    bson::builder::document filter_builder, doc_builder;
    filter_builder << "_id" << 1;
    doc_builder << "_id" << 2;
    bson::document::view filter = filter_builder.view();
    bson::document::view doc = doc_builder.view();
    bool single_doc_fun_called;
    SingleDocumentFun single_doc_fun(single_doc_fun_called, doc);
    bool filtered_doc_fun_called;
    FilteredDocumentFun filtered_doc_fun(filtered_doc_fun_called, filter, doc);

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
        bulk_insert->visit(filtered_doc_fun);

        bw.append(model::update_one(filter, doc));
        REQUIRE(filtered_doc_fun.called());
    }

    SECTION("update_one with upsert invokes mongoc_bulk_operation_update_one with upsert true") {
        auto bulk_insert = libmongoc::bulk_operation_update_one.create_instance();
        bool bulk_update_one_called = false;
        filtered_doc_fun.upsert(true);
        bulk_insert->visit(filtered_doc_fun);

        model::update_one uo(filter, doc);
        uo.upsert(true);
        bw.append(uo);
        REQUIRE(filtered_doc_fun.called());
    }

    SECTION("update_many invokes mongoc_bulk_operation_update") {
        auto bulk_insert = libmongoc::bulk_operation_update.create_instance();
        bool bulk_update_called = false;
        bulk_insert->visit(filtered_doc_fun);

        bw.append(model::update_many(filter, doc));
        REQUIRE(filtered_doc_fun.called());
    }

    SECTION("update_many with upsert invokes mongoc_bulk_operation_update with upsert true") {
        auto bulk_insert = libmongoc::bulk_operation_update.create_instance();
        bool bulk_update_called = false;
        filtered_doc_fun.upsert(true);
        bulk_insert->visit(filtered_doc_fun);

        model::update_many um(filter, doc);
        um.upsert(true);
        bw.append(um);
        REQUIRE(filtered_doc_fun.called());
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
