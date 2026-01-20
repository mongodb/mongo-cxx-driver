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

#include <mongocxx/v1/find_one_and_replace_options.hpp>

//

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/return_document.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <chrono>
#include <cstdint>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][find_one_and_replace_options]") {
    find_one_and_replace_options source;
    find_one_and_replace_options target;

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

TEST_CASE("default", "[mongocxx][v1][find_one_and_replace_options]") {
    find_one_and_replace_options const opts;

    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.bypass_document_validation().has_value());
    CHECK_FALSE(opts.hint().has_value());
    CHECK_FALSE(opts.let().has_value());
    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.projection().has_value());
    CHECK_FALSE(opts.return_document().has_value());
    CHECK_FALSE(opts.sort().has_value());
    CHECK_FALSE(opts.upsert().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("collation", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_one_and_replace_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("bypass_document_validation", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_one_and_replace_options{}.bypass_document_validation(v).bypass_document_validation() == v);
}

TEST_CASE("hint", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(find_one_and_replace_options{}.hint(v).hint() == v);
}

TEST_CASE("let", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_one_and_replace_options{}.let(v.value()).let() == v.view());
}

TEST_CASE("comment", "[mongocxx][v1][find_one_and_replace_options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(find_one_and_replace_options{}.comment(v).comment() == v);
}

TEST_CASE("max_time", "[mongocxx][v1][find_one_and_replace_options]") {
    using T = std::chrono::milliseconds;

    auto const v = GENERATE(values({
        T{T::min()},
        T{-1},
        T{0},
        T{1},
        T{T::max()},
    }));

    CHECK(find_one_and_replace_options{}.max_time(v).max_time() == v);
}

TEST_CASE("projection", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_one_and_replace_options{}.projection(v.value()).projection() == v.view());
}

TEST_CASE("return_document", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(v1::return_document::k_before, v1::return_document::k_after);

    CHECK(find_one_and_replace_options{}.return_document(v).return_document() == v);
}

TEST_CASE("sort", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(find_one_and_replace_options{}.sort(v.value()).sort() == v.view());
}

TEST_CASE("upsert", "[mongocxx][v1][find_one_and_replace_options]") {
    auto const v = GENERATE(false, true);

    CHECK(find_one_and_replace_options{}.upsert(v).upsert() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][find_one_and_replace_options]") {
    using T = v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    CHECK(find_one_and_replace_options{}.write_concern(v).write_concern() == v);
}

} // namespace v1
} // namespace mongocxx
