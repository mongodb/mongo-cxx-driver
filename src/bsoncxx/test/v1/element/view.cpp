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

#include <bsoncxx/test/v1/element/view.hh>

//

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/exception.hh>
#include <bsoncxx/test/v1/types/value.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <string>
#include <system_error>

#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using bsoncxx::v1::element::view;
using code = bsoncxx::v1::element::view::errc;

TEST_CASE("error code", "[bsoncxx][v1][element][view][error]") {
    using bsoncxx::v1::source_errc;
    using bsoncxx::v1::type_errc;

    auto const& category = bsoncxx::v1::element::view::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("bsoncxx::v1::element::view"));

    auto const zero_errc = make_error_condition(static_cast<std::errc>(0));

    SECTION("unknown") {
        std::error_code const ec = static_cast<code>(-1);

        CHECK(ec.category() == category);
        CHECK(ec.value() == -1);
        CHECK(ec);
        CHECK(ec.message() == "unknown: -1");
    }

    SECTION("zero") {
        std::error_code const ec = code::zero;

        CHECK(ec.category() == category);
        CHECK(ec.value() == 0);
        CHECK_FALSE(ec);
        CHECK(ec.message() == "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("non-zero") {
        std::error_code const ec = code::invalid_view;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_view) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_data) == source_errc::bsoncxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_view) == type_errc::runtime_error);
        CHECK(make_error_code(code::invalid_data) == type_errc::runtime_error);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][element][view]") {
    SECTION("invalid_view") {
        SECTION("view") {
            view v;

            CHECK_FALSE(v);

            CHECK_NOTHROW(v.raw(), code::invalid_view);
            CHECK_NOTHROW(v.length(), code::invalid_view);
            CHECK_NOTHROW(v.offset(), code::invalid_view);
            CHECK_NOTHROW(v.keylen(), code::invalid_view);

            CHECK_THROWS_WITH_CODE(v.type_id(), code::invalid_view);
            CHECK_THROWS_WITH_CODE(v.key(), code::invalid_view);
#pragma push_macro("X")
#undef X
#define X(_name, _value) CHECK_THROWS_WITH_CODE(v.get_##_name(), code::invalid_view);
            BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

            CHECK_THROWS_WITH_CODE(v.type_view(), code::invalid_view);
            CHECK_THROWS_WITH_CODE(v.type_value(), code::invalid_view);

            CHECK_FALSE(v[""]);
            CHECK_FALSE(v[0]);
        }

        SECTION("key") {
            SECTION("none") {
                bsoncxx::v1::document::view doc;

                auto const x = doc["x"];
                CHECK_FALSE(x);
                CHECK_THROWS_WITH_CODE(x.key(), code::invalid_view);
                CHECK_THROWS_WITH(x.key(), "view is invalid"); // No key.
            }

            SECTION("element") {
                std::uint8_t data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // { 'x': 1 }
                auto const doc = bsoncxx::v1::document::view{data};

                auto const x = doc["x"];
                REQUIRE(x);
                REQUIRE(x.key() == "x");

                auto const y = x["y"];
                CHECK_FALSE(y);
                CHECK_THROWS_WITH_CODE(y.key(), code::invalid_view);
                CHECK_THROWS_WITH(y.key(), Catch::Matchers::ContainsSubstring(R"("x")"));

                auto const z = y["z"];
                CHECK_FALSE(z);
                CHECK_THROWS_WITH_CODE(z.key(), code::invalid_view);
                CHECK_THROWS_WITH(z.key(), Catch::Matchers::ContainsSubstring(R"("x")"));
            }
        }
    }

    SECTION("invalid_data") {
        std::uint8_t data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // { 'x': 1 }
        auto const doc = bsoncxx::v1::document::view{data};
        auto const iter = doc.begin();
        REQUIRE(iter != doc.end());
        auto const v = *iter;

        std::memset(data, 0u, sizeof(data));

        CHECK_NOTHROW(v.raw());
        CHECK_NOTHROW(v.length());
        CHECK_NOTHROW(v.offset());
        CHECK_NOTHROW(v.keylen());

        CHECK_THROWS_WITH_CODE(v.key(), code::invalid_data);
        CHECK_THROWS_WITH_CODE(v.key(), code::invalid_data);

#pragma push_macro("X")
#undef X
#define X(_name, _value) CHECK_THROWS_WITH_CODE(v.get_##_name(), code::invalid_data);
        BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

        CHECK_THROWS_WITH_CODE(v.type_view(), code::invalid_data);
        CHECK_THROWS_WITH_CODE(v.type_value(), code::invalid_data);

        CHECK_FALSE(v[""]);
        CHECK_FALSE(v[0]);
    }
}

