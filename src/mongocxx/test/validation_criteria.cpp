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
#include <bsoncxx/document/element.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/validation_criteria.hpp>

using namespace mongocxx;
using namespace bsoncxx;

using builder::stream::open_document;
using builder::stream::close_document;
using builder::stream::finalize;

TEST_CASE("validation_criteria", "[validation_criteria]") {
    instance::current();

    validation_criteria criteria;

    auto doc = builder::stream::document{} << "email" << open_document << "$exists"
                                           << "true" << close_document << finalize;

    SECTION("Can contain a validation level") {
        criteria.level(validation_criteria::validation_level::k_off);
    }

    SECTION("Can contain a validation action") {
        criteria.action(validation_criteria::validation_action::k_warn);
    }

    SECTION("Can contain a validator") {
        criteria.rule(doc.view());
    }

    SECTION("Can be exported to a document") {
        criteria.level(validation_criteria::validation_level::k_strict);
        criteria.action(validation_criteria::validation_action::k_warn);
        criteria.rule(doc.view());

        auto criteria_doc = criteria.to_document();
        auto criteria_view = criteria_doc.view();

        document::element ele;

        ele = criteria_view["validationLevel"];
        REQUIRE(ele);
        REQUIRE(ele.type() == type::k_utf8);
        REQUIRE(ele.get_utf8().value.to_string() == "strict");

        ele = criteria_view["validationAction"];
        REQUIRE(ele);
        REQUIRE(ele.type() == type::k_utf8);
        REQUIRE(ele.get_utf8().value.to_string() == "warn");

        ele = criteria_view["validator"];
        REQUIRE(ele);
        REQUIRE(ele.type() == type::k_document);
        REQUIRE(ele.get_document().value == doc);
    }
}
