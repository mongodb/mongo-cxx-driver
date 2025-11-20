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

#include <mongocxx/v1/range_options.hpp>

//

#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][range_options]") {
    range_options source;
    range_options target;

    auto const source_value = 123;
    auto const target_value = 456;

    source.precision(source_value);
    target.precision(target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.precision() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.precision() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.precision() == source_value);
        CHECK(copy.precision() == source_value);

        target = copy;

        CHECK(copy.precision() == source_value);
        CHECK(target.precision() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][range_options]") {
    range_options const opts;

    CHECK_FALSE(opts.min().has_value());
    CHECK_FALSE(opts.max().has_value());
    CHECK_FALSE(opts.sparsity().has_value());
    CHECK_FALSE(opts.trim_factor().has_value());
    CHECK_FALSE(opts.precision().has_value());
}

TEST_CASE("min", "[mongocxx][v1][range_options]") {
    namespace types = bsoncxx::v1::types;

    auto const v = GENERATE(values(
        {types::value{},
         types::value{std::int32_t{123}},
         types::value{std::int64_t{456}},
         types::value{"abc"},
         types::value{scoped_bson{R"({"x": 1})"}.value()}}));

    CHECK(range_options{}.min(v).min() == v);
}

TEST_CASE("max", "[mongocxx][v1][range_options]") {
    namespace types = bsoncxx::v1::types;

    auto const v = GENERATE(values({
        types::value{},
        types::value{std::int32_t{123}},
        types::value{std::int64_t{456}},
        types::value{"abc"},
        types::value{scoped_bson{R"({"x": 1})"}.value()},
        types::value{types::b_null{}},
    }));

    CHECK(range_options{}.max(v).max() == v);
}

TEST_CASE("sparsity", "[mongocxx][v1][range_options]") {
    auto const v = GENERATE(values({
        std::int64_t{INT64_MIN},
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::int64_t{INT64_MAX},
    }));

    CHECK(range_options{}.sparsity(v).sparsity() == v);
}

TEST_CASE("trim_factor", "[mongocxx][v1][range_options]") {
    auto const v = GENERATE(values({
        std::int32_t{INT32_MIN},
        std::int32_t{-1},
        std::int32_t{0},
        std::int32_t{1},
        std::int32_t{INT32_MAX},
    }));

    CHECK(range_options{}.trim_factor(v).trim_factor() == v);
}

TEST_CASE("precision", "[mongocxx][v1][range_options]") {
    auto const v = GENERATE(values({
        std::int32_t{INT32_MIN},
        std::int32_t{-1},
        std::int32_t{0},
        std::int32_t{1},
        std::int32_t{INT32_MAX},
    }));

    CHECK(range_options{}.precision(v).precision() == v);
}

} // namespace v1
} // namespace mongocxx
