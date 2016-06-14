// Copyright 2016 MongoDB Inc.
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

#include <cstring>

#include <bson.h>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

void bson_eq_stream(const bson_t* bson, const bsoncxx::builder::stream::document& builder) {
    using namespace bsoncxx;

    bsoncxx::document::view expected(bson_get_data(bson), bson->len);
    bsoncxx::document::view test(builder.view());

    INFO("expected = " << to_json(expected));
    INFO("builder = " << to_json(test));
    REQUIRE(expected.length() == test.length());
    REQUIRE(std::memcmp(expected.data(), test.data(), expected.length()) == 0);
}

template <typename T, typename U>
void viewable_eq_viewable(const T& stream, const U& basic) {
    using namespace bsoncxx;

    bsoncxx::document::view expected(stream.view());
    bsoncxx::document::view test(basic.view());

    INFO("expected = " << to_json(expected));
    INFO("basic = " << to_json(test));
    REQUIRE(expected.length() == test.length());
    REQUIRE(std::memcmp(expected.data(), test.data(), expected.length()) == 0);
}

using namespace bsoncxx;

TEST_CASE("builder appends utf8", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_utf8(&expected, "hello", -1, "world", -1);

    builder::stream::document b;

    SECTION("works with string literals") {
        b << "hello"
          << "world";

        bson_eq_stream(&expected, b);
    }

    SECTION("works with std::string") {
        b << "hello" << std::string{"world"};

        bson_eq_stream(&expected, b);
    }

    SECTION("works with b_utf8") {
        b << "hello" << types::b_utf8{"world"};

        bson_eq_stream(&expected, b);
    }

    SECTION("works with const char*") {
        const char* world = "world";
        b << "hello" << world;

        bson_eq_stream(&expected, b);
    }

    SECTION("works with char*") {
        char* world = const_cast<char*>("world");
        b << "hello" << world;

        bson_eq_stream(&expected, b);
    }

    // SECTION("fails to compile with non-char*") {
    //     int world = 10;
    //     b << "hello" << &world;

    //     bson_eq_stream(&expected, b);
    // }

    bson_destroy(&expected);
}

