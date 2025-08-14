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

#include <bsoncxx/test/v1/types/view.hh>

//

#include <bsoncxx/v1/detail/type_traits.hpp>

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/exception.hh>
#include <bsoncxx/test/v1/types/id.hh>
#include <bsoncxx/test/v1/types/value.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <string>
#include <system_error>

#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using namespace bsoncxx::v1::types;
using code = bsoncxx::v1::types::view::errc;

TEST_CASE("error code", "[bsoncxx][v1][types][view][error]") {
    using bsoncxx::v1::source_errc;
    using bsoncxx::v1::type_errc;

    auto const& category = bsoncxx::v1::types::view::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("bsoncxx::v1::types::view"));

    auto const zero_errc = make_error_condition(static_cast<std::errc>(0));

    SECTION("unknown") {
        std::error_code const ec = static_cast<code>(-1);

        CHECK(ec.category() == category);
        CHECK(ec.value() == -1);
        CHECK(ec);
        CHECK(ec.message() == std::string(category.name()) + ":-1");
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
        std::error_code const ec = code::type_mismatch;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::type_mismatch) == source_errc::bsoncxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::type_mismatch) == type_errc::runtime_error);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][types][view]") {
    using bsoncxx::v1::types::id;

    SECTION("type_mismatch") {
#pragma push_macro("X")
#pragma push_macro("Y")
#undef X
#undef Y

#define X(_name, _value) bsoncxx::v1::types::b_##_name{},
#define Y(_name, _value)                                              \
    CHECKED_IF(id::k_##_name == v.type_id()) {                        \
        CHECK_NOTHROW(v.get_##_name());                               \
    }                                                                 \
    else {                                                            \
        CHECK_THROWS_WITH_CODE(v.get_##_name(), code::type_mismatch); \
    }

        view views[] = {BSONCXX_V1_TYPES_XMACRO(X)};

        for (auto const& v : views) {
            BSONCXX_V1_TYPES_XMACRO(Y)
        }
#pragma pop_macro("X")
#pragma pop_macro("Y")
    }
}

TEST_CASE("basic", "[bsoncxx][v1][types][view]") {
    // {"x": 1}
    std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0};
    bsoncxx::v1::document::view doc{data};
    REQUIRE(doc);

    auto const e_valid = *doc.begin();
    auto const e_invalid = *doc.end();
    REQUIRE(e_valid);
    REQUIRE_FALSE(e_invalid);

    SECTION("default") {
        view v;

        CHECK(v == v);
        CHECK_FALSE(v != v);

        CHECK(v == view{});
        CHECK_FALSE(v != view{});

        CHECK_FALSE(v == b_int32{1});
        CHECK_FALSE(b_int32{1} == v);
        CHECK(v != b_int32{1});
        CHECK(b_int32{1} != v);

        CHECK_FALSE(v == e_valid);
        CHECK_FALSE(e_valid == v);
        CHECK(v != e_valid);
        CHECK(e_valid != v);

        CHECK_THROWS_WITH_CODE(v == e_invalid, bsoncxx::v1::element::view::errc::invalid_view);
        CHECK_THROWS_WITH_CODE(v != e_invalid, bsoncxx::v1::element::view::errc::invalid_view);
        CHECK_THROWS_WITH_CODE(e_invalid == v, bsoncxx::v1::element::view::errc::invalid_view);
        CHECK_THROWS_WITH_CODE(e_invalid != v, bsoncxx::v1::element::view::errc::invalid_view);
    }

    SECTION("value") {
        view v{b_int32{1}};

        CHECK(v == v);
        CHECK_FALSE(v != v);

        CHECK_FALSE(v == view{});
        CHECK(v != view{});

        CHECK(v == b_int32{1});
        CHECK(b_int32{1} == v);
        CHECK_FALSE(v != b_int32{1});
        CHECK_FALSE(b_int32{1} != v);

        CHECK(v == e_valid);
        CHECK(e_valid == v);
        CHECK_FALSE(v != e_valid);
        CHECK_FALSE(e_valid != v);

        CHECK_THROWS_WITH_CODE(v == e_invalid, bsoncxx::v1::element::view::errc::invalid_view);
        CHECK_THROWS_WITH_CODE(v != e_invalid, bsoncxx::v1::element::view::errc::invalid_view);
        CHECK_THROWS_WITH_CODE(e_invalid == v, bsoncxx::v1::element::view::errc::invalid_view);
        CHECK_THROWS_WITH_CODE(e_invalid != v, bsoncxx::v1::element::view::errc::invalid_view);
    }
}

