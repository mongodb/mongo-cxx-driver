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

#include <bsoncxx/test/v1/decimal128.hh>

//

#include <bsoncxx/test/v1/exception.hh>

#include <climits>
#include <memory>
#include <string>
#include <system_error>

#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using bsoncxx::v1::decimal128;
using code = bsoncxx::v1::decimal128::errc;

TEST_CASE("error code", "[bsoncxx][v1][decimal128][error]") {
    using bsoncxx::v1::source_errc;
    using bsoncxx::v1::type_errc;

    auto const& category = bsoncxx::v1::decimal128::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("bsoncxx::v1::decimal128"));

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
        std::error_code const ec = code::empty_string;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::empty_string) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_string_length) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_string_data) == source_errc::bsoncxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::empty_string) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_string_length) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_string_data) == type_errc::invalid_argument);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][decimal128]") {
    SECTION("empty_string") {
        auto const expr = [] { decimal128 d128{bsoncxx::v1::stdx::string_view()}; };

        CHECK_THROWS_WITH_CODE(expr(), code::empty_string);
    }

    SECTION("invalid_string_length") {
        try {
            auto const size = std::size_t{INT_MAX} + 1u;
            std::unique_ptr<char[]> data{new char[size]}; // make_unique_for_overwrite
            auto const big_string = bsoncxx::v1::stdx::string_view{data.get(), size};

            auto const expr = [&] { decimal128 d128{big_string}; };

            CHECK_THROWS_WITH_CODE(expr(), code::invalid_string_length);
        } catch (std::bad_alloc const& ex) {
            WARN("could not allocate big_string: " << ex.what());
        }
    }

    SECTION("invalid_string") {
        auto const expr = [] { decimal128 d128{"sNaN"}; };

        CHECK_THROWS_WITH_CODE(expr(), code::invalid_string_data);
    }
}

TEST_CASE("basic", "[bsoncxx][v1][decimal128]") {
    SECTION("default") {
        decimal128 d128;

        CHECK(d128.high() == 0u);
        CHECK(d128.low() == 0u);

        CHECK(d128.to_string() == "0E-6176");

        CHECK(d128 == d128);
        CHECK_FALSE(d128 != d128);
    }

    SECTION("values") {
        using d128 = decimal128;

        CHECK((d128{0u, 0u}) == d128{});
        CHECK((d128{0u, 0u}) != d128{0u, 1u});
        CHECK((d128{0u, 0u}) != d128{1u, 0u});
        CHECK((d128{1u, 1u}) == d128{1u, 1u});

        // [0,0,0]
        CHECK((d128{0x3040000000000000, 0x0000000000000000}) == d128{"0"});

        // [0,123,0]
        CHECK((d128{0x3040000000000000, 0x000000000000007b}) == d128{"123"});

        // [0,inf]
        CHECK((d128{0x7800000000000000, 0x0000000000000000}) == d128{"Infinity"});
    }
}

TEST_CASE("stringify", "[bsoncxx][test][v1][decimal128]") {
    decimal128 d128;
    CHECK(d128.to_string() == "0E-6176");
    CHECK(bsoncxx::test::stringify(d128) == "0E-6176");
}

} // namespace
