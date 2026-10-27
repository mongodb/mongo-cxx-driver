// Copyright 2009-present MongoDB, Inc.
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

#include <bsoncxx/v1/types/value.hh>

#include <chrono>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/decimal128.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/types/view.hpp>

#include <bsoncxx/test/catch.hh>

namespace {
using namespace bsoncxx;

using bsoncxx::to_json;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

using namespace bsoncxx::types;

namespace {

void value_construction_test(bson_value::view test_view) {
    bson_value::value test_value{test_view};

    REQUIRE(test_value == test_view);
}

template <typename T, typename U>
void coverting_construction_test(T actual, U expected) {
    bson_value::value copy_initialization = actual;
    REQUIRE(copy_initialization == expected);

    bson_value::value direct_initialization(actual);
    REQUIRE(direct_initialization == expected);

    bson_value::value _static_cast = static_cast<bson_value::value>(actual);
    REQUIRE(_static_cast == expected);
}

} // namespace

TEST_CASE("types::bson_value::value", "[bsoncxx::types::bson_value::value]") {
    SECTION("can be constructed from") {
        SECTION("bool") {
            auto test_value = bson_value::make_value(b_bool{true});
            value_construction_test(test_value.view());

            coverting_construction_test(true, test_value);
            coverting_construction_test(b_bool{true}, test_value);
        }

        SECTION("string") {
            std::string value = "super duper";

            auto test_doc = bson_value::make_value(value);
            value_construction_test(test_doc.view());

            coverting_construction_test(value, test_doc);
            coverting_construction_test(value.c_str(), test_doc);
            coverting_construction_test(b_string{value}, test_doc);
            coverting_construction_test(stdx::string_view{value}, test_doc);

            char const raw_data[]{'s', 'u', 'p', 'e', 'r', ' ', 'd', 'u', 'p', 'e', 'r'};
            coverting_construction_test(stdx::string_view{raw_data, 11}, test_doc);

            auto test_empty = bson_value::make_value("");
            value_construction_test(test_empty.view());
            coverting_construction_test("", test_empty);

            auto nulls = "a\0\0\0";
            auto test_nulls = bson_value::make_value(nulls);
            value_construction_test(test_nulls.view());
            coverting_construction_test(nulls, test_nulls);
        }

        SECTION("double") {
            auto lower_bound = std::numeric_limits<double>::min();
            auto test_doc = bson_value::make_value(b_double{lower_bound});
            value_construction_test(test_doc.view());

            coverting_construction_test(lower_bound, test_doc);
            coverting_construction_test(b_double{lower_bound}, test_doc);
        }

        SECTION("int32") {
            auto lower_bound = std::numeric_limits<int32_t>::min();
            auto test_doc = bson_value::make_value(b_int32{lower_bound});
            value_construction_test(test_doc.view());

            coverting_construction_test(lower_bound, test_doc);
            coverting_construction_test(b_int32{lower_bound}, test_doc);
        }

        SECTION("int64") {
            auto lower_bound = std::numeric_limits<int64_t>::min();
            auto test_doc = bson_value::make_value(b_int64{lower_bound});
            value_construction_test(test_doc.view());

            coverting_construction_test(lower_bound, test_doc);
            coverting_construction_test(b_int64{lower_bound}, test_doc);
        }

        SECTION("undefined") {
            auto test_doc = bson_value::make_value(b_undefined{});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_undefined{}, test_doc);
            REQUIRE(bson_value::value(type::k_undefined) == test_doc);
        }

        SECTION("oid") {
            oid id{"0123456789abcdefABCDEFFF"};
            auto test_doc = bson_value::make_value(b_oid{id});
            value_construction_test(test_doc.view());

            coverting_construction_test(id, test_doc);
            coverting_construction_test(b_oid{id}, test_doc);
        }

        SECTION("decimal128") {
            uint64_t high = std::numeric_limits<uint64_t>::max();
            uint64_t low = std::numeric_limits<uint64_t>::min();

            auto test_doc = bson_value::make_value(b_decimal128{decimal128{high, low}});
            value_construction_test(test_doc.view());

            coverting_construction_test(decimal128{high, low}, test_doc);
            coverting_construction_test(b_decimal128{decimal128{high, low}}, test_doc);
            REQUIRE(bson_value::value(type::k_decimal128, high, low) == test_doc);
        }

        SECTION("date") {
            auto test_doc = bson_value::make_value(b_date(std::chrono::milliseconds(123456789)));
            value_construction_test(test_doc.view());

            coverting_construction_test(std::chrono::milliseconds(123456789), test_doc);
            coverting_construction_test(b_date(std::chrono::milliseconds(123456789)), test_doc);
        }

        SECTION("null") {
            auto test_doc = bson_value::make_value(b_null{});
            value_construction_test(test_doc.view());

            coverting_construction_test(nullptr, test_doc);
            coverting_construction_test(b_null{}, test_doc);
        }

        SECTION("regex") {
            auto regex = "amy";
            /* options are sorted and any duplicate or invalid options are removed */
            auto options = "imsx";

            auto test_doc = bson_value::make_value(b_regex{regex, options});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_regex{regex, options}, test_doc);
            REQUIRE(bson_value::value(regex, options) == test_doc);

            auto empty_regex = bson_value::make_value(b_regex{""});
            value_construction_test(empty_regex.view());

            coverting_construction_test(b_regex{""}, empty_regex);
            REQUIRE(bson_value::value(type::k_regex, "") == empty_regex);
            REQUIRE(bson_value::value("", "") == empty_regex);
        }

