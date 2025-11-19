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

#include <mongocxx/test/v1/exception.hh>

//

#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/exception.hh>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/exception.hh>

#include <cstdint>
#include <system_error>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("source", "[mongocxx][v1][error]") {
    auto const& c = source_error_category();

    SECTION("name") {
        CHECK_THAT(c.name(), Catch::Matchers::Equals("mongocxx::v1::source_errc"));
    }

    SECTION("message") {
        CHECK(c.message(-1) == "mongocxx::v1::source_errc:-1");
        CHECK(c.message(0) == "zero");
        CHECK(c.message(1) == "mongocxx");
        CHECK(c.message(2) == "mongoc");
        CHECK(c.message(3) == "mongocrypt");
        CHECK(c.message(4) == "server");
        CHECK(c.message(5) == "mongocxx::v1::source_errc:5");
    }
}

TEST_CASE("type", "[mongocxx][v1][error]") {
    auto const& c = type_error_category();

    SECTION("name") {
        CHECK_THAT(c.name(), Catch::Matchers::Equals("mongocxx::v1::type_errc"));
    }

    SECTION("message") {
        CHECK(c.message(-1) == "mongocxx::v1::type_errc:-1");
        CHECK(c.message(0) == "zero");
        CHECK(c.message(1) == "invalid argument");
        CHECK(c.message(2) == "runtime error");
        CHECK(c.message(3) == "mongocxx::v1::type_errc:3");
    }
}

TEST_CASE("ownership", "[mongocxx][v1][error][exception]") {
    auto source = exception::internal::make(1, std::system_category(), "source");
    auto target = exception::internal::make(2, std::system_category(), "target");

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

TEST_CASE("make", "[mongocxx][v1][error][exception]") {
    SECTION("error_code") {
        using std::make_error_code;

        SECTION("default") {
            auto const ec = exception::internal::make(std::error_code{}).code();

            CHECK(ec == std::error_code{});

            // Unsupported category.
            CHECK(ec != mongocxx::v1::type_errc::zero);
            CHECK(ec != mongocxx::v1::type_errc::invalid_argument);
            CHECK(ec != mongocxx::v1::type_errc::runtime_error);
        }

        SECTION("generic") {
            auto const ec = exception::internal::make(make_error_code(std::errc::invalid_argument)).code();

            CHECK(ec == std::errc::invalid_argument);

            // Unsupported category.
            CHECK(ec != mongocxx::v1::type_errc::zero);
            CHECK(ec != mongocxx::v1::type_errc::invalid_argument);
            CHECK(ec != mongocxx::v1::type_errc::runtime_error);
        }

        SECTION("bsoncxx") {
            auto const ec =
                exception::internal::make(make_error_code(bsoncxx::v1::document::view::errc::invalid_data)).code();

            // bsoncxx error category.
            CHECK(ec == bsoncxx::v1::document::view::errc::invalid_data);
            CHECK(ec == bsoncxx::v1::source_errc::bsoncxx);
            CHECK(ec == bsoncxx::v1::type_errc::runtime_error);

            // Not from mongocxx.
            CHECK(ec != mongocxx::v1::source_errc::mongocxx);
            CHECK(ec != mongocxx::v1::type_errc::runtime_error);
        }
    }

    SECTION("int + category") {
        auto const v = static_cast<int>(std::errc::invalid_argument);
        auto const ex = exception::internal::make(v, std::generic_category());

        CHECK(ex.code() == std::errc::invalid_argument);
        CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring(std::generic_category().message(v)));
    }

    SECTION("int + category + message") {
        auto const v = static_cast<int>(std::errc::invalid_argument);
        auto const ex = exception::internal::make(v, std::generic_category(), "abc");

        CHECK(ex.code() == std::errc::invalid_argument);
        CHECK_THAT(
            ex.what(),
            Catch::Matchers::ContainsSubstring(std::generic_category().message(v)) &&
                Catch::Matchers::ContainsSubstring("abc"));
    }
}