TEST_CASE("basic", "[bsoncxx][v1][element][view]") {
    SECTION("int32") {
        // { 'x': 1 }
        std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0};
        bsoncxx::v1::document::view doc{data};
        auto const iter = doc.begin();
        REQUIRE(iter != doc.end());
        auto const x = *iter;

        CHECK(x.type_id() == bsoncxx::v1::types::id::k_int32);
        CHECK(x.key() == "x");

        CHECK(x.get_int32() == bsoncxx::v1::types::b_int32{1});
        CHECK_THROWS_WITH_CODE(x.get_document(), bsoncxx::v1::types::view::errc::type_mismatch);
        CHECK_THROWS_WITH_CODE(x.get_array(), bsoncxx::v1::types::view::errc::type_mismatch);

        CHECK(x.type_view() == bsoncxx::v1::types::b_int32{1});
        CHECK(x.type_value() == x.type_view());

        CHECK_FALSE(x[""]);
        CHECK_FALSE(x[0]);
    }

    SECTION("document") {
        // { 'x': { 'y': 1 } }
        std::uint8_t const data[] = {20, 0, 0, 0, 3, 'x', '\0', 12, 0, 0, 0, 16, 'y', '\0', 2, 0, 0, 0, 0, 0};
        bsoncxx::v1::document::view doc{data};
        bsoncxx::v1::document::view subdoc{data + 7};
        auto const iter = doc.begin();
        REQUIRE(iter != doc.end());
        auto const x = *iter;

        CHECK(x.type_id() == bsoncxx::v1::types::id::k_document);
        CHECK(x.key() == "x");

        CHECK(x.get_document() == bsoncxx::v1::types::b_document{subdoc});
        CHECK_THROWS_WITH_CODE(x.get_int32(), bsoncxx::v1::types::view::errc::type_mismatch);
        CHECK_THROWS_WITH_CODE(x.get_array(), bsoncxx::v1::types::view::errc::type_mismatch);

        CHECK(x.type_view() == bsoncxx::v1::types::b_document{subdoc});
        CHECK(x.type_value() == x.type_view());

        CHECK_FALSE(x[""]);
        CHECK_FALSE(x[0]);

        auto const y = x["y"];
        REQUIRE(y);

        CHECK(y.type_id() == bsoncxx::v1::types::id::k_int32);
        CHECK(y.key() == "y");

        CHECK(y.get_int32() == bsoncxx::v1::types::b_int32{2});
        CHECK_THROWS_WITH_CODE(y.get_document(), bsoncxx::v1::types::view::errc::type_mismatch);
        CHECK_THROWS_WITH_CODE(y.get_array(), bsoncxx::v1::types::view::errc::type_mismatch);

        CHECK(y.type_view() == bsoncxx::v1::types::b_int32{2});
        CHECK(y.type_value() == y.type_view());
    }

    SECTION("array") {
        // { 'x': [ 1 ] }
        std::uint8_t const data[] = {20, 0, 0, 0, 4, 'x', '\0', 12, 0, 0, 0, 16, '0', '\0', 2, 0, 0, 0, 0, 0};
        bsoncxx::v1::document::view doc{data};
        bsoncxx::v1::array::view subarr{data + 7};
        auto const iter = doc.begin();
        REQUIRE(iter != doc.end());
        auto const x = *iter;

        CHECK(x.type_id() == bsoncxx::v1::types::id::k_array);
        CHECK(x.key() == "x");

        CHECK(x.get_array() == bsoncxx::v1::types::b_array{subarr});
        CHECK_THROWS_WITH_CODE(x.get_int32(), bsoncxx::v1::types::view::errc::type_mismatch);
        CHECK_THROWS_WITH_CODE(x.get_document(), bsoncxx::v1::types::view::errc::type_mismatch);

        CHECK(x.type_view() == bsoncxx::v1::types::b_array{subarr});
        CHECK(x.type_value() == x.type_view());

        CHECK_FALSE(x[""]);
        CHECK_FALSE(x[1]);

        auto const e = x[0];
        REQUIRE(e);

        CHECK(e.type_id() == bsoncxx::v1::types::id::k_int32);
        CHECK(e.key() == "0");

        CHECK(e.get_int32() == bsoncxx::v1::types::b_int32{2});
        CHECK_THROWS_WITH_CODE(e.get_document(), bsoncxx::v1::types::view::errc::type_mismatch);
        CHECK_THROWS_WITH_CODE(e.get_array(), bsoncxx::v1::types::view::errc::type_mismatch);

        CHECK(e.type_view() == bsoncxx::v1::types::b_int32{2});
        CHECK(e.type_value() == e.type_view());
    }
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][element][view]") {
    // {"x": 1}
    std::uint8_t const bytes[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0};

    bsoncxx::v1::document::view doc{bytes};

    REQUIRE(doc);

    CHECK(bsoncxx::test::stringify(doc["x"]) == R"("x": 1)");
    CHECK(bsoncxx::test::stringify(doc["y"]) == "invalid");
}

} // namespace

std::string Catch::StringMaker<bsoncxx::v1::element::view>::convert(bsoncxx::v1::element::view const& value) try {
    if (!value) {
        return "invalid";
    }

    std::string res;
    res += bsoncxx::test::stringify(value.key());
    res += ": ";
    res += bsoncxx::test::stringify(value.type_view());
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    if (ex.code() == code::invalid_view || ex.code() == code::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}
