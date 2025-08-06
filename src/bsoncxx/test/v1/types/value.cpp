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

#include <bsoncxx/test/v1/types/value.hh>

//

#include <bsoncxx/v1/detail/type_traits.hpp>

#include <bsoncxx/v1/types/view.hh>

#include <bsoncxx/test/v1/detail/bit.hh>
#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/exception.hh>
#include <bsoncxx/test/v1/types/id.hh>
#include <bsoncxx/test/v1/types/value.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <climits>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using namespace bsoncxx::v1::types;
using code = bsoncxx::v1::types::value::errc;

TEST_CASE("error code", "[bsoncxx][v1][types][value][error]") {
    using bsoncxx::v1::source_errc;
    using bsoncxx::v1::type_errc;

    auto const& category = bsoncxx::v1::types::value::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("bsoncxx::v1::types::value"));

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
        std::error_code const ec = code::invalid_type;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_type) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_length_u32) == source_errc::bsoncxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_type) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_length_u32) == type_errc::invalid_argument);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][types][value]") {
    using namespace bsoncxx::v1::types;

    SECTION("invalid_type") {
        bsoncxx::v1::types::view v;

        bsoncxx::v1::types::view::internal::type_id(v, bsoncxx::v1::types::id{}); // BSON_TYPE_EOD

        CHECK_THROWS_WITH_CODE(value{v}, code::invalid_type);
    }

    SECTION("invalid_length_u32") {
        try {
            auto const size = std::size_t{UINT32_MAX} + 1u;
            std::unique_ptr<unsigned char[]> data{new unsigned char[size]}; // make_unique_for_overwrite.

            auto const big_string = bsoncxx::v1::stdx::string_view{reinterpret_cast<char const*>(data.get()), size};

            REQUIRE(big_string.size() == size);

            SECTION("b_string") {
                auto const expr = [&] { value v{b_string{big_string}}; };
                CHECK_THROWS_WITH_CODE(expr(), code::invalid_length_u32);
            }

            SECTION("b_dbpointer") {
                auto const expr = [&] { value v{b_dbpointer{big_string, bsoncxx::v1::oid{}}}; };
                CHECK_THROWS_WITH_CODE(expr(), code::invalid_length_u32);
            }

            SECTION("b_code") {
                auto const expr = [&] { value v{b_code{big_string}}; };
                CHECK_THROWS_WITH_CODE(expr(), code::invalid_length_u32);
            }

            SECTION("b_symbol") {
                auto const expr = [&] { value v{b_symbol{big_string}}; };
                CHECK_THROWS_WITH_CODE(expr(), code::invalid_length_u32);
            }

            SECTION("b_codewscope") {
                auto const expr = [&] { value v{b_codewscope{big_string, bsoncxx::v1::document::view{}}}; };
                CHECK_THROWS_WITH_CODE(expr(), code::invalid_length_u32);
            }

            SECTION("bytes") {
                auto const expr = [&] { value v{nullptr, std::size_t{UINT32_MAX} + 1u, binary_subtype::k_binary}; };
                CHECK_THROWS_WITH_CODE(expr(), code::invalid_length_u32);
            }
        } catch (std::bad_alloc const& ex) {
            WARN("could not allocate big data: " << ex.what());
        }
    }
}

TEST_CASE("ownership", "[bsoncxx][v1][types][value]") {
    value target{"old"};
    value source{"new"};

    REQUIRE(source.type_id() == id::k_string);
    REQUIRE(target.type_id() == id::k_string);

    auto const data = source.get_string().value.data(); // "new"

    SECTION("move") {
        auto move = std::move(source);

        CHECK(source.type_id() == id::k_null);

        REQUIRE(move.type_id() == id::k_string);
        CHECK(move.get_string().value.data() == data);
        CHECK(move.get_string().value == "new");

        target = std::move(move);

        CHECK(move.type_id() == id::k_null);

        REQUIRE(target.type_id() == id::k_string);
        CHECK(target.get_string().value.data() == data);
        CHECK(target.get_string().value == "new");
    }

    SECTION("copy") {
        auto copy = source;

        REQUIRE(source.type_id() == id::k_string);
        CHECK(source.get_string().value.data() == data);
        CHECK(source.get_string().value == "new");

        REQUIRE(copy.type_id() == id::k_string);
        auto const copy_data = copy.get_string().value.data();
        CHECK(copy.get_string().value == "new");
        CHECK(copy_data != data);

        target = copy;

        REQUIRE(copy.type_id() == id::k_string);
        CHECK(copy.get_string().value == copy_data);

        REQUIRE(target.type_id() == id::k_string);
        auto const target_data = target.get_string().value.data();
        CHECK(target.get_string().value == "new");
        CHECK(target_data != data);
        CHECK(target_data != copy_data);
    }
}

