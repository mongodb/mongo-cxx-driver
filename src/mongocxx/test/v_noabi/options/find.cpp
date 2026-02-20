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

#include <mongocxx/options/find.hpp>

//

#include <mongocxx/v1/cursor.hpp>
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
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/read_preference.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("find", "[find][option]") {
    options::find find_opts{};

    auto collation = make_document(kvp("locale", "en_US"));
    auto hint = bsoncxx::document::view_or_value{make_document(kvp("_id", 1))};
    auto max = make_document(kvp("a", 6));
    auto min = make_document(kvp("a", 3));
    auto projection = make_document(kvp("_id", false));
    auto sort = make_document(kvp("x", -1));

    CHECK_OPTIONAL_ARGUMENT(find_opts, allow_partial_results, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, batch_size, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, comment, "comment");
    CHECK_OPTIONAL_ARGUMENT(find_opts, cursor_type, cursor::type::k_non_tailable);
    CHECK_OPTIONAL_ARGUMENT(find_opts, hint, hint);
    CHECK_OPTIONAL_ARGUMENT(find_opts, limit, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, max, max.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_await_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, min, min.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, no_cursor_timeout, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, projection, projection.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, read_preference, read_preference{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, return_key, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, show_record_id, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, skip, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, sort, sort.view());
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][find]") {
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;
    using mongocxx::v_noabi::to_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> allow_disk_use;
    bsoncxx::v1::stdx::optional<bool> allow_partial_results;
    bsoncxx::v1::stdx::optional<std::int32_t> batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<v1::cursor::type> cursor_type;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;
    bsoncxx::v1::stdx::optional<std::int64_t> limit;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> max;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_await_time;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> min;
    bsoncxx::v1::stdx::optional<bool> no_cursor_timeout;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> projection;
    bsoncxx::v1::stdx::optional<v1::read_preference> read_preference;
    bsoncxx::v1::stdx::optional<bool> return_key;
    bsoncxx::v1::stdx::optional<bool> show_record_id;
    bsoncxx::v1::stdx::optional<std::int64_t> skip;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> ordering;

    if (has_value) {
        allow_disk_use.emplace();
        allow_partial_results.emplace();
        batch_size.emplace();
        collation.emplace();
        cursor_type.emplace();
        hint.emplace("hint");
        let.emplace();
        comment.emplace();
        limit.emplace();
        max.emplace();
        max_await_time.emplace();
        max_time.emplace();
        min.emplace();
        no_cursor_timeout.emplace();
        projection.emplace();
        read_preference.emplace();
        return_key.emplace();
        show_record_id.emplace();
        skip.emplace();
        ordering.emplace();
    }

    using v_noabi = v_noabi::options::find;
    using v1 = v1::find_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.allow_disk_use(*allow_disk_use);
            from.allow_partial_results(*allow_partial_results);
            from.batch_size(*batch_size);
            from.collation(*collation);
            from.cursor_type(*cursor_type);
            from.hint(*hint);
            from.let(*let);
            from.comment(*comment);
            from.limit(*limit);
            from.max(*max);
            from.max_await_time(*max_await_time);
            from.max_time(*max_time);
            from.min(*min);
            from.no_cursor_timeout(*no_cursor_timeout);
            from.projection(*projection);
            from.read_preference(*read_preference);
            from.return_key(*return_key);
            from.show_record_id(*show_record_id);
            from.skip(*skip);
            from.sort(*ordering);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.allow_disk_use() == *allow_disk_use);
            CHECK(to.allow_partial_results() == *allow_partial_results);
            CHECK(to.batch_size() == *batch_size);
            CHECK(to.comment_option().value().view() == comment->view());
            CHECK(to.collation().value() == collation->view());
            CHECK(to.cursor_type() == *cursor_type);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.let().value() == let->view());
            CHECK(to.limit() == *limit);
            CHECK(to.max().value() == max->view());
            CHECK(to.max_await_time() == *max_await_time);
            CHECK(to.max_time() == *max_time);
            CHECK(to.min().value() == min->view());
            CHECK(to.no_cursor_timeout() == *no_cursor_timeout);
            CHECK(to.projection().value() == projection->view());
            CHECK(to.read_preference() == *read_preference);
            CHECK(to.return_key() == *return_key);
            CHECK(to.show_record_id() == *show_record_id);
            CHECK(to.skip() == *skip);
            CHECK(to.sort().value() == ordering->view());
        } else {
            CHECK_FALSE(to.allow_disk_use().has_value());
            CHECK_FALSE(to.allow_partial_results().has_value());
            CHECK_FALSE(to.batch_size().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.cursor_type().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.limit().has_value());
            CHECK_FALSE(to.max().has_value());
            CHECK_FALSE(to.max_await_time().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.min().has_value());
            CHECK_FALSE(to.no_cursor_timeout().has_value());
            CHECK_FALSE(to.projection().has_value());
            CHECK_FALSE(to.read_preference().has_value());
            CHECK_FALSE(to.return_key().has_value());
            CHECK_FALSE(to.show_record_id().has_value());
            CHECK_FALSE(to.skip().has_value());
            CHECK_FALSE(to.sort().has_value());
        }

        CHECK_FALSE(to.comment().has_value());
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.allow_disk_use(*allow_disk_use);
            from.allow_partial_results(*allow_partial_results);
            from.batch_size(*batch_size);
            from.collation(from_v1(collation->view()));
            from.cursor_type(*cursor_type);
            from.hint(*hint);
            from.let(from_v1(let->view()));
            from.comment_option(from_v1(comment->view()));
            from.limit(*limit);
            from.max(from_v1(max->view()));
            from.max_await_time(*max_await_time);
            from.max_time(*max_time);
            from.min(from_v1(min->view()));
            from.no_cursor_timeout(*no_cursor_timeout);
            from.projection(from_v1(projection->view()));
            from.read_preference(*read_preference);
            from.return_key(*return_key);
            from.show_record_id(*show_record_id);
            from.skip(*skip);
            from.sort(from_v1(ordering->view()));
        }

        auto const with_comment_str = GENERATE(false, true);

        bsoncxx::v1::types::value comment_str{"comment"};

        if (with_comment_str) {
            from.comment(comment_str.get_string().value);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.allow_disk_use() == *allow_disk_use);
            CHECK(to.allow_partial_results() == *allow_partial_results);
            CHECK(to.batch_size() == *batch_size);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.comment().value() == comment->view());
            CHECK(to.cursor_type() == *cursor_type);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.let().value() == let->view());
            CHECK(to.limit() == *limit);
            CHECK(to.max().value() == max->view());
            CHECK(to.max_await_time() == *max_await_time);
            CHECK(to.max_time() == *max_time);
            CHECK(to.min().value() == min->view());
            CHECK(to.no_cursor_timeout() == *no_cursor_timeout);
            CHECK(to.projection().value() == projection->view());
            CHECK(to.read_preference() == *read_preference);
            CHECK(to.return_key() == *return_key);
            CHECK(to.show_record_id() == *show_record_id);
            CHECK(to.skip() == *skip);
            CHECK(to.sort().value() == ordering->view());
        } else {
            CHECK_FALSE(to.allow_disk_use().has_value());
            CHECK_FALSE(to.allow_partial_results().has_value());
            CHECK_FALSE(to.batch_size().has_value());
            CHECK_FALSE(to.collation().has_value());

            if (with_comment_str) {
                CHECK(to.comment() == comment_str);
            } else {
                CHECK_FALSE(to.comment().has_value());
            }

            CHECK_FALSE(to.cursor_type().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.limit().has_value());
            CHECK_FALSE(to.max().has_value());
            CHECK_FALSE(to.max_await_time().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.min().has_value());
            CHECK_FALSE(to.no_cursor_timeout().has_value());
            CHECK_FALSE(to.projection().has_value());
            CHECK_FALSE(to.read_preference().has_value());
            CHECK_FALSE(to.return_key().has_value());
            CHECK_FALSE(to.show_record_id().has_value());
            CHECK_FALSE(to.skip().has_value());
            CHECK_FALSE(to.sort().has_value());
        }
    }
}

} // namespace mongocxx
