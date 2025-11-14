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

#include <bsoncxx/test/v1/oid.hh>

//

#include <bsoncxx/test/v1/exception.hh>

#include <cstdint>
#include <ctime>
#include <string>
#include <system_error>
#include <vector>

#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using bsoncxx::v1::oid;
using code = bsoncxx::v1::oid::errc;

TEST_CASE("error code", "[bsoncxx][v1][oid][error]") {
    using bsoncxx::v1::source_errc;
    using bsoncxx::v1::type_errc;

    auto const& category = bsoncxx::v1::oid::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("bsoncxx::v1::oid"));

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
        CHECK(make_error_code(code::null_bytes_ptr) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_length) == source_errc::bsoncxx);
        CHECK(make_error_code(code::empty_string) == source_errc::bsoncxx);
        CHECK(make_error_code(code::invalid_string) == source_errc::bsoncxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::null_bytes_ptr) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_length) == type_errc::invalid_argument);
        CHECK(make_error_code(code::empty_string) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_string) == type_errc::invalid_argument);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][oid]") {
    SECTION("null_bytes_ptr") {
        auto const expr = [] { oid o{nullptr, 0u}; };

        CHECK_THROWS_WITH_CODE(expr(), code::null_bytes_ptr);
    }

    SECTION("invalid_length") {
        static constexpr auto k_oid_length = bsoncxx::v1::oid::k_oid_length;

        std::uint8_t bytes[k_oid_length + 1u];

        SECTION("too short") {
            auto const expr = [&] { oid o{bytes, bsoncxx::v1::oid::k_oid_length - 1u}; };

            CHECK_THROWS_WITH_CODE(expr(), code::invalid_length);
        }

        SECTION("too long") {
            auto const expr = [&] { oid o{bytes, bsoncxx::v1::oid::k_oid_length + 1u}; };

            CHECK_THROWS_WITH_CODE(expr(), code::invalid_length);
        }
    }

    SECTION("empty_string") {
        auto const expr = [&] { oid o{""}; };

        CHECK_THROWS_WITH_CODE(expr(), code::empty_string);
    }

    SECTION("invalid_string") {
        auto const expr = [] { oid o{"invalid"}; };

        CHECK_THROWS_WITH_CODE(expr(), code::invalid_string);
    }
}

TEST_CASE("basic", "[bsoncxx][v1][oid]") {
    SECTION("default") {
        CHECK(oid{} != oid{}); // Random and unique per process.
    }

    SECTION("zero") {
        unsigned char const zeroes[oid::k_oid_length]{};
        oid o{zeroes, sizeof(zeroes)};

        CHECK(oid::size() == oid::k_oid_length);
        CHECK(o.size() == oid::size());
        CHECK(o.size() == 12);

        {
            std::time_t time = o.get_time_t();
            char str[sizeof("YYYY-MM-DD HH:MM:SS")];
            CHECK(std::strftime(str, sizeof(str), "%F %T", std::gmtime(&time)) == sizeof(str) - 1u);
            CHECK(std::string(str) == "1970-01-01 00:00:00");
        }

        std::vector<int> const bytes{o.bytes(), o.bytes() + o.size()};

        CHECK(bytes == std::vector<int>(oid::k_oid_length));
    }

    SECTION("values") {
        // Timestamp: 946771199 (0x386e94ff)
        // Value:     286462997 (0x11131415)
        // Counter:   2171427   (0x212223)
        oid const o{"386e94ff1112131415212223"};

        CHECK(o == o);
        CHECK_FALSE(o != o);
        CHECK_FALSE(o > o);
        CHECK_FALSE(o < o);
        CHECK(o >= o);
        CHECK(o <= o);

        {
            std::time_t time = o.get_time_t();
            char str[sizeof("YYYY-MM-DD HH:MM:SS")];
            CHECK(std::strftime(str, sizeof(str), "%F %T", std::gmtime(&time)) == sizeof(str) - 1u);
            CHECK(std::string(str) == "2000-01-01 23:59:59");
        }

        CHECK(o < oid{"389622001112131415212223"}); // Timestamp: 2000-02-01 00:00:00
        CHECK(o > oid{"386d43801112131415212223"}); // Timestamp: 2000-01-01 00:00:00
        CHECK(o < oid{"386e94ffffffffffff212223"}); // Value: 1099511627775
        CHECK(o > oid{"386e94ff0000000000212223"}); // Value: 0
        CHECK(o < oid{"386e94ff1112131415ffffff"}); // Counter: 16777215
        CHECK(o > oid{"386e94ff1112131415000000"}); // Counter: 0
    }
}

TEST_CASE("stringify", "[bsoncxx][test][v1][oid]") {
    oid o{"507f1f77bcf86cd799439011"};
    CHECK(bsoncxx::test::stringify(o) == "507f1f77bcf86cd799439011");
}

} // namespace
