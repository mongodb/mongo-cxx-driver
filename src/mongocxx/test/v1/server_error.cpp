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

#include <mongocxx/test/v1/server_error.hh>

//

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/server_error.hh>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <system_error>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][error][server_error]") {
    auto source = server_error::internal::make(1, "one", {});
    auto target = server_error::internal::make(2, "two", {});

    exception::internal::set_error_labels(source, scoped_bson{R"({"errorLabels": ["one"]})"}.view());
    exception::internal::set_error_labels(target, scoped_bson{R"({"errorLabels": ["two"]})"}.view());

    REQUIRE(source.code().value() == 1);
    REQUIRE(source.has_error_label("one"));
    REQUIRE_FALSE(source.has_error_label("two"));

    REQUIRE(target.code().value() == 2);
    REQUIRE_FALSE(target.has_error_label("one"));
    REQUIRE(target.has_error_label("two"));

    SECTION("copy") {
        auto copy = source;

        CHECK(source.code().value() == 1);
        CHECK(source.has_error_label("one"));
        CHECK_FALSE(source.has_error_label("two"));

        CHECK(copy.code().value() == 1);
        CHECK(copy.has_error_label("one"));
        CHECK_FALSE(copy.has_error_label("two"));

        target = copy;

        CHECK(copy.code().value() == 1);
        CHECK(copy.has_error_label("one"));
        CHECK_FALSE(copy.has_error_label("two"));

        CHECK(target.code().value() == 1);
        CHECK(target.has_error_label("one"));
        CHECK_FALSE(target.has_error_label("two"));
    }
}

TEST_CASE("make", "[mongocxx][v1][error][server_error]") {
    SECTION("simple") {
        auto const ex = server_error::internal::make(123, "abc", bsoncxx::v1::document::value{});

        CHECK(ex.code() == v1::source_errc::server);
        CHECK(ex.code() == v1::type_errc::runtime_error);
        CHECK(ex.code().value() == 123);
        CHECK(ex.code().message() == "server error code 123");
        CHECK(ex.client_code() == std::error_code{});

        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("abc"));

        CHECK(ex.raw().empty());
    }

    SECTION("raw") {
        scoped_bson const doc{R"({"ok": 0, "code": 123, "errmsg": "abc"})"};
        auto const ex = server_error::internal::make(123, "abc", doc.value());

        CHECK(ex.code() == v1::source_errc::server);
        CHECK(ex.code() == v1::type_errc::runtime_error);
        CHECK(ex.code().value() == 123);
        CHECK(ex.code().message() == "server error code 123");
        CHECK(ex.client_code() == std::error_code{});

        auto const raw = ex.raw();

        REQUIRE(raw == doc.view());

        REQUIRE(static_cast<bool>(raw["ok"]));
        CHECK(raw["ok"].get_int32().value == 0);

        REQUIRE(static_cast<bool>(raw["code"]));
        CHECK(raw["code"].get_int32().value == 123);

        REQUIRE(static_cast<bool>(raw["errmsg"]));
        CHECK(raw["errmsg"].get_string().value == "abc");
    }

    SECTION("client_code") {
        auto const ec = std::make_error_code(std::errc::invalid_argument);
        scoped_bson const doc{R"({"ok": 0, "code": 123, "errmsg": "abc"})"};
        auto const ex = server_error::internal::make(123, "abc", doc.value(), ec);

        CHECK(ex.code() == v1::source_errc::server);
        CHECK(ex.code() == v1::type_errc::runtime_error);
        CHECK(ex.code().value() == 123);
        CHECK(ex.code().message() == "server error code 123");
        CHECK(ex.client_code() == ec);

        auto const raw = ex.raw();

        REQUIRE(raw == doc.view());

        REQUIRE(static_cast<bool>(raw["ok"]));
        CHECK(raw["ok"].get_int32().value == 0);

        REQUIRE(static_cast<bool>(raw["code"]));
        CHECK(raw["code"].get_int32().value == 123);

        REQUIRE(static_cast<bool>(raw["errmsg"]));
        CHECK(raw["errmsg"].get_string().value == "abc");
    }

    SECTION("errorLabels") {
        SECTION("none") {
            scoped_bson doc{R"({})"};
            auto const ex = server_error::internal::make(0, "", std::move(doc).value());
            CHECK_FALSE(ex.has_error_label("a"));
            CHECK_FALSE(ex.has_error_label("b"));
            CHECK_FALSE(ex.has_error_label("c"));
        }

        SECTION("empty") {
            scoped_bson doc{R"({"errorLabels": []})"};
            auto const ex = server_error::internal::make(0, "", std::move(doc).value());
            CHECK_FALSE(ex.has_error_label("a"));
            CHECK_FALSE(ex.has_error_label("b"));
            CHECK_FALSE(ex.has_error_label("c"));
        }

        SECTION("one") {
            scoped_bson doc{R"({"errorLabels": ["a"]})"};
            auto const ex = server_error::internal::make(0, "", std::move(doc).value());
            CHECK(ex.has_error_label("a"));
            CHECK_FALSE(ex.has_error_label("b"));
            CHECK_FALSE(ex.has_error_label("c"));
        }

        SECTION("many") {
            scoped_bson doc{R"({"errorLabels": ["a", "b", "c"]})"};
            auto const ex = server_error::internal::make(0, "", std::move(doc).value());
            CHECK(ex.has_error_label("a"));
            CHECK(ex.has_error_label("b"));
            CHECK(ex.has_error_label("c"));
        }
    }
}

} // namespace v1
} // namespace mongocxx