        SECTION("dbpointer") {
            oid id{"0123456789abcdefABCDEFFF"};
            auto coll_name = "collection";

            auto test_doc = bson_value::make_value(b_dbpointer{coll_name, id});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_dbpointer{coll_name, id}, test_doc);
            REQUIRE(bson_value::value(coll_name, id) == test_doc);

            auto empty_oid = oid{};
            auto empty_collection = bson_value::make_value(b_dbpointer{"", empty_oid});
            value_construction_test(empty_collection.view());

            coverting_construction_test(b_dbpointer{"", empty_oid}, empty_collection);
            REQUIRE(bson_value::value("", empty_oid) == empty_collection);
        }

        SECTION("code") {
            auto code = "look at me I'm some JS code";
            auto test_doc = bson_value::make_value(b_code{code});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_code{code}, test_doc);
            REQUIRE(bson_value::value(type::k_code, code) == test_doc);

            auto empty_code = bson_value::make_value(b_code{""});
            value_construction_test(empty_code.view());
            REQUIRE(bson_value::value(type::k_code, "") == empty_code);
        }

        SECTION("codewscope") {
            auto doc = make_document(kvp("a", "b"));
            auto code = "it's me, Code with Scope";
            auto test_doc = bson_value::make_value(b_codewscope{code, doc.view()});

            value_construction_test(test_doc.view());
            coverting_construction_test(b_codewscope{code, doc.view()}, test_doc);
            REQUIRE(bson_value::value(code, doc.view()) == test_doc);

            auto empty_doc = make_document(kvp("a", ""));
            auto empty_code = bson_value::make_value(b_codewscope{"", empty_doc.view()});
            value_construction_test(empty_code.view());

            coverting_construction_test(b_codewscope{"", empty_doc.view()}, empty_code);
            REQUIRE(bson_value::value("", empty_doc.view()) == empty_code);
        }

        SECTION("minkey") {
            auto test_doc = bson_value::make_value(b_minkey{});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_minkey{}, test_doc);
            REQUIRE(bson_value::value(type::k_minkey) == test_doc);
        }

        SECTION("maxkey") {
            auto test_doc = bson_value::make_value(b_maxkey{});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_maxkey{}, test_doc);
            REQUIRE(bson_value::value(type::k_maxkey) == test_doc);
        }

        SECTION("binary") {
            std::vector<uint8_t> bin{'d', 'e', 'a', 'd', 'b', 'e', 'e', 'f'};
            auto test_doc = bson_value::make_value(
                b_binary{binary_sub_type::k_binary, static_cast<uint32_t>(bin.size()), bin.data()});
            value_construction_test(test_doc.view());

            coverting_construction_test(bin, test_doc);
            REQUIRE(
                bson_value::value(b_binary{binary_sub_type::k_binary, static_cast<uint32_t>(bin.size()), bin.data()}) ==
                test_doc);
            REQUIRE(bson_value::value(bin.data(), bin.size(), binary_sub_type::k_binary) == test_doc);
            REQUIRE(bson_value::value(bin.data(), bin.size()) == test_doc);

            auto empty = bson_value::make_value(b_binary{});
            coverting_construction_test(std::vector<unsigned char>{}, empty);
        }

        SECTION("symbol") {
            auto symbol = "some symbol";
            auto test_doc = bson_value::make_value(b_symbol{symbol});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_symbol{symbol}, test_doc);
            REQUIRE(bson_value::value(type::k_symbol, symbol) == test_doc);

            auto empty_symbol = bson_value::make_value(b_symbol{""});
            value_construction_test(empty_symbol.view());

            REQUIRE(bson_value::value(type::k_symbol, "") == empty_symbol);
        }

        SECTION("timestamp") {
            uint32_t inc = std::numeric_limits<uint32_t>::max();
            uint32_t time = std::numeric_limits<uint32_t>::max() - 1;

            auto test_doc = bson_value::make_value(b_timestamp{inc, time});
            value_construction_test(test_doc.view());

            coverting_construction_test(b_timestamp{inc, time}, test_doc);
            REQUIRE(bson_value::value(type::k_timestamp, inc, time) == test_doc);
        }

        SECTION("document") {
            auto doc = make_document(kvp("a", 1));
            auto test_doc = bson_value::make_value(doc.view());
            value_construction_test(test_doc.view());

            coverting_construction_test(b_document{doc.view()}, test_doc);
            REQUIRE(bson_value::value(doc.view()) == test_doc);

            auto empty_doc = bson_value::make_value(document::view{});
            value_construction_test(empty_doc.view());

            coverting_construction_test(b_document{}, empty_doc);
            coverting_construction_test(document::view{}, empty_doc);
        }

        SECTION("array") {
            auto arr = make_array(make_document(kvp("hi", 0)));
            auto test_doc = bson_value::make_value(arr.view());
            value_construction_test(test_doc.view());

            coverting_construction_test(b_array{arr.view()}, test_doc);
            coverting_construction_test(arr.view(), test_doc);

            auto empty_doc = bson_value::make_value(array::view{});
            value_construction_test(empty_doc.view());

            coverting_construction_test(b_array{}, empty_doc);
            coverting_construction_test(array::view{}, empty_doc);
        }
    }

    SECTION("can be constructed by a document::element") {
        auto doc_value = make_document(kvp("hello", "world"));
        auto doc2_value = make_document(kvp("a", 1));

        auto doc = doc_value.view();
        auto doc2 = doc2_value.view();

        auto elem = doc["hello"];
        auto elem2 = doc2["a"];

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

        SECTION("can be copy assign") {
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

        SECTION("can be move assign") {
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

TEST_CASE("types::bson_value::value::get_*", "[bsoncxx::types::bson_value::value]") {
    bson_value::value const values[] = {
        b_minkey{},
        b_double{1.0},
        b_string{"abc"},
        b_document{make_document(kvp("x", 1))},
        b_array{make_array(1, 2, 3)},
        b_binary{},
        b_undefined{},
        b_oid{oid{"507f1f77bcf86cd799439011"}},
        b_bool{},
        b_date{std::chrono::milliseconds{123456789}},
        b_null{},
        b_regex{"abc", "i"},
        b_dbpointer{"dbpointer", oid{"507f1f77bcf86cd799439011"}},
        b_code{"code"},
        b_symbol{"symbol"},
        b_codewscope{"code", make_document(kvp("x", 1))},
        b_int32{},
        b_timestamp{},
        b_int64{},
        b_decimal128{decimal128{}},
        b_maxkey{},
    };

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                  \
    case type::k_##_name: {                               \
        auto const view = value.view();                   \
        CHECK(value.type_id() == type::k_##_name);        \
        CHECK(value.get_##_name() == view.get_##_name()); \
        break;                                            \
    }

    for (auto const& value : values) {
        switch (value.type_id()) { BSONCXX_V1_TYPES_XMACRO(X) }
    }
#undef X
#pragma pop_macro("X")
}

TEST_CASE("empty string-like BSON type values", "[bsoncxx::types::bson_value::value]") {
    // An empty string must never be represented as a null pointer.
#define CHECK_EMPTY_STRING(sv)          \
    if (1) {                            \
        auto const _sv = (sv);          \
        CHECK(_sv.size() == 0u);        \
        REQUIRE(_sv.data() != nullptr); \
        CHECK(_sv.data()[0] == '\0');   \
    } else                              \
        ((void)0)

    SECTION("string constructors") {
        CHECK_EMPTY_STRING(bson_value::value{""}.view().get_string().value);
        CHECK_EMPTY_STRING(bson_value::value{std::string{}}.view().get_string().value);
        CHECK_EMPTY_STRING(bson_value::value{stdx::string_view{""}}.view().get_string().value);
    }

    SECTION("b_string") {
        bson_value::value const v{b_string{stdx::string_view{}}};
        CHECK_EMPTY_STRING(v.view().get_string().value);

        bson_value::value const copy = v;
        CHECK_EMPTY_STRING(copy.view().get_string().value);

        bson_value::value assign;
        assign = v;
        CHECK_EMPTY_STRING(assign.view().get_string().value);
    }

    SECTION("b_symbol") {
        bson_value::value const v{b_symbol{stdx::string_view{}}};
        CHECK_EMPTY_STRING(v.view().get_symbol().symbol);

        bson_value::value const copy = v;
        CHECK_EMPTY_STRING(copy.view().get_symbol().symbol);

        bson_value::value assign;
        assign = v;
        CHECK_EMPTY_STRING(assign.view().get_symbol().symbol);
    }

    SECTION("b_code") {
        bson_value::value const v{b_code{stdx::string_view{}}};
        CHECK_EMPTY_STRING(v.view().get_code().code);

        bson_value::value const copy = v;
        CHECK_EMPTY_STRING(copy.view().get_code().code);

        bson_value::value assign;
        assign = v;
        CHECK_EMPTY_STRING(assign.view().get_code().code);
    }

    SECTION("b_regex") {
        bson_value::value const v{b_regex{stdx::string_view{}, stdx::string_view{}}};
        auto const r = v.view().get_regex();
        CHECK_EMPTY_STRING(r.regex);
        CHECK_EMPTY_STRING(r.options);

        bson_value::value const copy = v;
        auto const rc = copy.view().get_regex();
        CHECK_EMPTY_STRING(rc.regex);
        CHECK_EMPTY_STRING(rc.options);

        bson_value::value assign;
        assign = v;
        auto const ra = assign.view().get_regex();
        CHECK_EMPTY_STRING(ra.regex);
        CHECK_EMPTY_STRING(ra.options);
    }

    SECTION("b_dbpointer") {
        oid const o{"507f1f77bcf86cd799439011"};
        bson_value::value const v{b_dbpointer{stdx::string_view{}, o}};
        CHECK_EMPTY_STRING(v.view().get_dbpointer().collection);

        bson_value::value const copy = v;
        CHECK_EMPTY_STRING(copy.view().get_dbpointer().collection);

        bson_value::value assign;
        assign = v;
        CHECK_EMPTY_STRING(assign.view().get_dbpointer().collection);
    }

    SECTION("b_codewscope") {
        auto const scope = make_document();
        bson_value::value const v{b_codewscope{stdx::string_view{}, scope.view()}};
        CHECK_EMPTY_STRING(v.view().get_codewscope().code);

        bson_value::value const copy = v;
        CHECK_EMPTY_STRING(copy.view().get_codewscope().code);

        bson_value::value assign;
        assign = v;
        CHECK_EMPTY_STRING(assign.view().get_codewscope().code);
    }

    SECTION("from type view") {
        bson_value::value const v{bson_value::view{b_string{stdx::string_view{}}}};
        CHECK_EMPTY_STRING(v.view().get_string().value);
    }

    SECTION("from element") {
        auto const scope = make_document();
        auto const doc = make_document(
            kvp("string", b_string{stdx::string_view{""}}),
            kvp("symbol", b_symbol{stdx::string_view{""}}),
            kvp("code", b_code{stdx::string_view{""}}),
            kvp("regex", b_regex{stdx::string_view{""}, stdx::string_view{""}}),
            kvp("dbpointer", b_dbpointer{stdx::string_view{""}, oid{"507f1f77bcf86cd799439011"}}),
            kvp("codewscope", b_codewscope{stdx::string_view{""}, scope.view()}));

        CHECK_EMPTY_STRING(doc["string"].get_string().value);
        CHECK_EMPTY_STRING(doc["string"].get_value().get_string().value);

        CHECK_EMPTY_STRING(doc["symbol"].get_symbol().symbol);
        CHECK_EMPTY_STRING(doc["code"].get_code().code);

        auto const r = doc["regex"].get_regex();
        CHECK_EMPTY_STRING(r.regex);
        CHECK_EMPTY_STRING(r.options);

        CHECK_EMPTY_STRING(doc["dbpointer"].get_dbpointer().collection);
        CHECK_EMPTY_STRING(doc["codewscope"].get_codewscope().code);
    }

#undef CHECK_EMPTY_STRING
}

TEST_CASE("empty binary-like BSON type values", "[bsoncxx::types::bson_value::value]") {
    oid const o{"507f1f77bcf86cd799439011"};
    auto const scope = make_document();

    SECTION("document") {
        bson_value::value const v{b_document{document::view{}}};
        CHECK(v.view().get_document().value.data() != nullptr);
        CHECK(v.view().get_document().value.size() == 5u);

        bson_value::value const copy = v;
        CHECK(copy.view().get_document().value == v.view().get_document().value);
        CHECK(copy.view().get_document().value.size() == v.view().get_document().value.size());
    }

    SECTION("array") {
        bson_value::value const v{b_array{array::view{}}};
        CHECK(v.view().get_array().value.data() != nullptr);
        CHECK(v.view().get_array().value.size() == 5u);
    }

    SECTION("binary") {
        std::uint8_t const data = {};
        bson_value::value const v{b_binary{binary_sub_type::k_binary, 0u, &data}};
        CHECK(v.view().get_binary().bytes == nullptr);
        CHECK(v.view().get_binary().size == 0u);

        bson_value::value const copy = v;
        CHECK(copy.view().get_binary().bytes == nullptr);
        CHECK(copy.view().get_binary().size == 0u);
    }

    SECTION("codewscope") {
        bson_value::value const v{b_codewscope{stdx::string_view{}, scope.view()}};
        CHECK(v.view().get_codewscope().code.data() != nullptr); // String is never null.
        CHECK(v.view().get_codewscope().scope == scope.view());

        bson_value::value const copy = v;
        CHECK(copy.view().get_codewscope().code.data() != nullptr);
        CHECK(copy.view().get_codewscope().scope == scope.view());
        CHECK(copy.view().get_codewscope().scope.size() == scope.view().size());
    }

    SECTION("dbpointer") {
        bson_value::value const v{b_dbpointer{stdx::string_view{}, o}};
        CHECK(v.view().get_dbpointer().collection.data() != nullptr); // String is never null.
        CHECK(v.view().get_dbpointer().value == o);

        bson_value::value const copy = v;
        CHECK(copy.view().get_dbpointer().collection.data() != nullptr);
        CHECK(copy.view().get_dbpointer().value == o);
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][types][view]") {
    using v1 = v1::types::view;
    using v_noabi = v_noabi::types::view;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    SECTION("from_v1") {
        v1 from{bsoncxx::v1::types::b_string{"hello"}};
        v_noabi const to = from_v1(from);

        CHECK(to == to_v1(to));
        CHECK(to == from);
        CHECK(to != v1{});
    }

    SECTION("to_v1") {
        v_noabi from{b_string{"hello"}};
        v1 const to = to_v1(from);

        CHECK(to == to_v1(from_v1(to)));
        CHECK(from == from_v1(to));
        CHECK(to != v1{bsoncxx::v1::types::b_string{"goodbye"}});
    }

    SECTION("empty string") {
#define CHECK_EMPTY_STRING(sv)          \
    if (1) {                            \
        auto const _sv = (sv);          \
        CHECK(_sv.size() == 0u);        \
        REQUIRE(_sv.data() != nullptr); \
        CHECK(_sv.data()[0] == '\0');   \
    } else                              \
        ((void)0)

#define CHECK_NULL_STRING(sv)         \
    if (1) {                          \
        auto const _sv = (sv);        \
        CHECK(_sv.size() == 0u);      \
        CHECK(_sv.data() == nullptr); \
    } else                            \
        ((void)0)

        oid const o{"507f1f77bcf86cd799439011"};
        auto const scope = make_document();

        SECTION("from_v1") {
            // An empty string value in v1 is always represented as a non-null empty string in v_noabi.
            v1 const from{bsoncxx::v1::types::b_string{}};
            v_noabi const to = from_v1(from);

            // types::view
            CHECK_EMPTY_STRING(to.get_string().value);

            // types::b_*
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_string{}).value);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_symbol{}).symbol);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_code{}).code);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_regex{}).regex);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_regex{}).options);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_dbpointer{}).collection);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_codewscope{}).code);

            // The conversions preserve value equality for all BSON type values.
            v1 const views[] = {
                v1{bsoncxx::v1::types::b_string{}},
                v1{bsoncxx::v1::types::b_document{}},
                v1{bsoncxx::v1::types::b_array{}},
                v1{bsoncxx::v1::types::b_binary{}},
                v1{bsoncxx::v1::types::b_regex{}},
                v1{bsoncxx::v1::types::b_dbpointer{}},
                v1{bsoncxx::v1::types::b_code{}},
                v1{bsoncxx::v1::types::b_symbol{}},
                v1{bsoncxx::v1::types::b_codewscope{}},
            };