TEST_CASE("b_types", "[bsoncxx][v1][types][view]") {
    std::uint8_t const data_a[] = {10, 0, 0, 0, 2, '0', '\0', 'a', '\0', 0}; // {"0": "a"}
    std::uint8_t const data_b[] = {10, 0, 0, 0, 2, '1', '\0', 'b', '\0', 0}; // {"1": "b"}

    bsoncxx::v1::oid const o;
    bsoncxx::v1::stdx::string_view const sv;

    double const double_a = 1.0;
    double const double_b = 2.0;

    bsoncxx::v1::stdx::string_view const string_a{"a"};
    bsoncxx::v1::stdx::string_view const string_b{"b"};

    bsoncxx::v1::document::view const doc_a{data_a};
    bsoncxx::v1::document::view const doc_b{data_b};

    bsoncxx::v1::array::view const arr_a{data_a};
    bsoncxx::v1::array::view const arr_b{data_b};

    bsoncxx::v1::oid const oid_a;
    bsoncxx::v1::oid const oid_b;

    std::chrono::milliseconds const date_a{1};
    std::chrono::milliseconds const date_b{2};

    bsoncxx::v1::stdx::string_view const code_a{"code_a"};
    bsoncxx::v1::stdx::string_view const code_b{"code_b"};

    bsoncxx::v1::stdx::string_view const symbol_a{"symbol_a"};
    bsoncxx::v1::stdx::string_view const symbol_b{"symbol_b"};

    std::int32_t const int32_a{1};
    std::int32_t const int32_b{2};

    std::int64_t const int64_a{1};
    std::int64_t const int64_b{2};

    bsoncxx::v1::decimal128 const d128_a{"1"};
    bsoncxx::v1::decimal128 const d128_b{"2"};

#define BTYPES_ASSERTIONS_COMMON(_name)             \
    if (1) {                                        \
        CHECK(b_##_name::type_id == id::k_##_name); \
        CHECK(v.type_id == id::k_##_name);          \
        CHECK(v == v);                              \
        CHECK_FALSE(v != v);                        \
    } else                                          \
        ((void)0)

#define BTYPES_ASSERTIONS_SINGLE(_name, _a, _b) \
    if (1) {                                    \
        b_##_name a{_a};                        \
        b_##_name b{_b};                        \
                                                \
        CHECK(a == a);                          \
        CHECK(b == b);                          \
        CHECK(a != b);                          \
                                                \
        /* Implicit Conversion */               \
        decltype(_a) c = a;                     \
                                                \
        CHECK(c == _a);                         \
        CHECK(c != _b);                         \
    } else                                      \
        ((void)0)

    // BSONCXX_V1_TYPES_XMACRO: update below.

    SECTION("b_minkey") {
        b_minkey v;
        BTYPES_ASSERTIONS_COMMON(minkey);
    }

    SECTION("b_double") {
        BSONCXX_PRIVATE_WARNINGS_PUSH();
        BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));
        b_double v{0.0};
        BTYPES_ASSERTIONS_COMMON(double);
        BTYPES_ASSERTIONS_SINGLE(double, double_a, double_b);
        BSONCXX_PRIVATE_WARNINGS_POP();
    }

    SECTION("b_string") {
        b_string v;
        BTYPES_ASSERTIONS_COMMON(string);
        BTYPES_ASSERTIONS_SINGLE(string, string_a, string_b);
    }

    SECTION("b_document") {
        b_document const v;
        BTYPES_ASSERTIONS_COMMON(document);
        BTYPES_ASSERTIONS_SINGLE(document, doc_a, doc_b);
    }

    SECTION("b_array") {
        b_array const v;
        BTYPES_ASSERTIONS_COMMON(array);
        BTYPES_ASSERTIONS_SINGLE(array, arr_a, arr_b);
    }

    SECTION("b_binary") {
        b_binary const v;
        BTYPES_ASSERTIONS_COMMON(binary);
        std::uint8_t const data[] = {1, 2};

        b_binary a = v;
        b_binary b{binary_subtype::k_binary, 0, nullptr};

        CHECK(a == b);

        a = {binary_subtype::k_binary, 1, nullptr}; // OK
        CHECK(a != b);

        b = a;
        CHECK(a == b);

        a = {binary_subtype::k_binary, 1, data};
        CHECK(a != b);

        b = a;
        CHECK(a == b);

        a = {binary_subtype::k_binary, 2, data};
        CHECK(a != b);

        b = a;
        CHECK(a == b);

        a = {binary_subtype::k_encrypted, 2, data};
        CHECK(a != b);

        b = a;
        CHECK(a == b);

        std::uint8_t b_data[] = {2, 2};
        b = {binary_subtype::k_encrypted, 2, b_data};
        CHECK(a != b); // a[0] != b[0]
        (void)(b_data[0] = 1), (void)(b_data[1] = 1);
        CHECK(a != b); // a[1] != b[1]
        (void)(b_data[0] = 1), (void)(b_data[1] = 2);
        CHECK(a == b);
    }

    SECTION("b_undefined") {
        b_undefined const v;
        BTYPES_ASSERTIONS_COMMON(undefined);
    }

    SECTION("b_oid") {
        b_oid const v;
        BTYPES_ASSERTIONS_COMMON(oid);
        BTYPES_ASSERTIONS_SINGLE(oid, oid_a, oid_b);
    }

    SECTION("b_bool") {
        b_bool const v;
        BTYPES_ASSERTIONS_COMMON(bool);
        BTYPES_ASSERTIONS_SINGLE(bool, false, true);
    }

    SECTION("b_date") {
        b_date const v;
        BTYPES_ASSERTIONS_COMMON(date);
        BTYPES_ASSERTIONS_SINGLE(date, date_a, date_b);
    }

    SECTION("b_null") {
        b_null const v;
        BTYPES_ASSERTIONS_COMMON(null);
    }

    SECTION("b_regex") {
        b_regex const v;
        BTYPES_ASSERTIONS_COMMON(regex);
    }

    SECTION("b_dbpointer") {
        b_dbpointer v{sv, o};
        BTYPES_ASSERTIONS_COMMON(dbpointer);
    }

    SECTION("b_code") {
        b_code const v;
        BTYPES_ASSERTIONS_COMMON(code);
        BTYPES_ASSERTIONS_SINGLE(code, code_a, code_b);
    }

    SECTION("b_symbol") {
        b_symbol const v;
        BTYPES_ASSERTIONS_COMMON(symbol);
        BTYPES_ASSERTIONS_SINGLE(symbol, symbol_a, symbol_b);
    }

    SECTION("b_codewscope") {
        b_codewscope const v;
        BTYPES_ASSERTIONS_COMMON(codewscope);
    }

    SECTION("b_int32") {
        b_int32 const v;
        BTYPES_ASSERTIONS_COMMON(int32);
        BTYPES_ASSERTIONS_SINGLE(int32, int32_a, int32_b);
    }

    SECTION("b_timestamp") {
        b_timestamp const v;
        BTYPES_ASSERTIONS_COMMON(timestamp);
    }

    SECTION("b_int64") {
        b_int64 const v;
        BTYPES_ASSERTIONS_COMMON(int64);
        BTYPES_ASSERTIONS_SINGLE(int64, int64_a, int64_b);
    }

    SECTION("b_decimal128") {
        b_decimal128 const v;
        BTYPES_ASSERTIONS_COMMON(decimal128);
        BTYPES_ASSERTIONS_SINGLE(decimal128, d128_a, d128_b);
    }

    SECTION("b_maxkey") {
        b_maxkey const v;
        BTYPES_ASSERTIONS_COMMON(maxkey);
    }

    // BSONCXX_V1_TYPES_XMACRO: update above.
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][types][view]") {
    // BSONCXX_V1_TYPES_XMACRO: update below.
    {
        bsoncxx::v1::stdx::string_view sv;
        bsoncxx::v1::oid o{"507f1f77bcf86cd799439011"};

        CHECK(bsoncxx::test::stringify(b_double{}) == "0.0");
        CHECK(bsoncxx::test::stringify(b_string{}) == R"("")");
        CHECK(bsoncxx::test::stringify(b_document{}) == "{}");
        CHECK(bsoncxx::test::stringify(b_array{}) == "[]");
        CHECK(bsoncxx::test::stringify(b_binary{}) == "empty");
        CHECK(bsoncxx::test::stringify(b_undefined{}) == "undefined");
        CHECK(bsoncxx::test::stringify(b_oid{o}) == "507f1f77bcf86cd799439011");
        CHECK(bsoncxx::test::stringify(b_bool{}) == "false");
        CHECK(bsoncxx::test::stringify(b_date{}) == "1970-01-01T00:00:00Z");
        CHECK(bsoncxx::test::stringify(b_null{}) == "null");
        CHECK(bsoncxx::test::stringify(b_regex{"regex"}) == R"({"pattern": "regex", "options": ""})");
        CHECK(bsoncxx::test::stringify(b_dbpointer{sv, o}) == R"({"$ref": "", "$oid": 507f1f77bcf86cd799439011})");
        CHECK(bsoncxx::test::stringify(b_code{"code"}) == R"("code")");
        CHECK(bsoncxx::test::stringify(b_symbol{"symbol"}) == R"("symbol")");
        CHECK(bsoncxx::test::stringify(b_codewscope{}) == R"({"code": "", "scope": {}})");
        CHECK(bsoncxx::test::stringify(b_int32{}) == "0");
        CHECK(bsoncxx::test::stringify(b_timestamp{}) == R"({"t": 0, "i": 0})");
        CHECK(bsoncxx::test::stringify(b_int64{}) == "0");
        CHECK(bsoncxx::test::stringify(b_decimal128{}) == "0E-6176");
        CHECK(bsoncxx::test::stringify(b_maxkey{}) == "maxkey");
        CHECK(bsoncxx::test::stringify(b_minkey{}) == "minkey");
    }
    // BSONCXX_V1_TYPES_XMACRO: update above.

