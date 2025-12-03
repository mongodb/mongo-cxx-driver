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

#include <mongocxx/v1/distinct_options.hpp>

//

#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <chrono>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][distinct_options]") {
    distinct_options source;
    distinct_options target;

    auto const source_value = std::chrono::milliseconds{123};
    auto const target_value = std::chrono::milliseconds{456};

    source.max_time(source_value);
    target.max_time(target_value);

    REQUIRE(source.max_time() == source_value);
    REQUIRE(target.max_time() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.max_time() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.max_time() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.max_time() == source_value);
        CHECK(copy.max_time() == source_value);

        target = copy;

        CHECK(copy.max_time() == source_value);
        CHECK(target.max_time() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][distinct_options]") {
    distinct_options const opts;

    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.read_preference().has_value());
}

TEST_CASE("collation", "[mongocxx][v1][distinct_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(distinct_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("max_time", "[mongocxx][v1][distinct_options]") {
    using T = std::chrono::milliseconds;

    auto const v = GENERATE(values({
        T{T::min()},
        T{-1},
        T{0},
        T{1},
        T{T::max()},
    }));

    CHECK(distinct_options{}.max_time(v).max_time() == v);
}

TEST_CASE("comment", "[mongocxx][v1][distinct_options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(distinct_options{}.comment(v).comment() == v);
}

TEST_CASE("read_preference", "[mongocxx][v1][distinct_options]") {
    using T = mongocxx::v1::read_preference;

    auto const v = GENERATE(values({
        T{},
        T{}.mode(T::read_mode::k_secondary),
        T{}.tags(scoped_bson{R"([1, 2.0, "3"])"}.array_view()),
    }));

    CHECK(distinct_options{}.read_preference(v).read_preference() == v);
}

} // namespace v1
} // namespace mongocxx
