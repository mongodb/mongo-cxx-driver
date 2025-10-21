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

#include <mongocxx/test/v1/instance.hh>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/test/v1/logger.hh>

#include <functional>
#include <utility>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/stringify.hh>

#include <mongocxx/test/subprocess.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#if !defined(_MSC_VER)
#include <unistd.h>
#endif

namespace mongocxx {
namespace v1 {

namespace {

class custom_logger : public logger {
   private:
    using fn_type = std::function<void(log_level, bsoncxx::v1::stdx::string_view, bsoncxx::v1::stdx::string_view)>;

    fn_type _fn;

   public:
    /* explicit(false) */ custom_logger(fn_type fn) : _fn{std::move(fn)} {}

    void operator()(
        log_level level,
        bsoncxx::v1::stdx::string_view domain,
        bsoncxx::v1::stdx::string_view message) noexcept override {
        _fn(level, domain, message);
    }
};

} // namespace

TEST_CASE("logger", "[mongocxx][v1][instance]") {
#if !defined(_MSC_VER)
    class capture_stderr {
       private:
        int _pipes[2];
        int _stderr; // stderr

       public:
        ~capture_stderr() {
            ::dup2(_stderr, STDERR_FILENO); // Restore original stderr.
        }

        capture_stderr(capture_stderr&&) = delete;
        capture_stderr& operator=(capture_stderr&&) = delete;
        capture_stderr(capture_stderr const&) = delete;
        capture_stderr& operator=(capture_stderr const&) = delete;

        capture_stderr()
            : _stderr{::dup(STDERR_FILENO)} // Save original stderr.
        {
            ::fflush(stderr);
            REQUIRE(::pipe(_pipes) == 0);                    // Open redirection pipes.
            REQUIRE(::dup2(_pipes[1], STDERR_FILENO) != -1); // Copy stderr to input pipe.
            REQUIRE(::close(_pipes[1]) != -1);               // Close original stderr.

            REQUIRE(
                ::fcntl(_pipes[0], F_SETFL, ::fcntl(_pipes[0], F_GETFL) | O_NONBLOCK) != -1); // Do not block on read.
        }

        std::string read() {
            std::string res;

            {
                // Capture everything up to this point.
                // Avoid recursive macro expansion of `stderr` due to Catch expression decomposition.
                auto const res = ::fflush(stderr);
                REQUIRE(res == 0);
            }

            while (true) {
                char buf[BUFSIZ];
                auto const n = ::read(_pipes[0], buf, std::size_t{BUFSIZ - 1});

                CHECKED_IF(n < 0) {
                    REQUIRE(errno == EAGAIN); // No data left in stream.
                    break;
                }
                else {
                    res.append(buf, static_cast<std::size_t>(n));
                }
            }

            return res;
        }
    };

#else

    class capture_stderr {
       public:
        std::string read() {
            return {};
        }
    };

#endif // !defined(_MSC_VER)

    {
        auto const test_default = [] {
            instance i;

            auto const output = [&] {
                capture_stderr guard;
                mongoc_log(MONGOC_LOG_LEVEL_WARNING, "mongocxx::v1::instance", "mongoc_log_default_handler");
                return guard.read();
            }();

            CHECK_THAT(
                output,
                Catch::Matchers::ContainsSubstring("mongocxx::v1::instance") &&
                    Catch::Matchers::ContainsSubstring("mongoc_log_default_handler"));
        };
        CHECK_SUBPROCESS(test_default);
    }

    {
        auto const test_noop = [] {
            instance i{nullptr};

            auto const output = [&] {
                capture_stderr guard;
                mongoc_log(MONGOC_LOG_LEVEL_WARNING, "mongocxx::v1::instance", "SHOULD NOT BE LOGGED");
                return guard.read();
            }();

            CHECK_THAT(
                output,
                !Catch::Matchers::ContainsSubstring("mongocxx::v1::instance") &&
                    !Catch::Matchers::ContainsSubstring("SHOULD NOT BE LOGGED"));
        };
        CHECK_SUBPROCESS(test_noop);
    }

    {
        auto const test_custom = [] {
            int count = 0;
            log_level level;
            std::string domain;
            std::string message;

            auto const fn =
                [&](log_level _level, bsoncxx::v1::stdx::string_view _domain, bsoncxx::v1::stdx::string_view _message) {
                    ++count;
                    level = _level;
                    domain = std::string{_domain};
                    message = std::string{_message};
                };

            instance i{bsoncxx::make_unique<custom_logger>(fn)};

            mongoc_log(MONGOC_LOG_LEVEL_WARNING, "mongocxx::v1::instance", "custom_logger");

            CHECK(count == 1);
            CHECK(level == log_level::k_warning);
            CHECK(domain == "mongocxx::v1::instance");
            CHECK(message == "custom_logger");
        };
        CHECK_SUBPROCESS(test_custom);
    }
}

TEST_CASE("stringify", "[mongocxx][test][v1][instance]") {
    auto const test = [] {
        instance i;

        REQUIRE(bsoncxx::test::stringify(i) == "mongocxx::v1::instance");
    };
    CHECK_SUBPROCESS(test);
}

} // namespace v1
} // namespace mongocxx
