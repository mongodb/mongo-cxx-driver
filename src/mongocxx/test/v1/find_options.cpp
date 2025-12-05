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

#include <mongocxx/v1/find_options.hpp>

//

#include <mongocxx/v1/cursor.hpp>
#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/test/v1/types/value.hh>

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

#include <mongocxx/private/scoped_bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][find_options]") {
    auto const source_value = 123;
    auto const target_value = 456;

    find_options source;
    find_options target;

    source.limit(source_value);
    target.limit(target_value);

    REQUIRE(source.limit() == source_value);
    REQUIRE(target.limit() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.limit() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.limit() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.limit() == source_value);
        CHECK(copy.limit() == source_value);

        target = copy;

        CHECK(copy.limit() == source_value);
        CHECK(target.limit() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][find_options]") {
    find_options const opts;

    CHECK_FALSE(opts.allow_disk_use().has_value());
    CHECK_FALSE(opts.allow_partial_results().has_value());
    CHECK_FALSE(opts.batch_size().has_value());
    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.cursor_type().has_value());
    CHECK_FALSE(opts.hint().has_value());
    CHECK_FALSE(opts.let().has_value());
    CHECK_FALSE(opts.limit().has_value());
    CHECK_FALSE(opts.max().has_value());
    CHECK_FALSE(opts.max_await_time().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.min().has_value());
    CHECK_FALSE(opts.no_cursor_timeout().has_value());
    CHECK_FALSE(opts.projection().has_value());
    CHECK_FALSE(opts.read_preference().has_value());
    CHECK_FALSE(opts.return_key().has_value());
    CHECK_FALSE(opts.show_record_id().has_value());
    CHECK_FALSE(opts.skip().has_value());
    CHECK_FALSE(opts.sort().has_value());
}

TEST_CASE("allow_disk_use", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_options{}.allow_disk_use(v).allow_disk_use() == v);
}

TEST_CASE("allow_partial_results", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_options{}.allow_partial_results(v).allow_partial_results() == v);
}

TEST_CASE("batch_size", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        std::int32_t{INT32_MIN},
        std::int32_t{-1},
        std::int32_t{0},
        std::int32_t{1},
        std::int32_t{INT32_MAX},
    }));

    CHECK(find_options{}.batch_size(v).batch_size() == v);
}

TEST_CASE("collation", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("comment", "[mongocxx][v1][find_options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(find_options{}.comment(v).comment() == v);
}

TEST_CASE("cursor_type", "[mongocxx][v1][find_options]") {
    using T = v1::cursor::type;

    auto const v = GENERATE(values({
        T::k_non_tailable,
        T::k_tailable,
        T::k_tailable_await,
    }));

    CHECK(find_options{}.cursor_type(v).cursor_type() == v);
}

TEST_CASE("hint", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(find_options{}.hint(v).hint() == v);
}

TEST_CASE("let", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_options{}.let(v.value()).let() == v.view());
}

TEST_CASE("limit", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        std::int64_t{INT64_MIN},
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::int64_t{INT64_MAX},
    }));

    CHECK(find_options{}.limit(v).limit() == v);
}

TEST_CASE("max", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_options{}.max(v.value()).max() == v.view());
}

TEST_CASE("max_await_time", "[mongocxx][v1][find_options]") {
    using T = std::chrono::milliseconds;

    auto const v = GENERATE(values({
        T{T::min()},
        T{-1},
        T{0},
        T{1},
        T{T::max()},
    }));

    CHECK(find_options{}.max_await_time(v).max_await_time() == v);
}

TEST_CASE("max_time", "[mongocxx][v1][find_options]") {
    using T = std::chrono::milliseconds;

    auto const v = GENERATE(values({
        T{T::min()},
        T{-1},
        T{0},
        T{1},
        T{T::max()},
    }));

    CHECK(find_options{}.max_time(v).max_time() == v);
}

TEST_CASE("min", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_options{}.min(v.value()).min() == v.view());
}

TEST_CASE("no_cursor_timeout", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_options{}.no_cursor_timeout(v).no_cursor_timeout() == v);
}

TEST_CASE("projection", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_options{}.projection(v.value()).projection() == v.view());
}

TEST_CASE("read_preference", "[mongocxx][v1][find_options]") {
    using T = v1::read_preference;

    auto const v = GENERATE(values({
        T{},
        T{}.mode(T::read_mode::k_secondary),
        T{}.tags(scoped_bson{R"([1, 2.0, "3"])"}.array_view()),
    }));

    CHECK(find_options{}.read_preference(v).read_preference() == v);
}

TEST_CASE("return_key", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_options{}.return_key(v).return_key() == v);
}

TEST_CASE("show_record_id", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_options{}.show_record_id(v).show_record_id() == v);
}

TEST_CASE("skip", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        std::int64_t{INT64_MIN},
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::int64_t{INT64_MAX},
    }));

    CHECK(find_options{}.skip(v).skip() == v);
}

TEST_CASE("sort", "[mongocxx][v1][find_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_options{}.sort(v.value()).sort() == v.view());
}

} // namespace v1
} // namespace mongocxx