TEST_CASE("basic", "[bsoncxx][v1][types][value]") {
    SECTION("default") {
        value v;

        CHECK(v.type_id() == id::k_null);

        CHECK(v == v);
        CHECK_FALSE(v != v);

        CHECK(v == value{});
        CHECK_FALSE(v != value{});

        CHECK_FALSE(v == value{1});
        CHECK_FALSE(value{1} == v);
        CHECK(v != value{1});
        CHECK(value{1} != v);

        CHECK(v == view{});
        CHECK_FALSE(v != view{});

        CHECK_FALSE(v == view{b_int32{1}});
        CHECK_FALSE(view{b_int32{1}} == v);
        CHECK(v != view{b_int32{1}});
        CHECK(view{b_int32{1}} != v);
    }

    SECTION("value") {
        value v{1};

        CHECK(v.type_id() == id::k_int32);

        CHECK(v == v);
        CHECK_FALSE(v != v);

        CHECK_FALSE(v == value{});
        CHECK(v != value{});

        CHECK(v == value{1});
        CHECK(value{1} == v);
        CHECK_FALSE(v != value{1});
        CHECK_FALSE(value{1} != v);

        CHECK_FALSE(v == view{});
        CHECK(v != view{});

        CHECK(v == view{b_int32{1}});
        CHECK(view{b_int32{1}} == v);
        CHECK_FALSE(v != view{b_int32{1}});
        CHECK_FALSE(view{b_int32{1}} != v);
    }
}