#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case bsoncxx::v1::types::id::k_##_name: \
        CHECK(to_v1(from_v1(v)) == v);      \
        break;

            for (auto const& v : views) {
                switch (v.type_id()) {
                    BSONCXX_V1_TYPES_XMACRO(X)

                    default:
                        FAIL("should not reach this point");
                }
            }
#pragma pop_macro("X")
        }

        SECTION("to_v1") {
            // An empty string value in v_noabi is represented as a default-initialized string view in v1.
            v_noabi const from_null{b_string{stdx::string_view{}}};
            v1 const to_null = to_v1(from_null);

            CHECK_NULL_STRING(to_null.get_string().value);

            v_noabi const from_empty{b_string{stdx::string_view{""}}};
            v1 const to_empty = to_v1(from_empty);

            CHECK_NULL_STRING(to_empty.get_string().value);

            CHECK_NULL_STRING(to_v1(b_string{stdx::string_view{}}).value);
            CHECK_NULL_STRING(to_v1(b_string{stdx::string_view{""}}).value);
            CHECK_NULL_STRING(to_v1(b_symbol{stdx::string_view{""}}).symbol);
            CHECK_NULL_STRING(to_v1(b_code{stdx::string_view{""}}).code);
            CHECK_NULL_STRING(to_v1(b_regex{stdx::string_view{}, stdx::string_view{""}}).regex);
            CHECK_NULL_STRING(to_v1(b_regex{stdx::string_view{}, stdx::string_view{""}}).options);
            CHECK_NULL_STRING(to_v1(b_dbpointer{stdx::string_view{}, o}).collection);
            CHECK_NULL_STRING(to_v1(b_codewscope{stdx::string_view{}, scope.view()}).code);

            CHECK(to_v1(b_regex{stdx::string_view{}, stdx::string_view{"abc"}}).options == "abc");
            CHECK(to_v1(b_dbpointer{stdx::string_view{}, o}).value == to_v1(o));
            CHECK(to_v1(b_codewscope{stdx::string_view{}, scope.view()}).scope == to_v1(scope.view()));

            // The conversions preserve value equality for all BSON type values.
            v_noabi const views[] = {
                v_noabi{b_string{stdx::string_view{""}}},
                v_noabi{b_document{document::view{}}},
                v_noabi{b_array{array::view{}}},
                v_noabi{b_binary{}},
                v_noabi{b_regex{stdx::string_view{""}, stdx::string_view{""}}},
                v_noabi{b_dbpointer{stdx::string_view{""}, o}},
                v_noabi{b_code{stdx::string_view{""}}},
                v_noabi{b_symbol{stdx::string_view{""}}},
                v_noabi{b_codewscope{stdx::string_view{""}, scope.view()}},
            };

#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case bsoncxx::v_noabi::type::k_##_name: \
        CHECK(from_v1(to_v1(v)) == v);      \
        break;

            for (auto const& v : views) {
                switch (v.type_id()) {
                    BSONCXX_V1_TYPES_XMACRO(X)

                    default:
                        FAIL("should not reach this point");
                }
            }
#pragma pop_macro("X")
        }

