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

#include <mongocxx/test/v1/logger.hh>

//

#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <mongocxx/test/v1/capture_stderr.hh>

#include <string>
#include <type_traits>

#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct custom_logger : logger {
    void operator()(
        log_level level,
        bsoncxx::v1::stdx::string_view domain,
        bsoncxx::v1::stdx::string_view message) noexcept override {
        (void)level;
        (void)domain;
        (void)message;
    }
};

} // namespace

// mongocxx::v1::logger must not unnecessarily impose special requirements on derived classes.
static_assert(std::is_nothrow_destructible<custom_logger>::value, "");
static_assert(std::is_nothrow_move_constructible<custom_logger>::value, "");
static_assert(std::is_copy_constructible<custom_logger>::value, "");
static_assert(std::is_default_constructible<custom_logger>::value, "");

TEST_CASE("stringify", "[mongocxx][test][v1][logger]") {
    // No point specializing StringMaker for abstract class mongocxx::v1::logger.
    CHECK(bsoncxx::test::stringify(custom_logger{}) == "{?}");
}

// These tests run within the lifetime of the test suite's global instance (see catch.cpp), which is
// the required context for the global-logger API. The `restore` guard below returns the global
// handler to its pre-test configuration so test cases remain isolated from one another.
TEST_CASE("set_global_logger and logger_guard", "[mongocxx][test][v1][logger]") {
    using Catch::Matchers::ContainsSubstring;
    using mongocxx::test::capture_stderr;

    // Recorded state of the most recent custom-handler invocation.
    int count = 0;
    log_level level = log_level::k_error;
    std::string domain;
    std::string message;

    auto const record = [&](log_level l, bsoncxx::v1::stdx::string_view d, bsoncxx::v1::stdx::string_view m) {
        ++count;
        level = l;
        domain = std::string{d};
        message = std::string{m};
    };

    // Declared after `record`/`count`/... so that restoring the prior handler (which frees any
    // custom handler still installed) happens before those captured variables are destroyed.
    logger_guard const restore{default_logger{}};

    SECTION("set_global_logger switches handlers at runtime") {
        set_global_logger(record);

        mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "custom");
        CHECK(count == 1);
        CHECK(level == log_level::k_warning);
        CHECK(domain == "dom");
        CHECK(message == "custom");

        // Switch to mongoc's default handler: output goes to stderr; the custom handler is inert.
        set_global_logger(default_logger{});
        std::string out;
        {
            capture_stderr cap;
            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "to-stderr");
            out = cap.read();
        }
        CHECK_THAT(out, ContainsSubstring("to-stderr"));
        CHECK(count == 1);

        // Disable logging: nothing is emitted anywhere.
        set_global_logger(nullptr);
        {
            capture_stderr cap;
            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "suppressed");
            out = cap.read();
        }
        CHECK(out.find("suppressed") == std::string::npos);
        CHECK(count == 1);
    }

    SECTION("logger_guard installs a custom handler for its scope over the default handler") {
        {
            logger_guard const scope{record};

            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "inside");
            CHECK(count == 1);
            CHECK(message == "inside");
        }

        // The default handler is restored: output goes to stderr; the custom handler is inert.
        std::string out;
        {
            capture_stderr cap;
            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "outside");
            out = cap.read();
        }
        CHECK_THAT(out, ContainsSubstring("outside"));
        CHECK(count == 1);
    }

    SECTION("logger_guard restores a previously-installed custom handler") {
        set_global_logger(record);

        {
            logger_guard const scope{default_logger{}};

            std::string out;
            {
                capture_stderr cap;
                mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "temp-default");
                out = cap.read();
            }
            CHECK_THAT(out, ContainsSubstring("temp-default"));
            CHECK(count == 0); // The custom handler was displaced by the guard.
        }

        // The custom handler is restored.
        mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "restored");
        CHECK(count == 1);
        CHECK(message == "restored");
    }

    SECTION("logger_guard restores the disabled state") {
        set_global_logger(nullptr); // disabled

        {
            logger_guard const scope{record};

            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "inside");
            CHECK(count == 1);
        }

        // The disabled state is restored: nothing is emitted anywhere.
        std::string out;
        {
            capture_stderr cap;
            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "suppressed");
            out = cap.read();
        }
        CHECK(out.empty());
        CHECK(count == 1);
    }

    SECTION("nested logger_guards restore in LIFO order") {
        int a = 0;
        int b = 0;
        int c = 0;

        auto const counter = [](int& n) {
            return [&n](log_level, bsoncxx::v1::stdx::string_view, bsoncxx::v1::stdx::string_view) { ++n; };
        };

        logger_guard const outer{counter(a)};
        mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "a");
        CHECK((a == 1 && b == 0 && c == 0));

        {
            logger_guard const middle{counter(b)};
            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "b");
            CHECK((a == 1 && b == 1 && c == 0));

            {
                logger_guard const inner{counter(c)};
                mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "c");
                CHECK((a == 1 && b == 1 && c == 1));
            }

            // `inner` destroyed: `middle`'s handler is active again.
            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "b");
            CHECK((a == 1 && b == 2 && c == 1));
        }

        // `middle` destroyed: `outer`'s handler is active again.
        mongoc_log(MONGOC_LOG_LEVEL_WARNING, "dom", "a");
        CHECK((a == 2 && b == 2 && c == 1));
    }
}

} // namespace v1
} // namespace mongocxx
