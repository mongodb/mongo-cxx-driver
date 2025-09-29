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

#include <mongocxx/test/subprocess.hh>

//

#include <atomic>
#include <iostream>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#if !defined(_MSC_VER)

#include <cstdlib>  // exit(), strsignal(), etc.
#include <string.h> // strsignal()
#include <unistd.h> // fork(), close(), etc.

#include <catch2/catch_test_macros.hpp>
#include <sys/wait.h> // waitpid()

namespace mongocxx {
namespace test {

int subprocess(std::function<void()> fn, bool* is_signal_ptr) {
    auto is_signal_local = false;
    auto& is_signal = is_signal_ptr ? *is_signal_ptr : is_signal_local;

    pid_t const pid = ::fork();

    // Child: do nothing more than call `fn`.
    if (pid == 0) {
        // Use `std::_Exit()` and `std::terminate()` to prevent continued execution of the Catch2 test suite.
        try {
            fn();
            std::_Exit(EXIT_SUCCESS);
        } catch (Catch::TestFailureException) {
            // Assertion failure already output its diagnostic message.
            std::_Exit(EXIT_FAILURE);
        } catch (Catch::TestSkipException) {
            // SKIP() already output its diagnostic message.
            // Don't try to propagate the "skip", just treat as equivalent to success.
            std::_Exit(EXIT_SUCCESS);
        } catch (std::exception const& ex) {
            // Trigger output of Catch2 diagnostic messages.
            FAIL_CHECK("uncaught exception in subprocess: " << ex.what());
            std::_Exit(EXIT_FAILURE);
        } catch (...) {
            // Allow default termination handler to translate the unknown exception type.
            // This should also trigger the output of Catch2 diagnostic messages.
            std::terminate();
        }
    }

    // Parent: wait for child and handle returned status values.
    else {
        int status;

        int const ret = ::waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) {
            UNSCOPED_INFO("subprocess exited with a non-zero exit code: " << WEXITSTATUS(status));
            is_signal = false;
            return WEXITSTATUS(status);
        }

        // For unexpected signals, stop immediately.
        else if (WIFSIGNALED(status)) {
            int const signal = WTERMSIG(status);
            char const* const sigstr = ::strsignal(signal);
            UNSCOPED_INFO("subprocess was killed by signal: " << signal << " (" << (sigstr ? sigstr : "") << ")");
            is_signal = true;
            return signal;
        }

        // We don't expect any other failure condition.
        else {
            REQUIRE(ret != -1);
            return 0;
        }
    }
}

} // namespace test
} // namespace mongocxx

#else

namespace mongocxx {
namespace test {

int subprocess(std::function<void()> fn, bool* is_signal_ptr) {
    (void)fn;
    (void)is_signal_ptr;

    SKIP("mongocxx::test::subprocess() is not supported");

    return 0; // Unused.
}

} // namespace test
} // namespace mongocxx

#endif // !defined(_MSC_VER)

TEST_CASE("counter", "[mongocxx][test][subprocess]") {
    std::atomic_int counter{0};

    CHECK_SUBPROCESS([&counter] { counter.fetch_add(1); });
    CHECK_SUBPROCESS([&counter] { counter.fetch_add(2); });
    CHECK_SUBPROCESS([&counter] { counter.fetch_add(3); });

    // State of a subprocess must not be observable by the original process.
    REQUIRE(counter.load() == 0);
}

TEST_CASE("failure", "[mongocxx][test][subprocess]") {
    auto is_signal = false;
    CHECK_FALSE_SUBPROCESS([] {
        // Try to silence noisy Catch2 output.
        (void)::close(1); // stdout
        (void)::close(2); // stderr

        FAIL("subprocess");
    });
    CHECK_FALSE(is_signal);
}

TEST_CASE("skip", "[mongocxx][test][subprocess]") {
    auto is_signal = false;
    CHECK_SUBPROCESS([] {
        // Try to silence noisy Catch2 output.
        (void)::close(1); // stdout
        (void)::close(2); // stderr

        SKIP("subprocess");
    });
    CHECK_FALSE(is_signal);
}

TEST_CASE("exception", "[mongocxx][test][subprocess]") {
    auto is_signal = false;
    CHECK_FALSE_SUBPROCESS(
        [] {
            // Try to silence noisy Catch2 output.
            (void)::close(1); // stdout
            (void)::close(2); // stderr

            throw std::runtime_error("subprocess");
        },
        &is_signal);
    CHECK_FALSE(is_signal);
}

TEST_CASE("unknown exception", "[mongocxx][test][subprocess]") {
#if !defined(_MSC_VER)
    auto is_signal = false;
    auto const ret = mongocxx::test::subprocess(
        [] {
            // Try to silence noisy Catch2 output.
            (void)::close(1); // stdout
            (void)::close(2); // stderr

            throw "subprocess";
        },
        &is_signal);
    CHECK(is_signal);
    CHECK(ret != 0);
    CHECK(ret != SIGTERM); // std::terminate()

#else

    CHECK_SUBPROCESS([] {});

#endif // !defined(_MSC_VER)
}
