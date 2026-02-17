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

#include <mongocxx/v1/gridfs/downloader.hh>

//

#include <mongocxx/v1/exception.hpp>

#include <string>
#include <system_error>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {
namespace gridfs {

using code = downloader::errc;

TEST_CASE("error code", "[mongocxx][v1][gridfs][downloader][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = downloader::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::gridfs::downloader"));

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
        std::error_code const ec = code::is_closed;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::is_closed) == source_errc::mongocxx);
        CHECK(make_error_code(code::corrupt_data) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::is_closed) == type_errc::runtime_error);
        CHECK(make_error_code(code::corrupt_data) == type_errc::runtime_error);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][gridfs][downloader]") {
    auto source = v1::gridfs::downloader::internal::make();
    auto target = v1::gridfs::downloader::internal::make();

    source.close();

    REQUIRE_FALSE(source.is_open());
    REQUIRE(target.is_open());

    SECTION("move") {
        auto move = std::move(source);

        CHECK_FALSE(source);

        REQUIRE(move);
        CHECK_FALSE(move.is_open());

        target = std::move(move);

        CHECK_FALSE(move);

        REQUIRE(target);
        CHECK_FALSE(target.is_open());
    }
}

TEST_CASE("default", "[mongocxx][v1][gridfs][downloader]") {
    downloader const v;

    CHECK_FALSE(v);
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
