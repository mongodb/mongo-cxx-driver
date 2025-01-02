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

#include <utility>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
class example_logger : public mongocxx::logger {
   private:
    int* counter_ptr;

   public:
    explicit example_logger(int* ptr) : counter_ptr(ptr) {}

    void operator()(
        mongocxx::log_level level,
        bsoncxx::stdx::string_view domain,
        bsoncxx::stdx::string_view message) noexcept override {
        EXPECT(level == mongocxx::log_level::k_info);
        EXPECT(domain == "mongocxx");
        EXPECT(message == "libmongoc logging callback enabled");

        *counter_ptr += 1;
    }
};

void example() {
    int counter = 0;

    // Use `std::make_unique` with C++14 or newer.
    auto logger = std::unique_ptr<mongocxx::logger>(new example_logger{&counter});

    // Emit the informational mongocxx log message: "libmongoc logging callback enabled".
    mongocxx::instance instance{std::move(logger)};

    EXPECT(counter == 1);
}
// [Example]

} // namespace

RUNNER_REGISTER_FORKING_COMPONENT() {
    example();
}
