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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/create_collection.hpp>

using namespace bsoncxx;
using namespace mongocxx;

using builder::stream::close_document;
using builder::stream::finalize;
using builder::stream::open_document;

TEST_CASE("create_collection", "[create_collection]") {
    instance::current();

    options::create_collection cc;

    SECTION("Can be exported to a document") {
        auto rule = builder::stream::document{} << "brain" << open_document << "$exists" << true
                                                << close_document << finalize;

        validation_criteria validation;
        validation.rule(rule.view());
        validation.level(validation_criteria::validation_level::k_strict);

        cc.validation_criteria(validation);
        cc.capped(true);
        cc.size(256);
        cc.max(100);
        cc.no_padding(false);

        auto doc = cc.to_document();
        document::view doc_view{doc.view()};

        // capped field is set to true
        document::element capped{doc_view["capped"]};
        REQUIRE(capped);
        REQUIRE(capped.type() == type::k_bool);
        REQUIRE(capped.get_bool() == true);

        // autoIndexId should not be set
        document::element autoIndex{doc_view["autoIndexId"]};
        REQUIRE(!autoIndex);

        // size should be set
        document::element size{doc_view["size"]};
        REQUIRE(size);
        REQUIRE(size.type() == type::k_int32);
        REQUIRE(size.get_int32() == 256);

        // max should be set
        document::element max{doc_view["max"]};
        REQUIRE(max);
        REQUIRE(max.type() == type::k_int32);
        REQUIRE(max.get_int32() == 100);

        // noPadding should be set to false
        document::element padding{doc_view["noPadding"]};
        REQUIRE(padding);
        REQUIRE(padding.type() == type::k_bool);
        REQUIRE(padding.get_bool() == false);

        // storageEngine should not be set
        document::element engine{doc_view["storageEngine"]};
        REQUIRE(!engine);

        // validator and validationLevel should be set, but not validationAction
        document::element validator{doc_view["validator"]};
        REQUIRE(validator);
        REQUIRE(validator.type() == type::k_document);
        REQUIRE(validator.get_document().value == rule);

        document::element validationLevel{doc_view["validationLevel"]};
        REQUIRE(validationLevel);
        REQUIRE(validationLevel.type() == type::k_utf8);
        REQUIRE(validationLevel.get_utf8().value.to_string() == "strict");

        document::element validationAction{doc_view["validationAction"]};
        REQUIRE(!validationAction);
    }
}
