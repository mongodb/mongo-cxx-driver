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

#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <mongocxx/test/v1/capture_stderr.hh>
#include <mongocxx/test/v1/logger.hh>

#include <functional>
#include <string>
#include <utility>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/stringify.hh>

#include <mongocxx/test/subprocess.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

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
    using mongocxx::test::capture_stderr;

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
        (void)test_default;
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
        (void)test_noop;
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
        (void)test_custom;
        CHECK_SUBPROCESS(test_custom);
    }
}

TEST_CASE("stringify", "[mongocxx][test][v1][instance]") {
    auto const test = [] {
        instance i;

        REQUIRE(bsoncxx::test::stringify(i) == "mongocxx::v1::instance");
    };
    (void)test;
    CHECK_SUBPROCESS(test);
}

} // namespace v1
} // namespace mongocxx
