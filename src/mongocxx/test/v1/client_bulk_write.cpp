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

#include <mongocxx/v1/client_bulk_write.hh>

//

#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][options]") {
    client_bulk_write::options source;
    client_bulk_write::options target;

    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    source.comment(source_value);
    target.comment(target_value);

    REQUIRE(source.comment() == source_value);
    REQUIRE(target.comment() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.comment() == source_value);

        target = std::move(move);

        // move is in an assign-or-move-only state.

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

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][options]") {
    client_bulk_write::options const opts;

    CHECK_FALSE(opts.bypass_document_validation().has_value());
    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.let().has_value());
    CHECK(opts.ordered());
    CHECK_FALSE(opts.verbose_results().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("bypass_document_validation", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::options{}.bypass_document_validation(v).bypass_document_validation() == v);
}

TEST_CASE("comment", "[mongocxx][v1][client_bulk_write][options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(client_bulk_write::options{}.comment(v).comment() == v);
}

TEST_CASE("let", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(client_bulk_write::options{}.let(v.value()).let() == v.view());
}

TEST_CASE("ordered", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::options{}.ordered(v).ordered() == v);
}

TEST_CASE("verbose_results", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::options{}.verbose_results(v).verbose_results() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][client_bulk_write][options]") {
    using T = v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    CHECK(client_bulk_write::options{}.write_concern(v).write_concern() == v);
}

} // namespace v1
} // namespace mongocxx
