#include "catch.hpp"

#include <cstring>
#include "bson/string_or_literal.hpp"

TEST_CASE("string_or_literal default construction works", "[bson::string_or_literal]") {
    using namespace bson;

    string_or_literal val;

    REQUIRE(val.length() == 0);
    REQUIRE(std::string(val.c_str()) == "");
}

TEST_CASE("string_or_literal literal construction works", "[bson::string_or_literal]") {
    using namespace bson;

    string_or_literal val("foo");

    REQUIRE(val.length() == 3);
    REQUIRE(std::string(val.c_str()) == std::string("foo"));
}

TEST_CASE("string_or_literal string construction works", "[bson::string_or_literal]") {
    using namespace bson;

    string_or_literal val(std::string("foo"));

    REQUIRE(val.length() == 3);
    REQUIRE(std::string(val.c_str()) == std::string("foo"));
}

void ensure_string_or_literal(const bson::string_or_literal& sol, const char* compare) {
    REQUIRE(sol.length() == std::strlen(compare));
    REQUIRE(std::string(sol.c_str()) == std::string(compare));
}

TEST_CASE("string_or_literal construction from a literal string_or_literal works",
          "[bson::string_or_literal]") {
    using namespace bson;

    string_or_literal val("foo");

    SECTION("copy construction works") {
        string_or_literal x(val);

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "foo");
    }

    SECTION("move construction works") {
        string_or_literal x(std::move(val));

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "");
    }

    SECTION("copy assignment works") {
        string_or_literal x = val;

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "foo");
    }

    SECTION("move assignment works") {
        string_or_literal x = std::move(val);

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "");
    }
}

TEST_CASE("string_or_literal construction from a string string_or_literal works",
          "[bson::string_or_literal]") {
    using namespace bson;

    string_or_literal val(std::string("foo"));

    SECTION("copy construction works") {
        string_or_literal x(val);

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "foo");
    }

    SECTION("move construction works") {
        string_or_literal x(std::move(val));

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "");
    }

    SECTION("copy assignment works") {
        string_or_literal x = val;

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "foo");
    }

    SECTION("move assignment works") {
        string_or_literal x = std::move(val);

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "");
    }

    SECTION("copy assignment works with previous unowned") {
        string_or_literal x("bar");
        x = val;

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "foo");
    }

    SECTION("copy assignment works with previous owned") {
        string_or_literal x(std::string("bar"));
        x = val;

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "foo");
    }

    SECTION("move assignment works with previous unowned") {
        string_or_literal x("bar");
        x = std::move(val);

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "");
    }

    SECTION("move assignment works with previous owned") {
        string_or_literal x(std::string("bar"));
        x = std::move(val);

        ensure_string_or_literal(x, "foo");
        ensure_string_or_literal(val, "");
    }
}
