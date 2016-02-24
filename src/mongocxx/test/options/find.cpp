// Copyright 2016 MongoDB Inc.
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

#include <chrono>

#include "catch.hpp"
#include "helpers.hpp"

#include <bsoncxx/document/view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>

using namespace mongocxx;

TEST_CASE("find", "[find][option]") {
    instance::current();

    options::find find_opts{};

    CHECK_OPTIONAL_ARGUMENT(find_opts, allow_partial_results, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, batch_size, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, comment, "comment");
    CHECK_OPTIONAL_ARGUMENT(find_opts, cursor_type, cursor::type::k_non_tailable);
    CHECK_OPTIONAL_ARGUMENT(find_opts, limit, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_await_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, no_cursor_timeout, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, skip, 3);

    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(find_opts, read_preference, read_preference{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, modifiers, bsoncxx::document::view{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, projection, bsoncxx::document::view{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, sort, bsoncxx::document::view{});
}