#undef CHECK_EMPTY_STRING
#undef CHECK_NULL_STRING
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][types][value]") {
    using v1 = v1::types::value;
    using v_noabi = v_noabi::types::value;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    SECTION("from_v1") {
        v1 from{bsoncxx::v1::types::b_string{"hello"}};
        v_noabi const to = from_v1(from);

        CHECK(to == from_v1(to_v1(to)));
        CHECK(to == from_v1(from));
        CHECK(to != v_noabi{});
    }

    SECTION("to_v1") {
        v_noabi from{b_string{"hello"}};
        v1 const to = to_v1(from);

        CHECK(to == to_v1(from));
        CHECK(from == from_v1(to));
        CHECK(to != v1{});
    }

    SECTION("empty string") {
        oid const o{"507f1f77bcf86cd799439011"};
        auto const scope = make_document();

        // An empty string-like value is represented as a not-null empty string in v_noabi...
#define CHECK_EMPTY_STRING(sv)          \
    if (1) {                            \
        auto const _sv = (sv);          \
        CHECK(_sv.size() == 0u);        \
        REQUIRE(_sv.data() != nullptr); \
        CHECK(_sv.data()[0] == '\0');   \
    } else                              \
        ((void)0)

        // ...and as a default-initialized (null) string view in v1.
#define CHECK_NULL_STRING(sv)         \
    if (1) {                          \
        auto const _sv = (sv);        \
        CHECK(_sv.size() == 0u);      \
        CHECK(_sv.data() == nullptr); \
    } else                            \
        ((void)0)

        SECTION("from_v1") {
            // An empty string value in v1 is always represented as a non-null empty string in v_noabi.
            v1 const from{bsoncxx::v1::types::b_string{}};
            v_noabi const to = from_v1(from);

            // types::value
            CHECK_EMPTY_STRING(to.view().get_string().value);

            // types::b_*
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_string{}}).view().get_string().value);
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_symbol{}}).view().get_symbol().symbol);
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_code{}}).view().get_code().code);
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_regex{}}).view().get_regex().regex);
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_regex{}}).view().get_regex().options);
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_dbpointer{}}).view().get_dbpointer().collection);
            CHECK_EMPTY_STRING(from_v1(v1{bsoncxx::v1::types::b_codewscope{}}).view().get_codewscope().code);
            CHECK_EMPTY_STRING(from_v1(bsoncxx::v1::types::b_string{}).value);

            // The conversions preserve value equality for all BSON type values.
            v1 const views[] = {
                v1{bsoncxx::v1::types::b_string{}},
                v1{bsoncxx::v1::types::b_document{}},
                v1{bsoncxx::v1::types::b_array{}},
                v1{bsoncxx::v1::types::b_binary{}},
                v1{bsoncxx::v1::types::b_regex{}},
                v1{bsoncxx::v1::types::b_dbpointer{}},
                v1{bsoncxx::v1::types::b_code{}},
                v1{bsoncxx::v1::types::b_symbol{}},
                v1{bsoncxx::v1::types::b_codewscope{}},
            };

