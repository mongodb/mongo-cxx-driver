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
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/instance.hpp>

using namespace mongocxx;
using namespace bsoncxx;

TEST_CASE("Hint", "[hint]") {
    instance::current();

    SECTION("Can be constructed with index name") {
        std::string index_name = "a_1";
        hint index_hint{index_name};

        SECTION("Can be applied to a query") {
            document::value filter = builder::stream::document{}
                                     << "a" << 15
                                     << builder::stream::concatenate(index_hint.to_document())
                                     << builder::stream::finalize;
            document::view view{filter.view()};
            document::element ele{view["$hint"]};
            REQUIRE(ele);
            REQUIRE(ele.type() == type::k_utf8);
            REQUIRE(ele.get_utf8().value.to_string() == index_name);
        }

        SECTION("Compares equal to matching index name") {
            REQUIRE(index_hint == index_name);
            REQUIRE(index_name == index_hint);
        }

        SECTION("Does not equal non-matching index name") {
            REQUIRE(index_hint != "sam");
            REQUIRE("sam" != index_hint);
        }

        SECTION("Does not equal index document") {
            auto index_doc = builder::stream::document{} << "a" << 1 << builder::stream::finalize;
            REQUIRE(index_hint != index_doc);
        }
    }

    SECTION("Can be constructed with index document value") {
        auto index_doc = builder::stream::document{} << "a" << 1 << builder::stream::finalize;
        document::value index_copy{index_doc};

        hint index_hint{std::move(index_doc)};

        SECTION("Can be applied to a query") {
            document::value filter = builder::stream::document{}
                                     << "a" << 12
                                     << builder::stream::concatenate(index_hint.to_document())
                                     << builder::stream::finalize;
            document::view view{filter.view()};
            document::element ele{view["$hint"]};
            REQUIRE(ele);
            REQUIRE(ele.type() == type::k_document);
            REQUIRE(ele.get_document().value == index_copy);
        }

        SECTION("Compares equal to matching index doc view or value") {
            REQUIRE(index_hint == index_copy);
            REQUIRE(index_hint == index_copy.view());
            REQUIRE(index_copy == index_hint);
            REQUIRE(index_copy.view() == index_hint);
        }

        SECTION("Does not equal non-matching index doc") {
            auto bad_doc = builder::stream::document{} << "totoro" << 1
                                                       << builder::stream::finalize;
            REQUIRE(index_hint != bad_doc);
            REQUIRE(bad_doc != index_hint);
        }

        SECTION("Does not equal index string") {
            REQUIRE(index_hint != "totoro_1");
            REQUIRE("a" != index_hint);
        }
    }

    SECTION("Can be constructed with index document view") {
        auto index_doc = builder::stream::document{} << "a" << 1 << builder::stream::finalize;
        hint index_hint{index_doc.view()};

        SECTION("Compares equal to matching index doc view or value") {
            REQUIRE(index_hint == index_doc);
            REQUIRE(index_hint == index_doc.view());
            REQUIRE(index_doc == index_hint);
            REQUIRE(index_doc.view() == index_hint);
        }
    }
}
