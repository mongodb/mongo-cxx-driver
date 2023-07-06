// Copyright 2015 MongoDB Inc.
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

#include <vector>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/private/libmongoc.hh>

namespace {
using namespace mongocxx;

class test_log_handler : public logger {
   public:
    using event = std::tuple<log_level, std::string, std::string>;

    test_log_handler(std::vector<event>* events) : _events(events) {}

    void operator()(log_level level,
                    stdx::string_view domain,
                    stdx::string_view message) noexcept final {
        fprintf(stderr, "*\n*\n*\n*\nCALLED OPERATOR()\n*\n*\n");
        fprintf(stderr, "\n***ERROR LEVEL: %d***\n", static_cast<int>(level));
        if (level == log_level::k_error) {
            _events->emplace_back(level, std::string(domain), std::string(message));
            fprintf(stderr, "*\n*\n*\n*\nEVENTS SIZE: %zu\n*\n*\n", _events->size());
        }
        if (level == log_level::k_info) {
            fprintf(stderr, "THIS IS AN INFO LEVEL LOG WHEN IT SHOULD BE AN ERROR LEVEL LOG\n");
        }
    }

   private:
    std::vector<event>* _events;
};

class reset_log_handler_when_done {
   public:
    ~reset_log_handler_when_done() {
        fprintf(stderr, "\n\nRESETTING LOG HANDLER\n\n");
        libmongoc::log_set_handler(::mongoc_log_default_handler, nullptr);
    }
};

TEST_CASE("a user-provided log handler will be used for logging output", "[instance]") {
    reset_log_handler_when_done rlhwd;

    std::vector<test_log_handler::event> events;
    mongocxx::instance driver{stdx::make_unique<test_log_handler>(&events)};

    REQUIRE(&mongocxx::instance::current() == &driver);

    // The libmongoc namespace mocking system doesn't play well with varargs
    // functions, so we use a bare mongoc_log call here.
    fprintf(stderr, "\n\nNOW LOGGING ERROR: %d\n\n", ::MONGOC_LOG_LEVEL_ERROR);
    mongoc_log(::MONGOC_LOG_LEVEL_ERROR, "foo", "bar");

    REQUIRE(&mongocxx::instance::current() == &driver);
    REQUIRE(events.size() == 1);
    REQUIRE(events[0] == std::make_tuple(log_level::k_error, "foo", "bar"));
}

}  // namespace
