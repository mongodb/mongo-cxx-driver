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

#include <mongocxx/v1/server_api.hpp>

//

#include <mongocxx/v1/exception.hpp>

#include <climits>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = server_api::errc;

TEST_CASE("error code", "[bsoncxx][v1][server_api][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = mongocxx::v1::server_api::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::server_api"));

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
        std::error_code const ec = code::invalid_version;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_version) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_version) == type_errc::invalid_argument);
    }
}

TEST_CASE("exceptions", "[bsoncxx][v1][server_api]") {
    SECTION("version_to_string") {
        CHECK_THROWS_WITH_CODE(
            server_api::version_to_string(static_cast<server_api::version>(1)), code::invalid_version);
    }

    SECTION("version_from_string") {
        CHECK_THROWS_WITH_CODE(server_api::version_from_string("invalid"), code::invalid_version);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][server_api]") {
    server_api source{server_api::version::k_version_1};
    server_api target{static_cast<server_api::version>(1)};

    CHECK(source.get_version() == server_api::version::k_version_1);
    CHECK(target.get_version() == static_cast<server_api::version>(1));

    auto const source_value = source.get_version();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.get_version() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.get_version() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.get_version() == source_value);
        CHECK(copy.get_version() == source_value);

        target = copy;

        CHECK(copy.get_version() == source_value);
        CHECK(target.get_version() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][server_api]") {
    server_api const api{server_api::version::k_version_1};

    CHECK_FALSE(api.strict().has_value());
    CHECK_FALSE(api.deprecation_errors().has_value());
    CHECK(api.get_version() == server_api::version::k_version_1);
}

TEST_CASE("version_to_string", "[mongocxx][v1][server_api]") {
    using T = server_api::version;

    SECTION("invalid") {
        auto const v = GENERATE(values({
            static_cast<T>(INT_MIN),
            static_cast<T>(-1),
            static_cast<T>(1),
            static_cast<T>(INT_MAX),
        }));
        CAPTURE(v);

        CHECK_THROWS_WITH_CODE(server_api::version_to_string(v), code::invalid_version);
    }

    SECTION("valid") {
        CHECK(server_api::version_to_string(T::k_version_1) == "1");
    }
}

TEST_CASE("version_from_string", "[mongocxx][v1][server_api]") {
    using T = server_api::version;

    SECTION("invalid") {
        auto const v = GENERATE(values({
            "",
            "x",
            "abc",
            "-1",
            "0",
            "2",
        }));
        CAPTURE(v);

        CHECK_THROWS_WITH_CODE(server_api::version_from_string(v), code::invalid_version);
    }

    SECTION("valid") {
        CHECK(server_api::version_from_string("1") == T::k_version_1);
    }
}

TEST_CASE("strict", "[mongocxx][v1][server_api]") {
    auto const v = GENERATE(false, true);
    server_api api{server_api::version::k_version_1};
    CHECK(api.strict(v).strict() == v);
}

TEST_CASE("deprecation_errors", "[mongocxx][v1][server_api]") {
    auto const v = GENERATE(false, true);
    server_api api{server_api::version::k_version_1};
    CHECK(api.deprecation_errors(v).deprecation_errors() == v);
}

TEST_CASE("get_version", "[mongocxx][v1][server_api]") {
    using T = int;

    auto const v = GENERATE(values({
        T{INT_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT_MAX},
    }));
    CAPTURE(v);

    auto const version = static_cast<server_api::version>(v);

    CHECK(server_api{version}.get_version() == version);
}

} // namespace v1
} // namespace mongocxx