#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case bsoncxx::v1::types::id::k_##_name: \
        CHECK(to_v1(from_v1(v)) == v);      \
        break;

            for (auto const& v : views) {
                switch (v.type_id()) {
                    BSONCXX_V1_TYPES_XMACRO(X)

                    default:
                        FAIL("should not reach this point");
                }
            }
#pragma pop_macro("X")
        }

        SECTION("to_v1") {
            // An empty string value in v_noabi is represented as a default-initialized string view in v1.
            v_noabi const from_null{b_string{stdx::string_view{}}};
            v1 const to_null = to_v1(from_null);

            CHECK_NULL_STRING(to_null.get_string().value);

            v_noabi const from_empty{b_string{stdx::string_view{""}}};
            v1 const to_empty = to_v1(from_empty);

            CHECK_NULL_STRING(to_empty.get_string().value);

            CHECK_NULL_STRING(to_v1(v_noabi{b_string{stdx::string_view{""}}}).get_string().value);
            CHECK_NULL_STRING(to_v1(v_noabi{b_symbol{stdx::string_view{""}}}).get_symbol().symbol);
            CHECK_NULL_STRING(to_v1(v_noabi{b_code{stdx::string_view{""}}}).get_code().code);
            CHECK_NULL_STRING(to_v1(v_noabi{b_regex{stdx::string_view{}, stdx::string_view{""}}}).get_regex().regex);
            CHECK_NULL_STRING(to_v1(v_noabi{b_regex{stdx::string_view{}, stdx::string_view{""}}}).get_regex().options);
            CHECK_NULL_STRING(to_v1(v_noabi{b_dbpointer{stdx::string_view{}, o}}).get_dbpointer().collection);
            CHECK_NULL_STRING(to_v1(v_noabi{b_codewscope{stdx::string_view{}, scope.view()}}).get_codewscope().code);

            // Non-empty fields of multi-field types are preserved, not canonicalized.
            CHECK(to_v1(v_noabi{b_regex{stdx::string_view{}, stdx::string_view{"abc"}}}).get_regex().options == "abc");
            CHECK(to_v1(v_noabi{b_dbpointer{stdx::string_view{}, o}}).get_dbpointer().value == to_v1(o));
            CHECK(
                to_v1(v_noabi{b_codewscope{stdx::string_view{}, scope.view()}}).get_codewscope().scope ==
                to_v1(scope.view()));

            // The conversions preserve value equality for all BSON type values.
            v_noabi const views[] = {
                v_noabi{b_string{stdx::string_view{""}}},
                v_noabi{b_document{document::view{}}},
                v_noabi{b_array{array::view{}}},
                v_noabi{b_binary{}},
                v_noabi{b_regex{stdx::string_view{""}, stdx::string_view{""}}},
                v_noabi{b_dbpointer{stdx::string_view{""}, o}},
                v_noabi{b_code{stdx::string_view{""}}},
                v_noabi{b_symbol{stdx::string_view{""}}},
                v_noabi{b_codewscope{stdx::string_view{""}, scope.view()}},
            };

#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case bsoncxx::v_noabi::type::k_##_name: \
        CHECK(from_v1(to_v1(v)) == v);      \
        break;

            for (auto const& v : views) {
                switch (v.type_id()) {
                    BSONCXX_V1_TYPES_XMACRO(X)

                    default:
                        FAIL("should not reach this point");
                }
            }
#pragma pop_macro("X")
        }

#undef CHECK_EMPTY_STRING
#undef CHECK_NULL_STRING
    }
}

} // namespace
