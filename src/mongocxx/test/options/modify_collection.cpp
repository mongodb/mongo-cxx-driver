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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/modify_collection.hpp>

namespace {
using namespace bsoncxx;
using namespace mongocxx;

using builder::basic::kvp;
using builder::basic::make_document;

TEST_CASE("modify_collection", "[modify_collection]") {
    instance::current();

    options::modify_collection cm;

    SECTION("Can be exported to a document") {
        auto rule = make_document(kvp("brain", make_document(kvp("$exists", true))));

        validation_criteria validation;
        validation.rule(rule.view());
        validation.level(validation_criteria::validation_level::k_strict);

        auto index = make_document(kvp("a", 1));

        cm.index(index.view(), std::chrono::seconds(10));
        cm.validation_criteria(validation);
        cm.no_padding(false);

        auto doc = cm.to_document();
        document::view doc_view{doc.view()};

        // noPadding should be set to false
        document::element padding{doc_view["noPadding"]};
        REQUIRE(padding);
        REQUIRE(padding.type() == type::k_bool);
        REQUIRE(padding.get_bool() == false);

        // validator and validationLevel should be set, but not validationAction
        document::element validator{doc_view["validator"]};
        REQUIRE(validator);
        REQUIRE(validator.type() == type::k_document);
        REQUIRE(validator.get_document().value == rule);

        document::element validationLevel{doc_view["validationLevel"]};
        REQUIRE(validationLevel);
        REQUIRE(validationLevel.type() == type::k_utf8);
        REQUIRE(bsoncxx::string::to_string(validationLevel.get_utf8().value) == "strict");

        document::element validationAction{doc_view["validationAction"]};
        REQUIRE(!validationAction);

        // index flag should be set
        document::element ttl_index{doc_view["index"]};
        REQUIRE(ttl_index);
        REQUIRE(ttl_index.type() == type::k_document);

        document::element ttl_key{doc_view["index"]["keyPattern"]};
        REQUIRE(ttl_key);
        REQUIRE(ttl_key.type() == type::k_document);
        REQUIRE(ttl_key.get_document().value == index);

        document::element ttl_seconds{doc_view["index"]["expireAfterSeconds"]};
        REQUIRE(ttl_seconds);
        REQUIRE(ttl_seconds.type() == type::k_int64);
        REQUIRE(ttl_seconds.get_int64() == 10);
    }
}

TEST_CASE("modify_collection equals", "[modify_collection]") {
    instance::current();

    options::modify_collection cm1{};
    options::modify_collection cm2{};

    REQUIRE(cm1 == cm2);
}

TEST_CASE("modify_collection inequals", "[modify_collection]") {
    instance::current();

    options::modify_collection cm1{};
    cm1.no_padding(true);
    options::modify_collection cm2{};

    REQUIRE(cm1 != cm2);
}

}  // namespace
