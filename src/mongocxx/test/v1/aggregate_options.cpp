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

#include <mongocxx/v1/aggregate_options.hpp>

//

#include <mongocxx/v1/hint.hpp>

#include <bsoncxx/test/v1/stdx/string_view.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>
#include <mongocxx/test/v1/read_concern.hh>
#include <mongocxx/test/v1/read_preference.hh>
#include <mongocxx/test/v1/write_concern.hh>

#include <chrono>
#include <cstdint>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][aggregate_options]") {
    aggregate_options source;
    aggregate_options target;

    source.comment(bsoncxx::v1::types::value{"source"});
    target.comment(bsoncxx::v1::types::value{"target"});

    REQUIRE(source.comment() == bsoncxx::v1::types::b_string{"source"});
    REQUIRE(target.comment() == bsoncxx::v1::types::b_string{"target"});

    auto const source_value = source.comment();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.comment() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.comment() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.comment() == source_value);
        CHECK(copy.comment() == source_value);

        target = copy;

        CHECK(copy.comment() == source_value);
        CHECK(target.comment() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][aggregate_options]") {
    aggregate_options const opts;

    CHECK_FALSE(opts.allow_disk_use().has_value());
    CHECK_FALSE(opts.batch_size().has_value());
    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.let().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.read_preference().has_value());
    CHECK_FALSE(opts.bypass_document_validation().has_value());
    CHECK_FALSE(opts.hint().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
    CHECK_FALSE(opts.read_concern().has_value());
    CHECK_FALSE(opts.comment().has_value());
}

TEST_CASE("allow_disk_use", "[mongocxx][v1][aggregate_options]") {
    auto const v = GENERATE(false, true);

    CHECK(aggregate_options{}.allow_disk_use(v).allow_disk_use() == v);
}

TEST_CASE("batch_size", "[mongocxx][v1][aggregate_options]") {
    auto const v = GENERATE(values({
        std::int32_t{INT32_MIN},
        std::int32_t{-1},
        std::int32_t{0},
        std::int32_t{1},
        std::int32_t{INT32_MAX},
    }));

    CHECK(aggregate_options{}.batch_size(v).batch_size() == v);
}

TEST_CASE("collation", "[mongocxx][v1][aggregate_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(aggregate_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("let", "[mongocxx][v1][aggregate_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(aggregate_options{}.let(v.value()).let() == v.view());
}

TEST_CASE("max_time", "[mongocxx][v1][aggregate_options]") {
    using T = std::chrono::milliseconds;

    auto const v = GENERATE(values({
        T{T::min()},
        T{-1},
        T{0},
        T{1},
        T{T::max()},
    }));

    CHECK(aggregate_options{}.max_time(v).max_time() == v);
}

TEST_CASE("read_preference", "[mongocxx][v1][aggregate_options]") {
    using T = mongocxx::v1::read_preference;

    auto const v = GENERATE(values({
        T{},
        T{}.mode(T::read_mode::k_secondary),
        T{}.tags(scoped_bson{R"([1, 2.0, "3"])"}.array_view()),
    }));

    CHECK(aggregate_options{}.read_preference(v).read_preference() == v);
}

TEST_CASE("bypass_document_validation", "[mongocxx][v1][aggregate_options]") {
    auto const v = GENERATE(false, true);

    CHECK(aggregate_options{}.bypass_document_validation(v).bypass_document_validation() == v);
}

TEST_CASE("hint", "[mongocxx][v1][aggregate_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(aggregate_options{}.hint(v).hint() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][aggregate_options]") {
    using T = v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    CHECK(aggregate_options{}.write_concern(v).write_concern() == v);
}

TEST_CASE("read_concern", "[mongocxx][v1][aggregate_options]") {
    using T = v1::read_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
    }));

    CHECK(aggregate_options{}.read_concern(v).read_concern() == v);
}

TEST_CASE("comment", "[mongocxx][v1][aggregate_options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(aggregate_options{}.comment(v).comment() == v);
}

} // namespace v1
} // namespace mongocxx
