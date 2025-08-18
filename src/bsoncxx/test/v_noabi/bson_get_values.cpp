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

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/document/view.hh>

#include <utility>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>

#include <bsoncxx/test/catch.hh>

namespace {
using namespace bsoncxx;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

TEST_CASE("[] can reach into nested arrays", "[bsoncxx]") {
    // {
    //     "ints": [ 1, 3, [ 5 ] ],
    //     "bools": [ true, false ]
    // }
    auto build_doc = make_document(kvp("ints", make_array(1, 3, make_array(5))), kvp("bools", make_array(true, false)));
    auto doc = build_doc.view();

    SECTION("works with one level") {
        REQUIRE(doc["ints"][0]);
        REQUIRE(doc["ints"][0].get_int32() == 1);

        REQUIRE(doc["ints"][1]);
        REQUIRE(doc["ints"][1].get_int32() == 3);

        REQUIRE(doc["bools"][0]);
        REQUIRE(doc["bools"][0].get_bool() == true);

        REQUIRE(doc["bools"][1]);
        REQUIRE(doc["bools"][1].get_bool() == false);
    }

    SECTION("works with two levels") {
        REQUIRE(doc["ints"][2][0]);
        REQUIRE(doc["ints"][2][0].get_int32() == 5);
    }

    SECTION("returns invalid on out-of-bounds") {
        REQUIRE(!doc["ints"][9]);
        REQUIRE(!doc["ints"][2][9]);
        REQUIRE(!doc["bools"][9]);
    }

    SECTION("returns invalid on index access to non-array") {
        REQUIRE(!doc["bools"][0][1]);
    }

    SECTION("returns invalid for operator[] on invalid value") {
        REQUIRE(!doc["ints"][9][0]);
        REQUIRE(!doc["ints"][9]["missing"]);
    }
}

TEST_CASE("[] can reach into nested documents", "[bsoncxx]") {
    // {
    //     "ints": {
    //         "x": 1,
    //         "y": 3,
    //         "more": {
    //             "z": 5
    //         }
    //     },
    //     "bools": {
    //         "t": true,
    //         "f": false,
    //     }
    // }
    auto build_doc = make_document(
        kvp("ints", make_document(kvp("x", 1), kvp("y", 3), kvp("more", make_document(kvp("z", 5))))),
        kvp("bools", make_document(kvp("t", true), kvp("f", false))));

    auto doc = build_doc.view();

    SECTION("works with one level") {
        REQUIRE(doc["ints"]["x"]);
        REQUIRE(doc["ints"]["x"].get_int32() == 1);

        REQUIRE(doc["ints"]["y"]);
        REQUIRE(doc["ints"]["y"].get_int32() == 3);

        REQUIRE(doc["bools"]["t"]);
        REQUIRE(doc["bools"]["t"].get_bool() == true);

        REQUIRE(doc["bools"]["f"]);
        REQUIRE(doc["bools"]["f"].get_bool() == false);
    }

    SECTION("works with two levels") {
        REQUIRE(doc["ints"]["more"]["z"]);
        REQUIRE(doc["ints"]["more"]["z"].get_int32() == 5);
    }

    SECTION("returns invalid on not found") {
        REQUIRE(!doc["ints"]["badKey"]);
        REQUIRE(!doc["ints"]["more"]["badKey"]);
        REQUIRE(!doc["bools"]["badKey"]);
    }

    SECTION("returns invalid on key access to non-document") {
        REQUIRE(!doc["bools"]["t"]["missing"]);
    }

    SECTION("returns invalid for operator[] on invalid value") {
        REQUIRE(!doc["missing"]["deep"]);
        REQUIRE(!doc["missing"][0]);
    }
}

TEST_CASE("[] can reach into mixed nested arrays and documents", "[bsoncxx]") {
    // {
    //     "ints": {
    //         "x": 1,
    //         "y": 3,
    //         "arr": [
    //             5,
    //             7,
    //             {
    //                 "z": 9,
    //                 "even_more": [ 11 ]
    //             }
    //         ]
    //     },
    //     "bools": {
    //         "t": true,
    //         "f": false,
    //         "arr": [ false, true ]
    //     }
    // }
    auto build_doc = make_document(
        kvp("ints",
            make_document(
                kvp("x", 1),
                kvp("y", 3),
                kvp("arr", make_array(5, 7, make_document(kvp("z", 9), kvp("even_more", make_array(11))))))),
        kvp("bools", make_document(kvp("t", true), kvp("f", false), kvp("arr", make_array(false, true)))));

    auto doc = build_doc.view();

    SECTION("succeeds on ints") {
        REQUIRE(doc["ints"]["x"].get_int32() == 1);
        REQUIRE(doc["ints"]["y"].get_int32() == 3);
        REQUIRE(doc["ints"]["arr"][0].get_int32() == 5);
        REQUIRE(doc["ints"]["arr"][1].get_int32() == 7);
        REQUIRE(doc["ints"]["arr"][2]["z"].get_int32() == 9);
        REQUIRE(doc["ints"]["arr"][2]["even_more"][0].get_int32() == 11);
    }

    SECTION("succeeds on bools") {
        REQUIRE(doc["bools"]["t"].get_bool() == true);
        REQUIRE(doc["bools"]["f"].get_bool() == false);
        REQUIRE(doc["bools"]["arr"][0].get_bool() == false);
        REQUIRE(doc["bools"]["arr"][1].get_bool() == true);
    }
}

TEST_CASE("[] with large nesting levels", "[bsoncxx]") {
    std::int32_t nesting_level;

    SECTION("no nesting") {
        nesting_level = 0;
    }

    SECTION("2 nesting level") {
        nesting_level = 2;
    }

    SECTION("100 nesting level") {
        nesting_level = 100;
    }

    SECTION("200 nesting level") {
        nesting_level = 200;
    }

    SECTION("2000 nesting level") {
        nesting_level = 2000;
    }

    document::value nest = make_document(kvp("x", nesting_level));

    for (int i = 0; i < nesting_level; i++) {
        auto temp = make_document(kvp("x", nest));
        nest = std::move(temp);
    }

    auto x = nest.view()["x"];
    for (std::int32_t i = 0; i < nesting_level; ++i) {
        REQUIRE(x["x"]);
        x = x["x"];
    }
    REQUIRE(x.get_int32() == nesting_level);
}

TEST_CASE("empty document view has working iterators", "[bsoncxx]") {
    auto value = make_document();
    auto doc = value.view();

    REQUIRE(doc.begin() == doc.end());
    REQUIRE(doc.cbegin() == doc.cend());
}

TEST_CASE("document view begin/end/find give expected types", "[bsoncxx]") {
    auto value = make_document(kvp("a", 1));

    SECTION("const document::view gives const_iterator") {
        document::view const const_doc = value.view();

        document::view::const_iterator citer = const_doc.begin();
        REQUIRE(citer != const_doc.end());

        citer = const_doc.cbegin();
        REQUIRE(citer != const_doc.end());

        citer = const_doc.find("a");
        REQUIRE(citer == const_doc.begin());
    }

    SECTION("non-const document::view gives const_iterator") {
        document::view doc = value.view();

        document::view::const_iterator citer = doc.begin();
        REQUIRE(citer != doc.end());

        citer = doc.cbegin();
        REQUIRE(citer != doc.end());

        citer = doc.find("a");
        REQUIRE(citer == doc.begin());
    }

    SECTION("iterator is an alias for const_iterator") {
        document::view doc = value.view();

        document::view::iterator iter = doc.begin();
        REQUIRE(iter != doc.end());

        iter = doc.cbegin();
        REQUIRE(iter != doc.end());

        iter = doc.find("a");
        REQUIRE(iter == doc.begin());
    }
}

TEST_CASE("empty array view has working iterators", "[bsoncxx]") {
    auto value = make_document();
    auto ary = value.view();

    REQUIRE(ary.begin() == ary.end());
    REQUIRE(ary.cbegin() == ary.cend());
}

TEST_CASE("array view begin/end/find give expected types", "[bsoncxx]") {
    auto value = make_array("a");

    SECTION("const array::view gives const_iterator") {
        array::view const const_ary = value.view();

        array::view::const_iterator citer = const_ary.begin();
        REQUIRE(citer != const_ary.end());

        citer = const_ary.cbegin();
        REQUIRE(citer != const_ary.end());

        citer = const_ary.find(0);
        REQUIRE(citer == const_ary.begin());
    }

    SECTION("non-const array::view gives const_iterator") {
        array::view ary = value.view();

        array::view::const_iterator citer = ary.begin();
        REQUIRE(citer != ary.end());

        citer = ary.cbegin();
        REQUIRE(citer != ary.end());

        citer = ary.find(0);
        REQUIRE(citer == ary.begin());
    }

    SECTION("iterator is an alias for const_iterator") {
        array::view ary = value.view();

        array::view::iterator iter = ary.begin();
        REQUIRE(iter != ary.end());

        iter = ary.cbegin();
        REQUIRE(iter != ary.cend());

        iter = ary.find(0);
        REQUIRE(iter == ary.begin());
    }
}

TEST_CASE("CXX-1476: CXX-992 regression fixes", "[bsoncxx]") {
    SECTION("request for field 'o' does not return field 'op'") {
        constexpr auto k_json = R"({ "op" : 1, "o" : 2 })";
        auto const bson = from_json(k_json);
        REQUIRE(bson.view()["o"].key() == stdx::string_view("o"));
    }

    SECTION("empty key is not ignored") {
        constexpr auto k_json = R"({ "" : 1 })";
        auto const bson = from_json(k_json);
        REQUIRE(bson.view().find("") != bson.view().cend());
        REQUIRE(bson.view().find(stdx::string_view()) != bson.view().cend());
    }
}

TEST_CASE("CXX-1880: array element should have key") {
    bsoncxx::array::value val = make_array(0, 1, 2);
    REQUIRE(val.view()[0].key() == stdx::string_view("0"));
    REQUIRE(val.view()[1].key() == stdx::string_view("1"));
    REQUIRE(val.view()[2].key() == stdx::string_view("2"));
}

TEST_CASE("can use operator[] with document::value") {
    // {
    //     "beep": 25,
    //     "boop": {
    //         "test": true
    //     },
    //     "test_array": [5, 4, 3]
    // }
    auto doc = make_document(
        kvp("beep", 25), kvp("boop", make_document(kvp("test", true))), kvp("test_array", make_array(5, 4, 3)));
    auto view = doc.view();

    SECTION("operator[] can access valid keys") {
        REQUIRE(doc["beep"].get_int32() == view["beep"].get_int32());
        REQUIRE(doc["boop"]["test"].get_bool() == view["boop"]["test"].get_bool());
        REQUIRE(doc["test_array"][2].get_int32() == view["test_array"][2].get_int32());
    }

    SECTION("operator[] returns invalid for nonexistent key") {
        REQUIRE(!doc["not_a_key"]);
        REQUIRE(!doc["not_a_key"]["test"]);
        REQUIRE(!doc["test_array"][5]["not_a_key"]);
    }
}

TEST_CASE("document::values have a superset of document::view's methods") {
    document::value doc = make_document(kvp("int", 5), kvp("alice", "Bob"));
    document::view view = doc.view();

    SECTION("iterators provide the same results") {
        REQUIRE(doc.cbegin() == view.cbegin());
        REQUIRE(doc.cend() == view.cend());
        REQUIRE(doc.begin() == view.begin());
        REQUIRE(doc.end() == view.end());
        REQUIRE(doc.find("alice") == view.find("alice"));
    }

    SECTION("getters return the same results") {
        REQUIRE(doc.data() == view.data());
        REQUIRE(doc.length() == view.length());
    }

    SECTION("empty doc and empty view yield same results") {
        document::value empty_doc = make_document();
        document::view empty_view = empty_doc.view();

        REQUIRE(empty_doc.empty());
        REQUIRE(empty_view.empty());
        REQUIRE(!doc.empty());
        REQUIRE(!view.empty());
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][document][element]") {
    using v1 = v1::element::view;
    using v_noabi = v_noabi::document::element;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    auto const owner = bsoncxx::from_json(R"({"x": 1})");
    auto const doc = owner.view();
    auto const e = doc["x"];

    SECTION("from_v1") {
        v1 from{e};
        v_noabi const to = from;

        CHECK(to.raw() == to_v1(to).raw());
        CHECK(to.raw() == from.raw());
        CHECK(to.raw() != v1{}.raw());
    }

    SECTION("to_v1") {
        v_noabi from{e};
        v1 const to{from};

        CHECK(to.raw() == v_noabi{to}.raw()); // No from_v1().
        CHECK(to.raw() == from.raw());
        CHECK(to.raw() != v_noabi{}.raw());
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][document][view]") {
    using v1 = v1::document::view;
    using v_noabi = v_noabi::document::view;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    auto const owner = bsoncxx::from_json(R"({"x": 1})");
    auto const doc = owner.view();

    SECTION("from_v1") {
        v1 from{doc.data()};
        v_noabi const to = from;

        CHECK(to == to_v1(to));
        CHECK(to == from);
        CHECK(to != v1{});
    }

    SECTION("to_v1") {
        v_noabi from{doc.data(), doc.size()};
        v1 const to{from};

        CHECK(to == from_v1(to));
        CHECK(to == from);
        CHECK(to != v_noabi{});
    }
}

template <typename Deleter, typename T>
bsoncxx::v1::stdx::optional<Deleter> get_deleter(T const& v) {
    if (auto const deleter_ptr = v.get_deleter().template target<Deleter>()) {
        return *deleter_ptr;
    }
    return {};
}

TEST_CASE("v1", "[bsoncxx][v_noabi][document][value]") {
    using v1 = v1::document::value;
    using v_noabi = v_noabi::document::value;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    auto tmp = bsoncxx::from_json(R"({"x": 1})");

    auto const bson_free_deleter = bsoncxx::from_json("{}").release().get_deleter();
    auto const uint8_t_deleter = v_noabi{{}}.release().get_deleter();
    auto const noop_deleter = &v1::noop_deleter;

    REQUIRE(bson_free_deleter);
    REQUIRE(uint8_t_deleter);
    REQUIRE(noop_deleter);

    CAPTURE(bson_free_deleter);
    CAPTURE(uint8_t_deleter);
    CAPTURE(noop_deleter);

    SECTION("from_v1") {
        auto const size = tmp.size();
        auto ptr = tmp.release();
        auto const data = ptr.release();
        auto const deleter = ptr.get_deleter(); // bson_free_deleter

        SECTION("default") {
            (void)v_noabi{data, size, deleter}; // Unused.

            bsoncxx::v1::document::view const empty;

            v1 from;

            CHECK(from.data() == empty.data());
            CHECK(from.size() == empty.size());
            CHECK(get_deleter<v_noabi::deleter_type>(from) == noop_deleter);

            SECTION("copy") {
                v_noabi to = from_v1(from); // noop_deleter -> uint8_t_deleter (copy)

                CHECK(to.data() != empty.data());
                CHECK(to.size() == empty.size());
                CHECK(to.release().get_deleter() == uint8_t_deleter);
            }

            SECTION("move") {
                v_noabi to = from_v1(std::move(from)); // noop_deleter -> uint8_t_deleter (fallback to copy)

                CHECK(from.data() == empty.data());
                CHECK(from.size() == empty.size());
                CHECK(get_deleter<v_noabi::deleter_type>(from) == noop_deleter);

                CHECK(to.data() != empty.data());
                CHECK(to.size() == empty.size());
                CHECK(to.release().get_deleter() == uint8_t_deleter);
            }
        }

        SECTION("copy") {
            v1 const from{data, deleter};

            CHECK(from.data() == data);
            CHECK(from.size() == size);
            CHECK(get_deleter<v_noabi::deleter_type>(from) == bson_free_deleter);

            v_noabi to = from_v1(from); // bson_free_deleter -> uint8_t_deleter (copy)

            CHECK(from.data() == data);
            CHECK(from.size() == size);
            CHECK(get_deleter<v_noabi::deleter_type>(from) == bson_free_deleter);

            CHECK(to.data() != data);
            CHECK(to.size() == size);
            CHECK(to.release().get_deleter() == uint8_t_deleter);
        }

        SECTION("move") {
            v1 from{data, deleter};

            CHECK(from.data() == data);
            CHECK(from.size() == size);
            CHECK(get_deleter<v_noabi::deleter_type>(from) == bson_free_deleter);

            v_noabi to = from_v1(std::move(from)); // bson_free_deleter (move)

            CHECK(from.data() == nullptr);
            CHECK(from.size() == 0u);

            // A moved-from std::function<T> is in "a valid with an unspecified value".
            CHECK_NOFAIL(get_deleter<v_noabi::deleter_type>(from) == nullptr);

            CHECK(to.data() == data);
            CHECK(to.size() == size);
            CHECK(to.release().get_deleter() == bson_free_deleter);
        }

        SECTION("invalid deleter type") {
            v1 const owner{data, deleter};
            v1 from = owner; // bson_free_deleter -> default_deleter_type (copy)

            CHECK(from.data() != data);
            CHECK(from.size() == size);
            CHECK(get_deleter<v1::default_deleter_type>(from).has_value());

            auto const from_data = from.data();

            SECTION("copy") {
                v_noabi to = from_v1(from); // default_deleter_type -> uint8_t_deleter (copy)

                CHECK(from.data() == from_data);
                CHECK(from.size() == size);
                CHECK(get_deleter<v1::default_deleter_type>(from).has_value());

                CHECK(to.data() != from_data);
                CHECK(to.size() == size);
                CHECK(to.release().get_deleter() == uint8_t_deleter);
            }

            SECTION("move") {
                v_noabi to = from_v1(std::move(from)); // default_deleter_type -> uint8_t_deleter (fallback to copy)

                CHECK(from.data() == from_data);
                CHECK(from.size() == size);
                CHECK(get_deleter<v1::default_deleter_type>(from).has_value());

                CHECK(to.data() != from_data);
                CHECK(to.size() == size);
                CHECK(to.release().get_deleter() == uint8_t_deleter);
            }
        }
    }

    SECTION("to_v1") {
        auto const size = tmp.size();
        auto ptr = tmp.release();
        auto const data = ptr.release();
        auto const deleter = ptr.get_deleter();

        v_noabi from{data, size, deleter};

        SECTION("copy") {
            CHECK(from.data() == data);
            CHECK(from.size() == size);

            v1 const to = to_v1(from); // bson_free_deleter -> uint8_t_deleter (copy)

            CHECK(from.data() == data);
            CHECK(from.size() == size);
            CHECK(from.release().get_deleter() == bson_free_deleter);

            CHECK(to.data() != data);
            CHECK(to.size() == size);
            CHECK(get_deleter<v_noabi::deleter_type>(to) == uint8_t_deleter);
        }

        SECTION("move") {
            CHECK(from.data() == data);
            CHECK(from.size() == size);

            v1 const to = to_v1(std::move(from)); // bson_free_deleter (move)

            CHECK(from.data() == nullptr);
            CHECK(from.size() == size);

            // A moved-from std::function<T> is in "a valid with an unspecified value".
            CHECK_NOFAIL(from.release().get_deleter() == nullptr);

            CHECK(to.data() == data);
            CHECK(to.size() == size);
            CHECK(get_deleter<v_noabi::deleter_type>(to) == bson_free_deleter);
        }
    }
}

} // namespace
