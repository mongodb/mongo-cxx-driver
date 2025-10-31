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

#include <bsoncxx/test/v1/document/view.hh>

//

#include <bsoncxx/test/v1/element/view.hh>
#include <bsoncxx/test/v1/exception.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>
#include <bsoncxx/test/v1/types/id.hh>
#include <bsoncxx/test/v1/types/value.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <system_error>

#include <bsoncxx/private/make_unique.hh>

#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using bsoncxx::v1::document::view;
using code = bsoncxx::v1::document::view::errc;

TEST_CASE("error code", "[bsoncxx][v1][document][view][error]") {
    using bsoncxx::v1::source_errc;
    using bsoncxx::v1::type_errc;

    auto const& category = bsoncxx::v1::document::view::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("bsoncxx::v1::document::view"));

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
        std::error_code const ec = code::invalid_length;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_length) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_data) == source_errc::bsoncxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_length) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_data) == type_errc::runtime_error);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][document][view]") {
    auto const empty_key = bsoncxx::v1::stdx::string_view{};

    SECTION("invalid_view") {
        view v{nullptr};

        REQUIRE_NOTHROW(v.end());
        REQUIRE_NOTHROW(v[empty_key]);

        CHECK(v.begin() == v.end());
        CHECK(v.find(empty_key) == v.end());
    }

    SECTION("invalid_length") {
        std::uint8_t data[] = {5, 0, 0, 0, 0}; // {}
        auto const data_ptr = data;

        REQUIRE(data[0] == sizeof(data));

        auto const expr = [&data](std::size_t length) { return view{data, length}; };

        CHECK_THROWS_WITH_CODE(expr(0), code::invalid_length);
        CHECK_THROWS_WITH_CODE(expr(sizeof(data) - 1u), code::invalid_length);

        CHECK(expr(sizeof(data)).data() == data_ptr);
        CHECK(expr(sizeof(data) + 1u).data() == data_ptr);

        ++data[0];

        CHECK_THROWS_WITH_CODE((view{data, sizeof(data)}), code::invalid_length);
    }

    SECTION("invalid_data") {
        SECTION("view") {
            std::uint8_t const data[] = {5u, 0u, 0u, 0u, 1u};
            view v{data};

            REQUIRE_NOTHROW(v.end());

            CHECK_THROWS_WITH_CODE(v.begin(), code::invalid_data);
            CHECK_THROWS_WITH_CODE(v.find(empty_key), code::invalid_data);
        }

        SECTION("const_iterator") {
            std::uint8_t data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // { 'x': 1 }
            view const v{data};
            auto iter = v.begin();
            REQUIRE(iter != v.end());

            std::memset(data, 0u, sizeof(data));

            REQUIRE_NOTHROW(v.end());

            CHECK_THROWS_WITH_CODE(++iter, code::invalid_data);
        }
    }

    SECTION("invalid_key_length") {
        try {
            auto const size = std::size_t{INT_MAX} + 1u;
            auto const data = bsoncxx::make_unique_for_overwrite<char[]>(size);
            auto const big_string = bsoncxx::v1::stdx::string_view{data.get(), size};

            view const v;

            CHECK(v.find(big_string) == v.end());
        } catch (std::bad_alloc const& ex) {
            WARN("could not allocate big_string: " << ex.what());
        }
    }
}