TEST_CASE("set_error_labels", "[mongocxx][v1][error][exception]") {
    auto ex = exception::internal::make(std::error_code{});

    exception::internal::set_error_labels(ex, scoped_bson{R"({"errorLabels": ["two"]})"}.view());

    CHECK_FALSE(ex.has_error_label("one"));
    CHECK(ex.has_error_label("two"));
    CHECK_FALSE(ex.has_error_label("three"));

    SECTION("replace") {
        scoped_bson const doc{R"({"errorLabels": ["one", "two", "three"]})"};
        exception::internal::set_error_labels(ex, doc.view());
        CHECK(ex.has_error_label("one"));
        CHECK(ex.has_error_label("two"));
        CHECK(ex.has_error_label("three"));
    }

    SECTION("reset") {
        exception::internal::set_error_labels(ex, bsoncxx::v1::document::view{});

        CHECK_FALSE(ex.has_error_label("one"));
        CHECK_FALSE(ex.has_error_label("two"));
        CHECK_FALSE(ex.has_error_label("three"));
    }

    SECTION("ignore") {
        scoped_bson const doc{R"({"errorLabels": ["one", 2, "three"]})"};
        exception::internal::set_error_labels(ex, doc.view());

        CHECK(ex.has_error_label("one"));
        CHECK_FALSE(ex.has_error_label("two"));
        CHECK(ex.has_error_label("three"));
    }
}

