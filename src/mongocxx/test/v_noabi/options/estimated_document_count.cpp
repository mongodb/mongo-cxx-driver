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

#include <mongocxx/options/estimated_document_count.hpp>

//

#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <chrono>

#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/read_preference.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][estimated_document_count]") {
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;
    bsoncxx::v1::stdx::optional<v1::read_preference> read_preference;

    if (has_value) {
        max_time.emplace();
        comment.emplace();
        read_preference.emplace();
    }

    using v_noabi = v_noabi::options::estimated_document_count;
    using v1 = v1::estimated_document_count_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.max_time(*max_time);
            from.comment(*comment);
            from.read_preference(*read_preference);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.max_time() == *max_time);
            CHECK(to.comment() == *comment);
            CHECK(to.read_preference() == *read_preference);
        } else {
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.read_preference().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.max_time(*max_time);
            from.comment(from_v1(comment->view()));
            from.read_preference(*read_preference);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.max_time() == *max_time);
            CHECK(to.comment() == *comment);
            CHECK(to.read_preference() == *read_preference);
        } else {
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.read_preference().has_value());
        }
    }
}

} // namespace mongocxx
