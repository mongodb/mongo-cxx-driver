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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

namespace {
using namespace bsoncxx;

using bsoncxx::to_json;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

using namespace bsoncxx::types;

TEST_CASE("types::bson_value::value", "[bsoncxx::types::bson_value::value]") {
    auto doc_value = make_document(kvp("hello", "world"));
    auto doc2_value = make_document(kvp("a", 1));

    auto doc = doc_value.view();
    auto doc2 = doc2_value.view();

    auto elem = doc["hello"];
    auto elem2 = doc2["a"];

    SECTION("can be constructed by a document::element") {
        bson_value::value value = elem.get_owning_value();

        SECTION("can create new views") {
            bson_value::view view{value};
            bson_value::view new_view{value};
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