TEST_CASE("b_types", "[bsoncxx][v1][types][value]") {
    bsoncxx::v1::oid const o{"507f1f77bcf86cd799439011"};
    std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
    bsoncxx::v1::document::view const doc{data};
    bsoncxx::v1::array::view const arr{data};

    // BSONCXX_V1_TYPES_XMACRO: update below.
    view const views[] = {
        b_double{},
        b_string{"string"},
        b_document{doc},
        b_array{arr},
        b_binary{binary_subtype::k_binary, sizeof(data), data},
        b_undefined{},
        b_oid{o},
        b_bool{},
        b_date{},
        b_null{},
        b_regex{"regex"},
        b_dbpointer{"dbpointer", o},
        b_code{"code"},
        b_symbol{"symbol"},
        b_codewscope{"code", doc},
        b_int32{},
        b_timestamp{},
        b_int64{},
        b_decimal128{},
        b_maxkey{},
        b_minkey{},
    };
    // BSONCXX_V1_TYPES_XMACRO: update above.

#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case id::k_##_name: {                   \
        /* v1::types::view */               \
        CHECK(value{v} == v);               \
                                            \
        /* v1::types::b_<type> */           \
        CHECK(value{v.get_##_name()} == v); \
    } break;

    for (auto const& v : views) {
        switch (v.type_id()) {
            BSONCXX_V1_TYPES_XMACRO(X)

            default:
                FAIL();
        }
    }
#pragma pop_macro("X")
}

TEST_CASE("constructors", "[bsoncxx][v1][types][value]") {
    auto const k_binary = binary_subtype::k_binary;
    auto const k_encrypted = binary_subtype::k_encrypted;

    bsoncxx::v1::stdx::string_view const sv{"sv"};
    bsoncxx::v1::decimal128 const d{"123"};
    std::chrono::milliseconds const ms{123};
    bsoncxx::v1::oid const o{"507f1f77bcf86cd799439011"};
    std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
    std::uint32_t const data_len{sizeof(data)};
    bsoncxx::v1::document::view const doc{data};
    bsoncxx::v1::array::view const arr{data};
    std::vector<std::uint8_t> const vec{data, data + data_len};

    CHECK(value{nullptr} == view{b_null{}});
    CHECK(value{sv} == view{b_string{sv}});
    CHECK(value{std::int32_t{123}} == view{b_int32{123}});
    CHECK(value{std::int64_t{456}} == view{b_int64{456}});
    CHECK(value{123.456} == view{b_double{123.456}});
    CHECK(value{true} == view{b_bool{true}});
    CHECK(value{d} == view{b_decimal128{d}});
    CHECK(value{ms} == view{b_date{ms}});
    CHECK(value{doc} == view{b_document{doc}});
    CHECK(value{arr} == view{b_array{arr}});
    CHECK(value{vec} == view{(b_binary{k_binary, data_len, data})});
    CHECK(value{vec, k_encrypted} == value{(b_binary{k_encrypted, data_len, vec.data()})});
    CHECK(value{sv, o} == view{b_dbpointer{sv, o}});
    CHECK(value{sv, doc} == view{b_codewscope{sv, doc}});
    CHECK(value{sv, sv} == view{b_regex{sv, sv}});
    CHECK(value{sv.data()} == view{b_string{sv}});
    CHECK(value{std::string{sv}} == view{b_string{sv}});
    CHECK(value{data, sizeof(data)} == view{b_binary{k_binary, data_len, data}});
    CHECK(value{data, sizeof(data), k_encrypted} == view{b_binary{k_encrypted, data_len, data}});
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][types][value]") {
    bsoncxx::v1::oid const o{"507f1f77bcf86cd799439011"};
    std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
    bsoncxx::v1::document::view const doc{data};
    bsoncxx::v1::array::view const arr{data};

    // BSONCXX_V1_TYPES_XMACRO: update below.
    view const views[] = {
        b_double{},
        b_string{"string"},
        b_document{doc},
        b_array{arr},
        b_binary{binary_subtype::k_binary, sizeof(data), data},
        b_undefined{},
        b_oid{o},
        b_bool{},
        b_date{},
        b_null{},
        b_regex{"regex"},
        b_dbpointer{"dbpointer", o},
        b_code{"code"},
        b_symbol{"symbol"},
        b_codewscope{"code", doc},
        b_int32{},
        b_timestamp{},
        b_int64{},
        b_decimal128{},
        b_maxkey{},
        b_minkey{},
    };
    // BSONCXX_V1_TYPES_XMACRO: update above.

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                                                        \
    case id::k_##_name:                                                                         \
        CHECK(bsoncxx::test::stringify(value{v}) == bsoncxx::test::stringify(v.get_##_name())); \
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

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wunused-function"));
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunused-template"));
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunneeded-member-function"));

#define IMPLIES(a, b) ((!(a)) || (b))

template <int I>
struct result {};

using bsoncxx::detail::invoke_result_t;

namespace btype_vs_view {

template <typename BType, typename... Args>
struct overload {
    // Prerequisite: Args is not BType or view.
    static_assert(
        IMPLIES(
            (sizeof...(Args) == 1),
            !(bsoncxx::detail::disjunction<std::is_same<BType, Args>..., std::is_same<view, Args>...>::value)),
        "Arg must not be BType or view");

    // Prerequisite: BType(Args...) is valid.
    static_assert(std::is_constructible<BType, Args...>::value, "BType(Args...) must be valid");

    static result<1> fn(BType) {
        return {};
    }

    static result<2> fn(view) {
        return {};
    }

    template <typename... Ts>
    using non_list_expr = decltype((fn)(std::declval<Ts>()...));

    template <typename... Ts>
    using list_expr = decltype((fn)({std::declval<Ts>()...}));

    // `fn({})` is always ambiguous.
    static_assert(!bsoncxx::detail::is_detected<list_expr>::value, "must be ill-formed");

    // `fn(arg)` is always ambiguous.
    static_assert(
        IMPLIES((sizeof...(Args) == 1), !(bsoncxx::detail::is_detected<non_list_expr, Args...>::value)),
        "must be ill-formed");

    // `fn({})` and `fn({arg})` is always ambiguous, but not `fn({args...})`.
    static_assert(
        IMPLIES((sizeof...(Args) < 2), !(bsoncxx::detail::is_detected<list_expr, Args...>::value)),
        "must be ill-formed");
};

using bsoncxx::v1::stdx::string_view;

// BSONCXX_V1_TYPES_XMACRO: update below

template struct overload<b_double, double>;
template struct overload<b_string, string_view>;
template struct overload<b_document, bsoncxx::v1::document::view>;
template struct overload<b_array, bsoncxx::v1::array::view>;
template struct overload<b_binary, binary_subtype, std::uint32_t, std::uint8_t const*>;
template struct overload<b_undefined>;
template struct overload<b_oid, bsoncxx::v1::oid>;
template struct overload<b_bool, bool>;
template struct overload<b_date, std::chrono::milliseconds>;
template struct overload<b_null>;
template struct overload<b_regex, string_view>;
template struct overload<b_regex, string_view, string_view>;
template struct overload<b_dbpointer, string_view, bsoncxx::v1::oid>;
template struct overload<b_code, string_view>;
template struct overload<b_symbol, string_view>;
template struct overload<b_codewscope, string_view, bsoncxx::v1::document::view>;
template struct overload<b_int32, std::int32_t>;
template struct overload<b_timestamp, std::int32_t, std::int32_t>;
template struct overload<b_int64, std::int64_t>;
template struct overload<b_decimal128, bsoncxx::v1::decimal128>;
template struct overload<b_maxkey>;
template struct overload<b_minkey>;

// BSONCXX_V1_TYPES_XMACRO: update above

} // namespace btype_vs_view

namespace btype_vs_value {

template <typename BType, typename... Args>
struct overload {
    // Prerequisite: Args is not BType or value.
    static_assert(
        IMPLIES(
            (sizeof...(Args) == 1),
            !(bsoncxx::detail::disjunction<std::is_same<BType, Args>..., std::is_same<value, Args>...>::value)),
        "Arg must not be BType or value");

    // Prerequisite: BType(Args...) is valid.
    static_assert(std::is_constructible<BType, Args...>::value, "BType(Args...) must be valid");

    static result<1> fn(BType) {
        return {};
    }

    static result<2> fn(value) {
        return {};
    }

    template <typename... Ts>
    using non_list_expr = decltype((fn)(std::declval<Ts>()...));

    template <typename... Ts>
    using list_expr = decltype((fn)({std::declval<Ts>()...}));

    // `fn({})` is always ambiguous.
    static_assert(!bsoncxx::detail::is_detected<list_expr>::value, "must be ill-formed");

    // `fn(arg)` is always ambiguous.
    static_assert(
        IMPLIES((sizeof...(Args) == 1), !(bsoncxx::detail::is_detected<non_list_expr, Args...>::value)),
        "must be ill-formed");

    // `fn({})` and `fn({arg})` is always ambiguous, but not `fn({args...})`.
    static_assert(
        IMPLIES((sizeof...(Args) < 2), !(bsoncxx::detail::is_detected<list_expr, Args...>::value)),
        "must be ill-formed");
};

using bsoncxx::v1::stdx::string_view;

// BSONCXX_V1_TYPES_XMACRO: update below

template struct overload<b_double, double>;
template struct overload<b_string, string_view>;
template struct overload<b_document, bsoncxx::v1::document::view>;
template struct overload<b_array, bsoncxx::v1::array::view>;
template struct overload<b_binary, binary_subtype, std::uint32_t, std::uint8_t const*>;
template struct overload<b_undefined>;
template struct overload<b_oid, bsoncxx::v1::oid>;
template struct overload<b_bool, bool>;
template struct overload<b_date, std::chrono::milliseconds>;
template struct overload<b_null>;
template struct overload<b_regex, string_view>;
template struct overload<b_regex, string_view, string_view>;
template struct overload<b_dbpointer, string_view, bsoncxx::v1::oid>;
template struct overload<b_code, string_view>;
template struct overload<b_symbol, string_view>;
template struct overload<b_codewscope, string_view, bsoncxx::v1::document::view>;
template struct overload<b_int32, std::int32_t>;
template struct overload<b_timestamp, std::int32_t, std::int32_t>;
template struct overload<b_int64, std::int64_t>;
template struct overload<b_decimal128, bsoncxx::v1::decimal128>;
template struct overload<b_maxkey>;
template struct overload<b_minkey>;

// BSONCXX_V1_TYPES_XMACRO: update above

} // namespace btype_vs_value

namespace view_vs_value {

result<1> overload(view) {
    return {};
}

result<2> overload(value) {
    return {};
}

static_assert(std::is_same<result<1>, decltype(overload(std::declval<view>()))>::value, "must not be ambiguous");
static_assert(std::is_same<result<2>, decltype(overload(std::declval<value>()))>::value, "must not be ambiguous");

#pragma push_macro("X")
#undef X
#define X(_name, _value) \
    static_assert(       \
        std::is_same<result<1>, decltype(overload(std::declval<b_##_name>()))>::value, "must not be ambiguous");

BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

} // namespace view_vs_value

BSONCXX_PRIVATE_WARNINGS_POP();

} // namespace static_assertions

} // namespace
