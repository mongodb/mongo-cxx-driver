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

#include <type_traits>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp>

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

} // namespace v1
} // namespace mongocxx