#pragma push_macro("X")
#undef X
#define X(_name, _value) b_##_name{},

    view const views[] = {BSONCXX_V1_TYPES_XMACRO(X)};
#pragma pop_macro("X")

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                                                 \
    case id::k_##_name:                                                                  \
        CHECK(bsoncxx::test::stringify(v) == bsoncxx::test::stringify(v.get_##_name())); \
        break;

    for (auto const& v : views) {
        switch (v.type_id()) {
            BSONCXX_V1_TYPES_XMACRO(X)

            default:
                FAIL();
        }
    }
#pragma pop_macro("X")
}

namespace static_assertions {

// BSONCXX_V1_TYPES_XMACRO: update below

namespace single_value_types {

// BType -> Value is implicit, but Value -> BType is explicit.
template <typename BType, typename Value>
struct one_way_implicit_convertible {
    template <typename From, typename To>
    struct is_explicitly_convertible : bsoncxx::detail::conjunction<
                                           std::is_constructible<To, From>,
                                           bsoncxx::detail::negation<std::is_convertible<From, To>>> {};

    template <typename From, typename To>
    struct is_implicitly_convertible
        : bsoncxx::detail::conjunction<std::is_constructible<To, From>, std::is_convertible<From, To>> {};

    static_assert(is_explicitly_convertible<Value, BType>::value, "must be explicit");
    static_assert(is_implicitly_convertible<BType, Value>::value, "must be implicit");
};

// b_minkey
template struct one_way_implicit_convertible<b_double, double>;
template struct one_way_implicit_convertible<b_string, bsoncxx::v1::stdx::string_view>;
template struct one_way_implicit_convertible<b_document, bsoncxx::v1::document::view>;
template struct one_way_implicit_convertible<b_array, bsoncxx::v1::array::view>;
// b_binary
// b_undefined
template struct one_way_implicit_convertible<b_oid, bsoncxx::v1::oid>;
template struct one_way_implicit_convertible<b_bool, bool>;
template struct one_way_implicit_convertible<b_date, std::chrono::milliseconds>;
// b_null
// b_regex
// b_dbpointer
template struct one_way_implicit_convertible<b_code, bsoncxx::v1::stdx::string_view>;
template struct one_way_implicit_convertible<b_symbol, bsoncxx::v1::stdx::string_view>;
// b_codewscope
template struct one_way_implicit_convertible<b_int32, std::int32_t>;
// b_timestamp
template struct one_way_implicit_convertible<b_int64, std::int64_t>;
template struct one_way_implicit_convertible<b_decimal128, bsoncxx::v1::decimal128>;
// maxkey

} // namespace single_value_types

// BSONCXX_V1_TYPES_XMACRO: update above

} // namespace static_assertions

} // namespace
