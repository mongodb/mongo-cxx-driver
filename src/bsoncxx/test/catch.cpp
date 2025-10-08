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

#include <bsoncxx/test/catch.hh>

//

#include <bsoncxx/v1/config/export.hpp>

#include <cstdlib>
#include <iostream>
#include <system_error>

#include <bsoncxx/private/bson.hh> // <winsock.h> via <bson/bson-compat.h>

#include <catch2/catch_session.hpp>

// Ensure the Winsock DLL is initialized prior to calling `gethostname` in `bsoncxx::oid::oid()`:
//  - bson_oid_init -> bson_context_get_default -> ... -> _bson_context_init_random -> gethostname.
struct WSAGuard {
    ~WSAGuard() {
#if defined(_WIN32)
        (void)WSACleanup();
#endif
    }

    WSAGuard(WSAGuard&&) = delete;
    WSAGuard& operator=(WSAGuard&) = delete;
    WSAGuard(WSAGuard const&) = delete;
    WSAGuard& operator=(WSAGuard const&) = delete;

    WSAGuard() {
#if defined(_WIN32)
        WSADATA wsaData;
        if (WSAStartup((MAKEWORD(2, 2)), &wsaData) != 0) {
            std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
            std::abort();
        }
#endif
    }
};

int BSONCXX_ABI_CDECL main(int argc, char* argv[]) {
    WSAGuard wsa_guard;
    return Catch::Session().run(argc, argv);
}

TEST_CASE("THROWS_WITH_CODE", "[bsoncxx][test]") {
    SECTION("basic") {
        CHECK_THROWS_WITH_CODE(
            throw std::system_error(std::make_error_code(std::errc::invalid_argument)), std::errc::invalid_argument);
    }

    // TEST_CHECK is evaluated when a `std::system_error` exception is thrown as expected and is
    // used to evaluate the error code comparison check.
    int checked = 0;

    // TEST_CHECK_THROWS_AS is evaluated when an unexpected exception type is thrown and is used to
    // trigger Catch test failure (always fails).
    int checked_throws_as = 0;

    SECTION("Catch::TestFailureException") {
#define TEST_CHECK(expr)                                   \
    if (1) {                                               \
        ++checked;                                         \
        FAIL("Catch::TestFailureException did not throw"); \
    } else                                                 \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)                    \
    if (1) {                                                \
        ++checked_throws_as;                                \
        CHECK_THROWS_AS(expr, Catch::TestFailureException); \
    } else                                                  \
        ((void)0)

        try {
            THROWS_WITH_CODE_IMPL(TEST_CHECK, throw Catch::TestFailureException(), std::errc::invalid_argument);
        } catch (Catch::TestFailureException const&) {
            SUCCEED("Catch::TestFailureException was propagated");
        } catch (...) {
            FAIL("unexpected exception was thrown");
        }

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 0);
        CHECK(checked_throws_as == 0);
    }

    SECTION("Catch::TestSkipException") {
#define TEST_CHECK(expr)                                \
    if (1) {                                            \
        ++checked;                                      \
        FAIL("Catch::TestSkipException did not throw"); \
    } else                                              \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)                 \
    if (1) {                                             \
        ++checked_throws_as;                             \
        CHECK_THROWS_AS(expr, Catch::TestSkipException); \
    } else                                               \
        ((void)0)

        try {
            THROWS_WITH_CODE_IMPL(TEST_CHECK, throw Catch::TestSkipException(), std::errc::invalid_argument);
        } catch (Catch::TestSkipException const&) {
            SUCCEED("Catch::TestSkipException was propagated");
        } catch (...) {
            FAIL("unexpected exception was thrown");
        }

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 0);
        CHECK(checked_throws_as == 0);
    }

    SECTION("unrelated") {
        struct unrelated {};

#define TEST_CHECK(expr)                           \
    if (1) {                                       \
        ++checked;                                 \
        FAIL("unrelated exception did not throw"); \
    } else                                         \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)  \
    if (1) {                              \
        ++checked_throws_as;              \
        CHECK_THROWS_AS(expr, unrelated); \
    } else                                \
        ((void)0)

        THROWS_WITH_CODE_IMPL(TEST_CHECK, throw unrelated(), std::errc::invalid_argument);

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 0);
        CHECK(checked_throws_as == 1);
    }

    SECTION("std::system_error") {
#define TEST_CHECK(expr) \
    if (1) {             \
        ++checked;       \
        CHECK(expr);     \
    } else               \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)          \
    if (1) {                                      \
        ++checked_throws_as;                      \
        CHECK_THROWS_AS(expr, std::system_error); \
    } else                                        \
        ((void)0)

        THROWS_WITH_CODE_IMPL(
            TEST_CHECK,
            throw std::system_error(std::make_error_code(std::errc::invalid_argument)),
            std::make_error_code(std::errc::invalid_argument));

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 1);
        CHECK(checked_throws_as == 0);
    }

    SECTION("derived") {
        struct derived : std::system_error {
            using std::system_error::system_error;
        };

#define TEST_CHECK(expr) \
    if (1) {             \
        ++checked;       \
        CHECK(expr);     \
    } else               \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)          \
    if (1) {                                      \
        ++checked_throws_as;                      \
        CHECK_THROWS_AS(expr, std::system_error); \
    } else                                        \
        ((void)0)

        THROWS_WITH_CODE_IMPL(
            TEST_CHECK, throw derived(std::make_error_code(std::errc::invalid_argument)), std::errc::invalid_argument);

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 1);
        CHECK(checked_throws_as == 0);
    }

    SECTION("error code") {
#define TEST_CHECK(expr)                                           \
    if (1) {                                                       \
        ++checked;                                                 \
        CHECK_FALSE(expr); /* invalid_argument != not_supported */ \
    } else                                                         \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)          \
    if (1) {                                      \
        ++checked_throws_as;                      \
        CHECK_THROWS_AS(expr, std::system_error); \
    } else                                        \
        ((void)0)

        THROWS_WITH_CODE_IMPL(
            TEST_CHECK,
            throw std::system_error(std::make_error_code(std::errc::invalid_argument)),
            std::errc::not_supported);

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 1);
        CHECK(checked_throws_as == 0);
    }

    SECTION("error condition") {
#define TEST_CHECK(expr) \
    if (1) {             \
        ++checked;       \
        CHECK(expr);     \
    } else               \
        ((void)0)

#define TEST_CHECK_THROWS_AS(expr, type)          \
    if (1) {                                      \
        ++checked_throws_as;                      \
        CHECK_THROWS_AS(expr, std::system_error); \
    } else                                        \
        ((void)0)

        THROWS_WITH_CODE_IMPL(
            TEST_CHECK,
            throw std::system_error(std::make_error_code(std::errc::invalid_argument)),
            std::make_error_condition(std::errc::invalid_argument));

#undef TEST_CHECK
#undef TEST_CHECK_THROWS_AS

        CHECK(checked == 1);
        CHECK(checked_throws_as == 0);
    }
}