TEST_CASE("basic", "[bsoncxx][v1][document][view]") {
    auto const invalid = view{nullptr};
    auto const empty_key = bsoncxx::v1::stdx::string_view{};

    SECTION("invalid") {
        REQUIRE_FALSE(invalid);

        CHECK(invalid.data() == nullptr);
        CHECK(invalid.size() == 0u);
        CHECK_FALSE(invalid.empty());

        CHECK(invalid.begin() == invalid.begin());
        CHECK(invalid.end() == invalid.end());
        CHECK(invalid.begin() == invalid.end());

        CHECK(invalid.find(empty_key) == invalid.end());
        CHECK_FALSE(invalid[empty_key]);

        CHECK(invalid == invalid);
        CHECK_FALSE(invalid != invalid);
    }

    SECTION("default") {
        view const v;

        REQUIRE(v);

        CHECK(v.data() == view{}.data());
        CHECK(v.size() == 5u);
        CHECK(v.empty());

        CHECK(v.begin() == v.begin());
        CHECK(v.end() == v.end());
        CHECK(v.begin() == v.end());

        CHECK(v.find(empty_key) == v.end());
        CHECK_FALSE(v[empty_key]);

        CHECK(v == v);
        CHECK_FALSE(v != v);

        CHECK_FALSE(v == invalid);
        CHECK(v != invalid);
        CHECK(v.begin() == invalid.begin());
        CHECK(v.end() == invalid.end());
    }

    SECTION("valid") {
        // { 'x': 1, 'y': 2 }
        std::uint8_t const data[] = {19, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 16, 'y', '\0', 2, 0, 0, 0, 0};
        view const v{data};

        REQUIRE(v);

        CHECK(v != view{});

        CHECK(v.data() == data);
        CHECK(v.size() == sizeof(data));
        CHECK_FALSE(v.empty());

        CHECK(v.begin() == v.begin());
        CHECK(v.end() == v.end());
        CHECK(v.begin() != v.end());

        CHECK(v == v);
        CHECK_FALSE(v != v);

        CHECK_FALSE(v == invalid);
        CHECK(v != invalid);
        CHECK(v.begin() != invalid.begin());
        CHECK(v.end() == invalid.end());

        CHECK(v.find("x") != v.end());
        CHECK(v.find("y") != v.end());
        CHECK(v.find("z") == v.end());
    }

    SECTION("iterator") {
        // { 'x': 1, 'y': 2 }
        std::uint8_t const data[] = {19, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 16, 'y', '\0', 2, 0, 0, 0, 0};
        view const v{data};

        auto iter = v.begin(); // x: 1

        {
            auto const citer = v.begin();

            REQUIRE(citer != v.end());
            REQUIRE(*citer);

            CHECK(citer->raw() == data);
            CHECK(citer->length() == v.length());
            CHECK(citer->offset() == 4u);
            CHECK(citer->keylen() == 1u);

            CHECK(citer->type_id() == bsoncxx::v1::types::id::k_int32);
            CHECK(citer->key() == "x");

            CHECK(citer->get_int32() == bsoncxx::v1::types::b_int32{1});
            CHECK_THROWS_WITH_CODE(citer->get_document(), bsoncxx::v1::types::view::errc::type_mismatch);
            CHECK_THROWS_WITH_CODE(citer->get_array(), bsoncxx::v1::types::view::errc::type_mismatch);

            CHECK(citer->type_view() == bsoncxx::v1::types::b_int32{1});
            CHECK(citer->type_value() == citer->type_view());

            CHECK_FALSE((*citer)[empty_key]);
            CHECK_FALSE((*citer)[0]);
        }

        {
            auto pre = iter;
            auto post = iter;

            REQUIRE(pre == iter);
            REQUIRE(post == iter);

            CHECK(++pre != iter);
            CHECK(post++ == iter);

            CHECK(pre == post);

            CHECK(pre != v.end());
            CHECK(post != v.end());

            REQUIRE(*pre);

            CHECK(pre->raw() == data);
            CHECK(pre->length() == v.length());
            CHECK(pre->offset() == 11u);
            CHECK(pre->keylen() == 1u);

            CHECK(pre->type_id() == bsoncxx::v1::types::id::k_int32);
            CHECK(pre->key() == "y");
            CHECK(pre->get_int32() == bsoncxx::v1::types::b_int32{2});
        }

        ++iter; // y: 2

        {
            auto pre = iter;
            auto post = iter;

            REQUIRE(pre == iter);
            REQUIRE(post == iter);

            CHECK(++pre != iter);
            CHECK(post++ == iter);

            CHECK(pre == post);

            CHECK(pre == v.end());
            CHECK(post == v.end());

            CHECK_FALSE(*pre);

            CHECK(pre->raw() == nullptr);
            CHECK(pre->length() == 0u);
            CHECK(pre->offset() == 0u);
            CHECK(pre->keylen() == 0u);
        }

        iter = v.end();

        {
            auto pre = iter;
            auto post = iter;

            REQUIRE(pre == iter);
            REQUIRE(post == iter);

            CHECK(++pre == iter);
            CHECK(post++ == iter);

            CHECK(pre == post);

            CHECK(pre == v.end());
            CHECK(post == v.end());

            CHECK_FALSE(*pre);
        }
    }
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][document][view]") {
    SECTION("invalid") {
        view doc{nullptr};

        REQUIRE_FALSE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == "invalid");
    }

    SECTION("empty") {
        view doc;

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"({})");
    }

    SECTION("one") {
        // {"x": 1}
        std::uint8_t const bytes[] = {
            // clang-format off
            12, 0, 0, 0,
            16, 'x', '\0', 1, 0, 0, 0, // "x": 1
            0,
            // clang-format on
        };

        view doc{bytes};

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"({"x": 1})");
    }

    SECTION("two") {
        // {"x": 1, "y": 2}
        std::uint8_t const bytes[] = {
            // clang-format off
            19, 0, 0, 0,
            16, 'x', '\0', 1, 0, 0, 0, // "x": 1
            16, 'y', '\0', 2, 0, 0, 0, // "y": 2
            0,
            // clang-format on
        };

        view doc{bytes};

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"({"x": 1, "y": 2})");
    }

    SECTION("three") {
        // {"x": 1, "y": 2, "z": 3}
        std::uint8_t const bytes[] = {
            // clang-format off
            26, 0, 0, 0,
            16, 'x', '\0', 1, 0, 0, 0, // "x": 1
            16, 'y', '\0', 2, 0, 0, 0, // "y": 2
            16, 'z', '\0', 3, 0, 0, 0, // "z": 3
            0,
            // clang-format on
        };

        view doc{bytes};

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"({"x": 1, "y": 2, "z": 3})");
    }
}

} // namespace

std::string Catch::StringMaker<bsoncxx::v1::document::view>::convert(bsoncxx::v1::document::view const& value) try {
    if (!value) {
        return "invalid";
    }

    auto const end = value.end();
    auto iter = value.begin();

    if (iter == end) {
        return "{}";
    }

    std::string res;
    res += '{';
    res += bsoncxx::test::stringify(*iter);
    for (++iter; iter != end; ++iter) {
        res += ", ";
        res += bsoncxx::test::stringify(*iter);
    }
    res += '}';
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    WARN("exception during stringification: " << ex.what());
    if (ex.code() == code::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}
