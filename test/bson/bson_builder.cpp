#include "catch.hpp"

#include <cstring>
#include "bson/builder.hpp"
#include "bson.h"

void bson_eq_builder(const bson_t* bson, const bson::builder::document& builder) {
    bson::document::view expected(bson_get_data(bson), bson->len);
    bson::document::view test(builder.view());
    INFO("expected = " << expected);
    INFO("builder = " << test);
    REQUIRE(expected.get_len() == test.get_len());
    REQUIRE(std::memcmp(expected.get_buf(), test.get_buf(), expected.get_len()) == 0);
}

using namespace bson;

TEST_CASE("builder appends utf8", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_utf8(&expected, "hello", -1, "world", -1);

    builder::document b;

    SECTION("works with string literals") {
        b << "hello"
          << "world";

        bson_eq_builder(&expected, b);
    }

    SECTION("works with std::string") {
        b << "hello" << std::string{"world"};

        bson_eq_builder(&expected, b);
    }

    SECTION("works with b_utf8") {
        b << "hello" << types::b_utf8{"world"};

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends double", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_double(&expected, "foo", -1, 1.1);

    builder::document b;

    SECTION("works with raw float") {
        b << "foo" << 1.1;

        bson_eq_builder(&expected, b);
    }

    SECTION("works with b_double") {
        b << "foo" << types::b_double{1.1};

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends binary", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_binary(&expected, "foo", -1, BSON_SUBTYPE_BINARY, (uint8_t*)"deadbeef", 8);

    builder::document b;

    b << "foo" << types::b_binary{binary_sub_type::k_binary, 8, (uint8_t*)"deadbeef"};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends undefined", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);

    bson_append_undefined(&expected, "foo", -1);

    builder::document b;

    b << "foo" << types::b_undefined{};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends oid", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);

    bson_oid_t oid;
    bson_oid_init(&oid, NULL);

    bson_append_oid(&expected, "foo", -1, &oid);

    builder::document b;

    SECTION("b_oid works") {
        b << "foo" << types::b_oid{bson::oid{(char*)oid.bytes, 12}};

        bson_eq_builder(&expected, b);
    }

    SECTION("raw oid works") {
        b << "foo" << bson::oid{(char*)oid.bytes, 12};

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends bool", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    SECTION("b_bool true works") {
        bson_append_bool(&expected, "foo", -1, 1);

        b << "foo" << types::b_bool{true};

        bson_eq_builder(&expected, b);
    }

    SECTION("raw true works") {
        bson_append_bool(&expected, "foo", -1, 1);

        b << "foo" << true;

        bson_eq_builder(&expected, b);
    }

    SECTION("b_bool false works") {
        bson_append_bool(&expected, "foo", -1, 0);

        b << "foo" << types::b_bool{false};

        bson_eq_builder(&expected, b);
    }

    SECTION("raw false works") {
        bson_append_bool(&expected, "foo", -1, 0);

        b << "foo" << false;

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends date time", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_date_time(&expected, "foo", -1, 10000);

    b << "foo" << types::b_date{10000};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends null", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_null(&expected, "foo", -1);

    b << "foo" << types::b_null{};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends regex", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_regex(&expected, "foo", -1, "^foo|bar$", "i");

    b << "foo" << types::b_regex{"^foo|bar$", "i"};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends code", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_code(&expected, "foo", -1, "var a = {};");

    b << "foo" << types::b_code{"var a = {};"};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends symbol", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_symbol(&expected, "foo", -1, "deadbeef", -1);

    b << "foo" << types::b_symbol{"deadbeef"};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends code with scope", "[bson::builder]") {
    bson_t expected, scope;
    bson_init(&expected);
    builder::document b;
    builder::document scope_builder;

    bson_init(&scope);

    bson_append_int32(&scope, "b", -1, 10);

    scope_builder << "b" << 10;

    bson_append_code_with_scope(&expected, "foo", -1, "var a = b;", &scope);

    b << "foo" << types::b_codewscope{"var a = b;", scope_builder.view()};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends int32", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_int32(&expected, "foo", -1, 100);

    SECTION("raw int32") {
        b << "foo" << 100;

        bson_eq_builder(&expected, b);
    }

    SECTION("b_int32") {
        b << "foo" << types::b_int32{100};

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends timestamp", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_timestamp(&expected, "foo", -1, 100, 1000);

    b << "foo" << types::b_timestamp{100, 1000};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends int64", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_int64(&expected, "foo", -1, 100);

    SECTION("raw int64") {
        b << "foo" << std::int64_t(100);

        bson_eq_builder(&expected, b);
    }

    SECTION("b_int64") {
        b << "foo" << types::b_int64{100};

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&expected);
}

TEST_CASE("builder appends minkey", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_minkey(&expected, "foo", -1);

    b << "foo" << types::b_minkey{};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends maxkey", "[bson::builder]") {
    bson_t expected;
    bson_init(&expected);
    builder::document b;

    bson_append_maxkey(&expected, "foo", -1);

    b << "foo" << types::b_maxkey{};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}

TEST_CASE("builder appends array", "[bson::builder]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::document b;
    builder::document child_builder;

    bson_append_utf8(&child, "0", -1, "baz", -1);
    bson_append_array(&expected, "foo", -1, &child);

    child_builder << "0"
                  << "baz";

    b << "foo" << types::b_array{child_builder.view()};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends document", "[bson::builder]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::document b;
    builder::document child_builder;

    bson_append_utf8(&child, "bar", -1, "baz", -1);
    bson_append_document(&expected, "foo", -1, &child);

    child_builder << "bar"
                  << "baz";

    b << "foo" << types::b_document{child_builder.view()};

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends inline array", "[bson::builder]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::document b;

    bson_append_utf8(&child, "0", -1, "baz", -1);
    bson_append_array(&expected, "foo", -1, &child);

    b << "foo" << builder::helpers::open_array << "baz" << builder::helpers::close_array;

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends inline document", "[bson::builder]") {
    bson_t expected, child;
    bson_init(&expected);
    bson_init(&child);
    builder::document b;

    bson_append_utf8(&child, "bar", -1, "baz", -1);
    bson_append_document(&expected, "foo", -1, &child);

    b << "foo" << builder::helpers::open_doc << "bar"
      << "baz" << builder::helpers::close_doc;

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&child);
}

TEST_CASE("builder appends inline nested", "[bson::builder]") {
    using namespace builder::helpers;

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
    builder::document b;

    b << "foo" << open_doc << "bar" << open_array << 1 << 2 << open_doc << "hello"
      << "world" << close_doc << close_array << close_doc;

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
    bson_destroy(&foo);
    bson_destroy(&bar);
    bson_destroy(&third);
}

TEST_CASE("builder appends concat", "[bson::builder]") {
    using namespace builder::helpers;

    bson_t expected, child;

    bson_init(&expected);
    bson_init(&child);

    builder::document b;

    SECTION("document context works") {
        bson_append_utf8(&child, "hello", -1, "world", -1);
        bson_append_document(&expected, "foo", -1, &child);

        builder::document child_builder;

        child_builder << "hello"
                      << "world";

        b << "foo" << open_doc << concat{child_builder.view()} << close_doc;

        bson_eq_builder(&expected, b);
    }

    SECTION("array context works") {
        bson_append_utf8(&child, "0", -1, "bar", -1);
        bson_append_utf8(&child, "1", -1, "baz", -1);
        bson_append_array(&expected, "foo", -1, &child);

        builder::document child_builder;

        child_builder << "0"
                      << "baz";

        b << "foo" << open_array << "bar" << concat{child_builder.view()} << close_array;

        bson_eq_builder(&expected, b);
    }

    bson_destroy(&child);
    bson_destroy(&expected);
}

TEST_CASE("builder appends element", "[bson::builder]") {
    using namespace builder::helpers;

    bson_t expected;
    bson_init(&expected);

    builder::document b;
    builder::document tmp;

    bson_append_int32(&expected, "foo", -1, 999);

    tmp << "foo" << 999;

    b << "foo" << tmp.view()["foo"];

    bson_eq_builder(&expected, b);

    bson_destroy(&expected);
}