TEST_CASE("throw_exception", "[mongocxx][v1][error][exception]") {
    // mongoc-error-private.h
    enum category : std::uint8_t {
        MONGOC_ERROR_CATEGORY_BSON = 1, // BSON_ERROR_CATEGORY
        MONGOC_ERROR_CATEGORY = 2,
        MONGOC_ERROR_CATEGORY_SERVER = 3,
        MONGOC_ERROR_CATEGORY_CRYPT = 4,
        MONGOC_ERROR_CATEGORY_SASL = 5,
    };

    SECTION("bson") {
        SECTION("with message") {
            try {
                bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY_BSON};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                auto const& code = ex.code();

                // Translate bson error codes from mongoc into MONGOC_ERROR_BSON_INVALID.
                CHECK(code == mongocxx::v1::source_errc::mongoc);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
                CHECK(code.value() == MONGOC_ERROR_BSON_INVALID);
                CHECK(MONGOC_ERROR_BSON_INVALID == 18);
                CHECK(code.message() == "mongoc_error_code_t:18");

                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("bson error code 123: abc"));
            }
        }

        SECTION("no message") {
            try {
                bson_error_t const error{0, 123, {}, MONGOC_ERROR_CATEGORY_BSON};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                CHECK_THAT(
                    ex.what(),
                    !Catch::Matchers::ContainsSubstring(ex.code().message() + ": ") ||
                        !Catch::Matchers::ContainsSubstring("bson error code 123: "));
            }
        }
    }

    SECTION("mongoc") {
        SECTION("with message") {
            try {
                bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                auto const& code = ex.code();

                CHECK(code == mongocxx::v1::source_errc::mongoc);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
                CHECK(code.value() == 123);
                CHECK(code.message() == "mongoc_error_code_t:123");

                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("abc"));
            }
        }

        SECTION("no message") {
            try {
                bson_error_t const error{0, 123, {}, MONGOC_ERROR_CATEGORY};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                CHECK_THAT(ex.what(), !Catch::Matchers::ContainsSubstring(ex.code().message() + ": "));
            }
        }
    }

    SECTION("server") {
        SECTION("plain") {
            SECTION("with message") {
                try {
                    bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY_SERVER};
                    throw_exception(error);
                } catch (v1::exception const& ex) {
                    auto const& code = ex.code();

                    CHECK(code == mongocxx::v1::source_errc::server);
                    CHECK(code == mongocxx::v1::type_errc::runtime_error);
                    CHECK(code.message() == "server error code 123");

                    CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("abc"));
                }
            }

            SECTION("no message") {
                try {
                    bson_error_t const error{0, 123, {}, MONGOC_ERROR_CATEGORY_SERVER};
                    throw_exception(error);
                } catch (v1::exception const& ex) {
                    CHECK_THAT(ex.what(), !Catch::Matchers::ContainsSubstring(ex.code().message() + ": "));
                }
            }
        }

        SECTION("invalid raw") {
            auto const op = [](bsoncxx::v1::document::value raw) -> std::error_code {
                try {
                    bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY_SERVER};
                    throw_exception(error, std::move(raw));
                } catch (v1::server_error const& ex) {
                    CAPTURE(ex);
                    FAIL("should not reach this point");
                    return {};
                } catch (v1::exception const& ex) {
                    return ex.code();
                }
            };

            {
                auto const code = op(bsoncxx::v1::document::value{nullptr});

                CHECK(code.message() == "server error code 123");
                CHECK(code == mongocxx::v1::source_errc::server);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
            }

            {
                auto const code = op(scoped_bson{}.value());

                CHECK(code.message() == "server error code 123");
                CHECK(code == mongocxx::v1::source_errc::server);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
            }

            {
                auto const code = op(scoped_bson{R"({"x": 1})"}.value());

                CHECK(code.message() == "server error code 123");
                CHECK(code == mongocxx::v1::source_errc::server);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
            }
        }

        SECTION("valid raw") {
            SECTION("with message") {
                try {
                    bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY};
                    throw_exception(error, scoped_bson{R"({"code": 456})"}.value());
                } catch (v1::server_error const& ex) {
                    auto const& code = ex.code();

                    CHECK(code == mongocxx::v1::source_errc::server);
                    CHECK(code == mongocxx::v1::type_errc::runtime_error);
                    CHECK(code.value() == 456);
                    CHECK(code.message() == "server error code 456");
                    CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("abc"));

                    CHECK(ex.client_code().value() == 123);
                    CHECK(ex.client_code() == mongocxx::v1::source_errc::mongoc);
                }
            }

            SECTION("no message") {
                try {
                    bson_error_t const error{0, 123, {}, MONGOC_ERROR_CATEGORY};
                    throw_exception(error, scoped_bson{R"({"code": 456})"}.value());
                } catch (v1::server_error const& ex) {
                    CHECK_THAT(ex.what(), !Catch::Matchers::ContainsSubstring(ex.code().message() + ": "));
                }
            }
        }
    }

    SECTION("mongocrypt") {
        SECTION("with message") {
            try {
                bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY_CRYPT};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                auto const& code = ex.code();

                CHECK(code == mongocxx::v1::source_errc::mongocrypt);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
                CHECK(code.value() == 123);
                CHECK(code.message() == "mongocrypt_status_t:123");

                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("abc"));
            }
        }

        SECTION("no message") {
            try {
                bson_error_t const error{0, 123, {}, MONGOC_ERROR_CATEGORY_CRYPT};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                CHECK_THAT(ex.what(), !Catch::Matchers::ContainsSubstring(ex.code().message() + ": "));
            }
        }
    }

    SECTION("sasl") {
        SECTION("with message") {
            try {
                bson_error_t const error{0, 123, "abc", MONGOC_ERROR_CATEGORY_SASL};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                auto const& code = ex.code();

                // Translate sasl error codes from mongoc into MONGOC_ERROR_CLIENT_AUTHENTICATE.
                CHECK(code == mongocxx::v1::source_errc::mongoc);
                CHECK(code == mongocxx::v1::type_errc::runtime_error);
                CHECK(code.value() == MONGOC_ERROR_CLIENT_AUTHENTICATE);
                CHECK(MONGOC_ERROR_CLIENT_AUTHENTICATE == 11);
                CHECK(code.message() == "mongoc_error_code_t:11");

                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("sasl error code 123: abc"));
            }
        }

        SECTION("no message") {
            try {
                bson_error_t const error{0, 123, {}, MONGOC_ERROR_CATEGORY_SASL};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                CHECK_THAT(
                    ex.what(),
                    !Catch::Matchers::ContainsSubstring(ex.code().message() + ": ") ||
                        !Catch::Matchers::ContainsSubstring("sasl error code 123: "));
            }
        }
    }

    SECTION("unknown") {
        SECTION("with message") {
            try {
                bson_error_t const error{0, 123, "abc", UINT8_MAX};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                auto const& code = ex.code();

                CHECK(code != mongocxx::v1::source_errc::zero);
                CHECK(code != mongocxx::v1::source_errc::mongocxx);
                CHECK(code != mongocxx::v1::source_errc::mongoc);
                CHECK(code != mongocxx::v1::source_errc::mongocrypt);
                CHECK(code != mongocxx::v1::source_errc::server);

                CHECK(code == mongocxx::v1::type_errc::runtime_error);

                CHECK(code.value() == 123);
                CHECK(code.message() == "unknown:123");

                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("unknown error category 255: abc"));
            }
        }

        SECTION("no message") {
            try {
                bson_error_t const error{0, 123, {}, UINT8_MAX};
                throw_exception(error);
            } catch (v1::exception const& ex) {
                CHECK_THAT(
                    ex.what(),
                    !Catch::Matchers::ContainsSubstring(ex.code().message() + ": ") ||
                        !Catch::Matchers::ContainsSubstring("unknown error category 255: "));
            }
        }
    }
}

} // namespace v1
} // namespace mongocxx
