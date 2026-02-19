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

#include <mongocxx/options/count.hpp>

//

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <chrono>
#include <cstdint>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/read_preference.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("count", "[count][option]") {
    options::count cnt;

    auto collation = make_document(kvp("locale", "en_US"));
    auto hint = bsoncxx::document::view_or_value{make_document(kvp("_id", 1))};

    CHECK_OPTIONAL_ARGUMENT(cnt, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(cnt, hint, hint);
    CHECK_OPTIONAL_ARGUMENT(cnt, limit, 3);
    CHECK_OPTIONAL_ARGUMENT(cnt, max_time, std::chrono::milliseconds{1000});
    CHECK_OPTIONAL_ARGUMENT(cnt, read_preference, read_preference{});
    CHECK_OPTIONAL_ARGUMENT(cnt, skip, 3);
}

} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][count]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;
    bsoncxx::v1::stdx::optional<std::int64_t> limit;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_time;
    bsoncxx::v1::stdx::optional<std::int64_t> skip;
    bsoncxx::v1::stdx::optional<v1::read_preference> read_preference;

    if (has_value) {
        collation.emplace();
        hint.emplace("hint");
        comment.emplace();
        limit.emplace();
        max_time.emplace();
        skip.emplace();
        read_preference.emplace();
    }

    using v_noabi = v_noabi::options::count;
    using v1 = v1::count_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.collation(*collation);
            from.hint(*hint);
            from.comment(*comment);
            from.limit(*limit);
            from.max_time(*max_time);
            from.skip(*skip);
            from.read_preference(*read_preference);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.comment().value() == *comment);
            CHECK(to.limit() == *limit);
            CHECK(to.max_time() == *max_time);
            CHECK(to.skip() == *skip);
            CHECK(to.read_preference() == *read_preference);
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.limit().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.skip().has_value());
            CHECK_FALSE(to.read_preference().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.collation(from_v1(collation->view()));
            from.hint(*hint);
            from.comment(from_v1(comment->view()));
            from.limit(*limit);
            from.max_time(*max_time);
            from.skip(*skip);
            from.read_preference(*read_preference);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.comment().value() == *comment);
            CHECK(to.limit() == *limit);
            CHECK(to.max_time() == *max_time);
            CHECK(to.skip() == *skip);
            CHECK(to.read_preference() == *read_preference);
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.limit().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.skip().has_value());
            CHECK_FALSE(to.read_preference().has_value());
        }
    }
}

} // namespace mongocxx
