// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/make_value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/libbson.hh>

namespace {
using namespace bsoncxx;

using bsoncxx::to_json;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

using namespace bsoncxx::types;

namespace {

void value_construction_test(bson_value::view test_view) {
    bson_value::value test_value{test_view};

    REQUIRE(test_value == test_view);
}
}  // namespace

TEST_CASE("types::bson_value::value", "[bsoncxx::types::bson_value::value]") {
    auto doc_value = make_document(kvp("hello", "world"));
    auto doc2_value = make_document(kvp("a", 1));

    auto doc = doc_value.view();
    auto doc2 = doc2_value.view();

    auto elem = doc["hello"];
    auto elem2 = doc2["a"];

    SECTION("can be constructed from a bson_value::view") {
        SECTION("bool") {
            auto test_doc = bson_value::make_value(types::b_bool{true});
            value_construction_test(test_doc.view());
        }

        SECTION("utf8") {
            auto test_doc = bson_value::make_value("super duper");
            value_construction_test(test_doc.view());

            auto test_empty = bson_value::make_value("");
            value_construction_test(test_empty.view());

            auto test_nulls = bson_value::make_value("a\0\0\0");
            value_construction_test(test_nulls.view());
        }

        SECTION("double") {
            auto test_doc = bson_value::make_value(types::b_double{12});
            value_construction_test(test_doc.view());
        }

        SECTION("int32") {
            auto test_doc = bson_value::make_value(types::b_int32{42});
            value_construction_test(test_doc.view());
        }

        SECTION("int64") {
            auto test_doc = bson_value::make_value(types::b_int64{72});
            value_construction_test(test_doc.view());
        }

        SECTION("undefined") {
            auto test_doc = bson_value::make_value(types::b_undefined{});
            value_construction_test(test_doc.view());
        }

        SECTION("undefined") {
            auto test_doc = bson_value::make_value(types::b_undefined{});
            value_construction_test(test_doc.view());
        }

        SECTION("oid") {
            auto test_doc = bson_value::make_value(types::b_oid{});
            value_construction_test(test_doc.view());
        }

        SECTION("decimal128") {
            auto test_doc = bson_value::make_value(types::b_decimal128{decimal128{4, 4}});
            value_construction_test(test_doc.view());
        }

        SECTION("date") {
            auto test_doc =
                bson_value::make_value(types::b_date(std::chrono::milliseconds(123456789)));
            value_construction_test(test_doc.view());
        }

        SECTION("null") {
            auto test_doc = bson_value::make_value(types::b_null{});
            value_construction_test(test_doc.view());
        }

        SECTION("regex") {
            auto test_doc = bson_value::make_value(types::b_regex{"amy", "no options"});
            value_construction_test(test_doc.view());

            auto empty_regex = bson_value::make_value(types::b_regex{"", ""});
            value_construction_test(empty_regex.view());
        }

        SECTION("dbpointer") {
            auto test_doc = bson_value::make_value(types::b_dbpointer{"collection", oid{}});
            value_construction_test(test_doc.view());

            auto empty_collection = bson_value::make_value(types::b_dbpointer{"", oid{}});
            value_construction_test(empty_collection.view());
        }

        SECTION("code") {
            auto test_doc = bson_value::make_value(types::b_code{"look at me I'm some JS code"});
            value_construction_test(test_doc.view());

            auto empty_code = bson_value::make_value(types::b_code{""});
            value_construction_test(empty_code.view());
        }

        SECTION("codewscope") {
            auto doc = make_document(kvp("a", "b"));
            auto test_doc =
                bson_value::make_value(types::b_codewscope{"it's me, Code with Scope", doc.view()});
            value_construction_test(test_doc.view());

            auto empty_doc = make_document(kvp("a", ""));
            auto empty_code = bson_value::make_value(types::b_codewscope{"", empty_doc.view()});
            value_construction_test(empty_code.view());
        }

        SECTION("minkey") {
            auto test_doc = bson_value::make_value(types::b_minkey{});
            value_construction_test(test_doc.view());
        }

        SECTION("maxkey") {
            auto test_doc = bson_value::make_value(types::b_maxkey{});
            value_construction_test(test_doc.view());
        }

        SECTION("document") {
            auto doc = make_document(kvp("a", 1));
            auto test_doc = bson_value::make_value(doc.view());
            value_construction_test(test_doc.view());

            // Empty document
            test_doc = bson_value::make_value(document::view{});
            value_construction_test(test_doc.view());
        }

        SECTION("array") {
            auto arr = make_array(make_document(kvp("hi", 0)));
            auto test_doc = bson_value::make_value(arr.view());
            value_construction_test(test_doc.view());
        }
    }

    SECTION("can be constructed by a document::element") {
        bson_value::value value = elem.get_owning_value();

        SECTION("can create new views") {
            bson_value::view view(value);
            bson_value::view new_view(value);
            REQUIRE(view == new_view);
        }

        SECTION("can be copy constructed") {
            bson_value::value copied{value};
            REQUIRE(value == copied);
        }

        SECTION("can be copy assigned") {
            bson_value::value copied = elem2.get_owning_value();
            REQUIRE(value != copied);
            {
                bson_value::value temp = elem.get_owning_value();
                copied = temp;
            }
            REQUIRE(value == copied);
        }

        SECTION("can be move constructed") {
            bson_value::value temp = elem.get_owning_value();
            bson_value::value moved{std::move(temp)};
            REQUIRE(moved == value);
        }

        SECTION("can be move assigned") {
            bson_value::value moved = elem2.get_owning_value();
            {
                bson_value::value temp = elem.get_owning_value();
                moved = std::move(temp);
            }
            REQUIRE(moved == value);
        }

        SECTION("Owns its own memory buffer") {
            bson_value::value moved = elem2.get_owning_value();
            bson_value::value original = elem.get_owning_value();

            {
                // Matches "doc" but has distinct memory.
                auto tempdoc = make_document(kvp("hello", "world"));
                auto tempview = tempdoc.view();
                auto temp = tempview["hello"].get_owning_value();

                moved = std::move(temp);
            }

            REQUIRE(moved == original);
        }

        SECTION("Can be compared to another bson_value::value") {
            SECTION("Compares equal with equal views, regardless of ownership") {
                bson_value::value temp = elem.get_owning_value();
                bson_value::value a{std::move(temp)};
                bson_value::value b = elem.get_owning_value();

                REQUIRE(b == a);
            }

            SECTION("Compares inequal with different views") {
                bson_value::value a = elem.get_owning_value();
                bson_value::value b = elem2.get_owning_value();

                REQUIRE(a != b);
            }
        }

        SECTION("Can be compared to a bson_value::view") {
            auto bad_doc = make_document(kvp("blah", 1));
            auto bad_doc_view = bad_doc.view();
            auto bad_view = bad_doc_view["blah"].get_value();
            auto view = elem.get_value();

            bson_value::value a = elem.get_owning_value();

            REQUIRE(a == view);
            REQUIRE(view == a);
            REQUIRE(a != bad_view);
            REQUIRE(bad_view != a);
        }
    }
}

}  // namespace
