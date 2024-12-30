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

#include <bsoncxx/test/exception_guard.hh>

//

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <bsoncxx/test/catch.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

TEST_CASE("bsoncxx::test::exception_guard", "[test]") {
    using EGuard = bsoncxx::test::exception_guard_state;

    EGuard eguard;

    SECTION("init") {
        CHECK(eguard.ptr == nullptr);
        CHECK(eguard.file == bsoncxx::stdx::string_view(""));
        CHECK(eguard.line == 0u);
        CHECK(eguard.func == bsoncxx::stdx::string_view(""));
    }

    SECTION("reset") {
        // clang-format off
        BSONCXX_TEST_EXCEPTION_GUARD_RESET(eguard); const auto line = __LINE__;
        // clang-format on

        CHECK(eguard.ptr == nullptr);
        CHECK(eguard.file == bsoncxx::stdx::string_view(__FILE__));
        CHECK(eguard.line == line);
        CHECK(eguard.func == bsoncxx::stdx::string_view(__func__));
    }

    SECTION("simple") {
        SECTION("no throw") {
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);
            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard);

            CHECK(eguard.ptr == nullptr);
            CHECK(eguard.file == bsoncxx::stdx::string_view(""));
            CHECK(eguard.line == 0u);
            CHECK(eguard.func == bsoncxx::stdx::string_view(""));

            BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard);
            SUCCEED("no exception was thrown by the check");
        }

        SECTION("throw") {
            struct EGuardException {};

            EGuard expected;

            // clang-format off
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);
            throw EGuardException();
            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard); BSONCXX_TEST_EXCEPTION_GUARD_RESET(expected);
            // clang-format on

            CHECK(eguard.ptr != nullptr);
            CHECK(eguard.file == expected.file);
            CHECK(eguard.line == expected.line);
            CHECK(eguard.func == expected.func);
            CHECK(eguard.ignored.empty());

            auto const check_expr = [&] { BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard); };
            REQUIRE_THROWS_AS(check_expr(), EGuardException);
            REQUIRE_THROWS_AS(check_expr(), EGuardException); // Rethrow is OK.

            CHECK(eguard.ptr != nullptr);
            CHECK(eguard.file == expected.file);
            CHECK(eguard.line == expected.line);
            CHECK(eguard.func == expected.func);
            CHECK(eguard.ignored.empty());
        }

        SECTION("ignored") {
            struct EGuardException : std::runtime_error {
                using std::runtime_error::runtime_error;
            };

            // clang-format off
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);
            throw EGuardException("one");
            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard);
            // clang-format on

            REQUIRE(eguard.ptr != nullptr);
            REQUIRE(eguard.ignored.size() == 0u);

            EGuard expected;

            // clang-format off
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);
            throw EGuardException("two");
            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard); BSONCXX_TEST_EXCEPTION_GUARD_RESET(expected);
            // clang-format on

            auto const npos = std::string::npos;

            REQUIRE(eguard.ignored.size() == 1u);
            {
                auto const& log = eguard.ignored[0];
                auto const log_view = bsoncxx::stdx::string_view(log);

                CAPTURE(log);
                CAPTURE(expected.file);
                CAPTURE(expected.line);
                CAPTURE(expected.func);

                CHECK_THAT(log, Catch::Matchers::ContainsSubstring("two"));
                CHECK(log_view.find(expected.file) != npos);
                CHECK(log_view.find(std::to_string(expected.line)) != npos);
                CHECK(log_view.find(expected.func) == npos); // Func is not logged.
            }

            // clang-format off
            BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);
            throw Catch::TestFailureException(); // As-if by `REQUIRE(false)`.
            BSONCXX_TEST_EXCEPTION_GUARD_END(eguard); BSONCXX_TEST_EXCEPTION_GUARD_RESET(expected);
            // clang-format on

            REQUIRE(eguard.ignored.size() == 2u);
            {
                auto const& log = eguard.ignored[1];
                auto const log_view = bsoncxx::stdx::string_view(log);

                CAPTURE(log);
                CAPTURE(expected.file);
                CAPTURE(expected.line);
                CAPTURE(expected.func);

                CHECK_THAT(log, Catch::Matchers::ContainsSubstring("Catch::TestFailureException"));
                CHECK(log_view.find(expected.file) != npos);
                CHECK(log_view.find(std::to_string(expected.line)) != npos);
                CHECK(log_view.find(expected.func) == npos); // Func is not logged.
            }

            // The original exception.
            auto const check_expr = [&] { BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard); };
            REQUIRE_THROWS_WITH(check_expr(), Catch::Matchers::ContainsSubstring("one"));
        }
    }

    SECTION("concurrent") {
        EGuard expected;

        struct EGuardException : std::runtime_error {
            int id;
            EGuardException(int i) : std::runtime_error(std::to_string(i)), id(i) {}
        };

        {
            std::atomic_int counter;
            std::atomic_bool latch;

            std::atomic_init(&counter, 0);
            std::atomic_init(&latch, false);

            auto fn = [&] {
                // A simple latch to maximize parallelism.
                while (!latch.load()) {
                }

                BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(eguard);
                throw EGuardException(++counter);
                // clang-format off
                BSONCXX_TEST_EXCEPTION_GUARD_END(eguard); BSONCXX_TEST_EXCEPTION_GUARD_RESET(expected);
                // clang-format on
            };

            std::vector<std::thread> threads;

            threads.emplace_back(fn);
            threads.emplace_back(fn);
            threads.emplace_back(fn);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            latch.store(true);

            for (auto& thread : threads) {
                thread.join();
            }

            REQUIRE(counter.load() == 3);
        }

        REQUIRE(eguard.ptr != nullptr);
        CHECK(eguard.file == expected.file);
        CHECK(eguard.line == expected.line);
        CHECK(eguard.func == expected.func);

        auto test = [&] {
            try {
                BSONCXX_TEST_EXCEPTION_GUARD_CHECK(eguard);
                FAIL("should have thrown an EGuardException");
            } catch (EGuardException const& e) {
                CAPTURE(e.id);
                CHECK(e.id > 0);
            }
        };

        REQUIRE_NOTHROW(test());
    }
}

} // namespace