TEST_CASE("builder appends double", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_double(&expected, "foo", -1, 1.1);

    builder::stream::document b;

    SECTION("works with raw float") {
        b << "foo" << 1.1;

        bson_eq_stream(&expected, b);
    }

    SECTION("works with b_double") {
        b << "foo" << types::b_double{1.1};

        bson_eq_stream(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends binary", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_binary(&expected, "foo", -1, BSON_SUBTYPE_BINARY, (uint8_t*)"deadbeef", 8);

    builder::stream::document b;

    b << "foo" << types::b_binary{binary_sub_type::k_binary, 8, (uint8_t*)"deadbeef"};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends undefined", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_undefined(&expected, "foo", -1);

    builder::stream::document b;

    b << "foo" << types::b_undefined{};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends oid", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);

    bson_oid_t oid;
    bson_oid_init(&oid, NULL);

    bson_append_oid(&expected, "foo", -1, &oid);

    builder::stream::document b;

    SECTION("b_oid works") {
        b << "foo" << types::b_oid{bsoncxx::oid{(char*)oid.bytes, 12}};

        bson_eq_stream(&expected, b);
    }

    SECTION("raw oid works") {
        b << "foo" << bsoncxx::oid{(char*)oid.bytes, 12};

        bson_eq_stream(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends bool", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    SECTION("b_bool true works") {
        bson_append_bool(&expected, "foo", -1, 1);

        b << "foo" << types::b_bool{true};

        bson_eq_stream(&expected, b);
    }

    SECTION("raw true works") {
        bson_append_bool(&expected, "foo", -1, 1);

        b << "foo" << true;

        bson_eq_stream(&expected, b);
    }

    SECTION("b_bool false works") {
        bson_append_bool(&expected, "foo", -1, 0);

        b << "foo" << types::b_bool{false};

        bson_eq_stream(&expected, b);
    }

    SECTION("raw false works") {
        bson_append_bool(&expected, "foo", -1, 0);

        b << "foo" << false;

        bson_eq_stream(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends date time", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_date_time(&expected, "foo", -1, 10000);

    b << "foo" << types::b_date{std::chrono::milliseconds{10000}};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends null", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_null(&expected, "foo", -1);

    b << "foo" << types::b_null{};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends regex", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_regex(&expected, "foo", -1, "^foo|bar$", "i");

    b << "foo" << types::b_regex{"^foo|bar$", "i"};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends code", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_code(&expected, "foo", -1, "var a = {};");

    b << "foo" << types::b_code{"var a = {};"};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends symbol", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_symbol(&expected, "foo", -1, "deadbeef", -1);

    b << "foo" << types::b_symbol{"deadbeef"};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends code with scope", "[bsoncxx::builder::stream]") {
    bson_t expected, scope;
    bson_init(&expected);
    builder::stream::document b;
    builder::stream::document scope_builder;

    bson_init(&scope);

    bson_append_int32(&scope, "b", -1, 10);

    scope_builder << "b" << 10;

    bson_append_code_with_scope(&expected, "foo", -1, "var a = b;", &scope);

    b << "foo" << types::b_codewscope{"var a = b;", scope_builder.view()};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends int32", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_int32(&expected, "foo", -1, 100);

    SECTION("raw int32") {
        b << "foo" << 100;

        bson_eq_stream(&expected, b);
    }

    SECTION("b_int32") {
        b << "foo" << types::b_int32{100};

        bson_eq_stream(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends timestamp", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_timestamp(&expected, "foo", -1, 100, 1000);

    b << "foo" << types::b_timestamp{100, 1000};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends int64", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_int64(&expected, "foo", -1, 100);

    SECTION("raw int64") {
        b << "foo" << std::int64_t(100);

        bson_eq_stream(&expected, b);
    }

    SECTION("b_int64") {
        b << "foo" << types::b_int64{100};

        bson_eq_stream(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends minkey", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_minkey(&expected, "foo", -1);

    b << "foo" << types::b_minkey{};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends maxkey", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);
    builder::stream::document b;

    bson_append_maxkey(&expected, "foo", -1);

    b << "foo" << types::b_maxkey{};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends array", "[bsoncxx::builder::stream]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::stream::document b;
    builder::stream::array child_builder;

    bson_append_utf8(&child, "0", -1, "baz", -1);
    bson_append_array(&expected, "foo", -1, &child);

    child_builder << "baz";

    b << "foo" << types::b_array{child_builder.view()};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends document", "[bsoncxx::builder::stream]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::stream::document b;
    builder::stream::document child_builder;

    bson_append_utf8(&child, "bar", -1, "baz", -1);
    bson_append_document(&expected, "foo", -1, &child);

    child_builder << "bar"
                  << "baz";

    b << "foo" << types::b_document{child_builder.view()};

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends inline array", "[bsoncxx::builder::stream]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::stream::document b;

    bson_append_utf8(&child, "0", -1, "baz", -1);
    bson_append_array(&expected, "foo", -1, &child);

    b << "foo" << builder::stream::open_array << "baz" << builder::stream::close_array;

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends inline document", "[bsoncxx::builder::stream]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::stream::document b;

    bson_append_utf8(&child, "bar", -1, "baz", -1);
    bson_append_document(&expected, "foo", -1, &child);

    b << "foo" << builder::stream::open_document << "bar"
      << "baz" << builder::stream::close_document;

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends inline nested", "[bsoncxx::builder::stream]") {
    bson_t expected, foo, bar, third;

    bson_init(&expected);
    bson_init(&foo);
    bson_init(&bar);
    bson_init(&third);

    bson_append_utf8(&third, "hello", -1, "world", -1);
    bson_append_int32(&bar, "0", -1, 1);
    bson_append_int32(&bar, "1", -1, 2);
    bson_append_document(&bar, "2", -1, &third);
    bson_append_array(&foo, "bar", -1, &bar);
    bson_append_document(&expected, "foo", -1, &foo);
    builder::stream::document b;

    {
        using namespace builder::stream;

        b << "foo" << open_document << "bar" << open_array << 1 << 2 << open_document << "hello"
          << "world" << close_document << close_array << close_document;
    }

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&foo);
    bson_destroy(&bar);
    bson_destroy(&third);
}

TEST_CASE("builder appends concatenate", "[bsoncxx::builder::stream]") {
    bson_t expected, child;

    bson_init(&expected);
    bson_init(&child);

    builder::stream::document b;

    SECTION("document context works") {
        bson_append_utf8(&child, "hello", -1, "world", -1);
        bson_append_document(&expected, "foo", -1, &child);

        builder::stream::document child_builder;

        child_builder << "hello"
                      << "world";

        {
            using namespace builder::stream;
            b << "foo" << open_document << concatenate(child_builder.view()) << close_document;
        }

        bson_eq_stream(&expected, b);
    }

    SECTION("array context works") {
        bson_append_utf8(&child, "0", -1, "bar", -1);
        bson_append_utf8(&child, "1", -1, "0", -1);
        bson_append_utf8(&child, "2", -1, "baz", -1);
        bson_append_array(&expected, "foo", -1, &child);

        builder::stream::array child_builder;

        child_builder << "0"
                      << "baz";

        {
            using namespace builder::stream;
            b << "foo" << open_array << "bar" << concatenate(child_builder.view()) << close_array;
        }

        bson_eq_stream(&expected, b);
    }

    bson_destroy(&child);
    bson_destroy(&expected);
}

TEST_CASE("builder appends value", "[bsoncxx::builder::stream]") {
    bson_t expected;
    bson_init(&expected);

    builder::stream::document b;
    builder::stream::document tmp;

    bson_append_int32(&expected, "foo", -1, 999);

    tmp << "foo" << 999;

    b << "foo" << tmp.view()["foo"].get_value();

    bson_eq_stream(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends lambdas", "[bsoncxx::builder::stream]") {
    builder::stream::document expected;
    builder::stream::document stream;

    {
        using namespace builder::stream;
        expected << "a"
                 << "single"
                 << "b" << open_document << "key1"
                 << "value1"
                 << "key2"
                 << "value2" << close_document << "c" << open_array << 1 << 2 << 3 << close_array;

        stream << "a" << [](single_context s) { s << "single"; } << "b" << open_document <<
            [](key_context<> k) {
                k << "key1"
                  << "value1"
                  << "key2"
                  << "value2";
            } << close_document << "c" << open_array << [](array_context<> a) { a << 1 << 2 << 3; }
               << close_array;
    }

    viewable_eq_viewable(expected, stream);
}

TEST_CASE("document builder finalizes", "[bsoncxx::builder::stream]") {
    builder::stream::document expected;

    expected << "foo" << 999;

    document::value value = builder::stream::document{} << "foo" << 999
                                                        << bsoncxx::builder::stream::finalize;

    viewable_eq_viewable(expected, value);
}

TEST_CASE("array builder finalizes", "[bsoncxx::builder::stream]") {
    builder::stream::array expected;

    expected << 1 << 2 << 3;

    array::value value = builder::stream::array{} << 1 << 2 << 3
                                                  << bsoncxx::builder::stream::finalize;

    viewable_eq_viewable(expected, value);
}

TEST_CASE("document core builder ownership", "[bsoncxx::builder::core]") {
    builder::core b(false);

    SECTION("when passing a std::string key, ownership transfers to the instance") {
        {
            std::string key{"falafel"};
            b.key_owned(key);
        }
        b.append(types::b_int32{1});
        auto doc = b.view_document();
        auto ele = doc["falafel"];
        REQUIRE(ele.type() == type::k_int32);
        REQUIRE(ele.get_value() == types::b_int32{1});
    }

    SECTION("when passing a stdx::string_view, ownership handled by caller") {
        std::string key{"sabich"};
        stdx::string_view key_view{key};
        b.key_view(key_view);
        b.append(1);
    }
}

TEST_CASE("document core builder throws on insufficient stack", "[bsoncxx::builder::core]") {
    builder::core b(false);

    b.key_view("hi");
    REQUIRE_THROWS(b.close_document());
}

TEST_CASE("array core builder throws on insufficient stack", "[bsoncxx::builder::core]") {
    builder::core b(true);

    REQUIRE_THROWS(b.close_array());
}

TEST_CASE("core builder open/close works", "[bsoncxx::builder::core]") {
    builder::core b(false);

    b.key_view("hi");

    SECTION("opening and closing a document works") {
        b.open_document();
        b.close_document();
    }

    SECTION("opening and closing an array works") {
        b.open_array();
        b.close_array();
    }

    SECTION("opening a document and closing an array throws") {
        b.open_document();
        REQUIRE_THROWS(b.close_array());
    }

    SECTION("opening an array and closing a document throws") {
        b.open_array();
        REQUIRE_THROWS(b.close_document());
    }

    SECTION("opening an array and viewing throws") {
        b.open_array();
        REQUIRE_THROWS(b.view_document());
    }

    SECTION("opening a document and viewing throws") {
        b.open_document();
        REQUIRE_THROWS(b.view_document());
    }

    SECTION("viewing with with only the key and no value fails") {
        REQUIRE_THROWS(b.view_document());
    }

    SECTION("viewing with with a key and value suceeds") {
        b.append(10);
        b.view_document();
    }
}

TEST_CASE("basic document builder works", "[bsoncxx::builder::basic]") {
    builder::stream::document stream;
    builder::basic::document basic;

    stream << "hello"
           << "world";

    SECTION("kvp works") {
        {
            using namespace builder::basic;
            basic.append(kvp("hello", "world"));
        }

        viewable_eq_viewable(stream, basic);
    }

    SECTION("kvp works with std::string key/value") {
        {
            using namespace builder::basic;

            std::string a("hello");
            std::string b("world");
            basic.append(kvp(a, b));
        }

        viewable_eq_viewable(stream, basic);
    }

    SECTION("kvp works with stdx::string_view key/value") {
        {
            using namespace builder::basic;

            stdx::string_view a("hello");
            stdx::string_view b("world");
            basic.append(kvp(a, b));
        }

        viewable_eq_viewable(stream, basic);
    }
    SECTION("variadic works") {
        {
            using namespace builder::stream;
            stream << "foo" << 35 << "bar" << open_document << "que"
                   << "qux" << close_document << "baz" << open_array << 1 << 2 << 3 << close_array;
        }

        {
            using namespace builder::basic;

            basic.append(kvp("hello", "world"), kvp("foo", 35), kvp("bar", [](sub_document sd) {
                sd.append(kvp("que", "qux"));
            }), kvp("baz", [](sub_array sa) { sa.append(1, 2, 3); }));
        }

        viewable_eq_viewable(stream, basic);
    }
}

TEST_CASE("basic array builder works", "[bsoncxx::builder::basic]") {
    using namespace builder::basic;

    builder::stream::array stream;
    builder::basic::array basic;

    stream << "hello";

    SECTION("single insert works") {
        basic.append("hello");

        viewable_eq_viewable(stream, basic);
    }

    SECTION("variadic works") {
        stream << 35;

        basic.append("hello", 35);

        viewable_eq_viewable(stream, basic);
    }
}

TEST_CASE("basic document builder works with concat", "[bsoncxx::builder::basic]") {
    using namespace builder::basic;

    auto subdoc = builder::stream::document{} << "hello"
                                              << "world" << builder::stream::finalize;

    builder::stream::document stream;
    builder::basic::document basic;

    stream << builder::stream::concatenate(subdoc.view());

    SECTION("single insert works") {
        basic.append(builder::basic::concatenate(subdoc.view()));

        viewable_eq_viewable(stream, basic);
    }

    SECTION("variadic works") {
        stream << builder::stream::concatenate(subdoc.view());

        basic.append(builder::basic::concatenate(subdoc.view()),
                     builder::basic::concatenate(subdoc.view()));

        viewable_eq_viewable(stream, basic);
    }
}

TEST_CASE("basic array builder works with concat", "[bsoncxx::builder::basic]") {
    using namespace builder::basic;

    auto array_builder = builder::stream::array{} << 1 << 2 << builder::stream::finalize;
    auto array_view = array_builder.view();

    builder::stream::array stream;
    builder::basic::array basic;

    stream << builder::stream::concatenate(array_view);

    SECTION("single insert works") {
        basic.append(builder::basic::concatenate(array_view));

        viewable_eq_viewable(stream, basic);
    }

    SECTION("variadic works") {
        stream << builder::stream::concatenate(array_view);

        basic.append(builder::basic::concatenate(array_view),
                     builder::basic::concatenate(array_view));

        viewable_eq_viewable(stream, basic);
    }
}

TEST_CASE("element throws on bad get_", "[bsoncxx::builder::basic]") {
    using namespace builder::basic;

    builder::stream::array stream;
    builder::basic::array basic;

    stream << "hello";

    SECTION("single insert works") {
        basic.append("hello");

        viewable_eq_viewable(stream, basic);
    }

    SECTION("variadic works") {
        stream << 35;

        basic.append("hello", 35);

        viewable_eq_viewable(stream, basic);
    }
}

TEST_CASE("array::view works", "[bsoncxx::builder::array]") {
    using namespace builder::stream;

    builder::stream::array stream;

    stream << 100 << 99 << 98;

    REQUIRE(stream.view()[0].get_int32() == 100);
    REQUIRE(stream.view()[1].get_int32() == 99);
    REQUIRE(stream.view()[2].get_int32() == 98);
}

TEST_CASE("stream in a document::view works", "[bsoncxx::bulder::stream]") {
    using namespace builder::stream;

    auto sub_doc = builder::stream::document{} << "b" << 1 << finalize;
    auto full_doc = builder::stream::document{} << "a" << sub_doc.view() << finalize;

    REQUIRE(full_doc.view()["a"].type() == bsoncxx::types::b_document::type_id);
    REQUIRE(full_doc.view()["a"]["b"].type() == bsoncxx::types::b_int32::type_id);
    REQUIRE(full_doc.view()["a"]["b"].get_int32().value == 1);
}

TEST_CASE("stream in an array::view works", "[bsoncxx::bulder::stream]") {
    using namespace builder::stream;

    auto sub_array = builder::stream::array{} << 1 << 2 << 3 << finalize;
    auto full_doc = builder::stream::document{} << "a" << sub_array.view() << finalize;

    REQUIRE(full_doc.view()["a"].type() == bsoncxx::types::b_array::type_id);
    REQUIRE(full_doc.view()["a"][1].type() == bsoncxx::types::b_int32::type_id);
    REQUIRE(full_doc.view()["a"][1].get_int32().value == 2);
}
