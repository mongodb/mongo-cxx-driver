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

#include <mongocxx/v1/pipeline.hpp>

//

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <tuple>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][pipeline]") {
    pipeline source;
    pipeline target;

    source.append_stage(scoped_bson{R"({"$stage": "source"})"}.view());
    target.append_stage(scoped_bson{R"({"$stage": "target"})"}.view());

    CHECK(source.view_array() == scoped_bson{R"([{"$stage": "source"}])"}.array_view());
    CHECK(target.view_array() == scoped_bson{R"([{"$stage": "target"}])"}.array_view());

    auto const source_value = source.view_array();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.view_array() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.view_array() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.view_array() == source_value);
        CHECK(copy.view_array() == source_value);

        target = copy;

        CHECK(copy.view_array() == source_value);
        CHECK(target.view_array() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][pipeline]") {
    pipeline const v;

    CHECK(v.view_array().empty());
}

TEST_CASE("append_stage", "[mongocxx][v1][pipeline]") {
    pipeline v;

    v.append_stage(scoped_bson{R"({"a": 1})"}.view());
    CHECK(v.view_array() == scoped_bson{R"([{"a": 1}])"}.view());

    v.append_stage(scoped_bson{R"({"b": 2})"}.view());
    CHECK(v.view_array() == scoped_bson{R"([{"a": 1}, {"b": 2}])"}.view());
}

TEST_CASE("append_stages", "[mongocxx][v1][pipeline]") {
    static bsoncxx::v1::array::view const empty;

    pipeline v;

    SECTION("valid") {
        v.append_stages(empty);
        CHECK(v.view_array() == empty);

        v.append_stages(scoped_bson{R"([{"a": 1}, {"b": 2}])"}.array_view());
        CHECK(v.view_array() == scoped_bson{R"([{"a": 1}, {"b": 2}])"}.view());

        v.append_stages(scoped_bson{R"([{"c": 3}, {"d": 4}])"}.array_view());
        CHECK(v.view_array() == scoped_bson{R"([{"a": 1}, {"b": 2}, {"c": 3}, {"d": 4}])"}.view());
    }

    SECTION("invalid") {
        SECTION("values") {
            auto const input = GENERATE(values({
                scoped_bson{R"([123])"},
                scoped_bson{R"(["abc"])"},
                scoped_bson{R"([["x"]])"},
            }));

            CAPTURE(input.view());

            CHECK_THROWS_WITH_CODE(v.append_stages(input.array_view()), bsoncxx::v1::types::view::errc::type_mismatch);

            CHECK(v.view_array() == empty);
        }

        SECTION("elements") {
            scoped_bson const input{R"([{"a": 1}, "throws", {"b": 2}])"};

            CHECK_THROWS_WITH_CODE(v.append_stages(input.array_view()), bsoncxx::v1::types::view::errc::type_mismatch);

            CHECK(v.view_array() == scoped_bson{R"([{"a": 1}])"}.view());
        }
    }
}

TEST_CASE("document", "[mongocxx][v1][pipeline]") {
    SECTION("common") {
        using mem_fn_type = pipeline& (pipeline::*)(bsoncxx::v1::document::view);

        mem_fn_type mem_fn = {};
        char const* name = {};

        std::tie(mem_fn, name) = GENERATE(
            table<mem_fn_type, char const*>({
                {&pipeline::add_fields, "$addFields"},
                {&pipeline::bucket, "$bucket"},
                {&pipeline::bucket_auto, "$bucketAuto"},
                {&pipeline::coll_stats, "$collStats"},
                {&pipeline::current_op, "$currentOp"},
                {&pipeline::facet, "$facet"},
                {&pipeline::geo_near, "$geoNear"},
                {&pipeline::graph_lookup, "$graphLookup"},
                {&pipeline::group, "$group"},
                {&pipeline::list_local_sessions, "$listLocalSessions"},
                {&pipeline::list_sessions, "$listSessions"},
                {&pipeline::lookup, "$lookup"},
                {&pipeline::match, "$match"},
                {&pipeline::merge, "$merge"},
                {&pipeline::project, "$project"},
                {&pipeline::redact, "$redact"},
                {&pipeline::replace_root, "$replaceRoot"},
                {&pipeline::sort, "$sort"},
                {&pipeline::sort_by_count, "$sortByCount"},
                {&pipeline::unwind, "$unwind"},
            }));

        CAPTURE(name);

        auto const input = GENERATE(values({
            scoped_bson{},
            scoped_bson{R"({"x": 1})"},
            scoped_bson{R"({"x": 1, "y": 2})"},
        }));

        CAPTURE(input);

        auto const v = (pipeline{}.*mem_fn)(input.view());
        auto const expected = scoped_bson{BCON_NEW("0", "{", name, BCON_DOCUMENT(input.bson()), "}")};

        CHECK(v.view_array() == expected.view());
    }

    SECTION("coll_stats") {
        auto const v = pipeline{}.coll_stats();
        auto const expected = scoped_bson{R"({"0": {"$collStats": {}}})"};

        CHECK(v.view_array() == expected.view());
    }

    SECTION("index_stats") {
        auto const v = pipeline{}.index_stats();
        auto const expected = scoped_bson{R"({"0": {"$indexStats": {}}})"};

        CHECK(v.view_array() == expected.view());
    }
}

TEST_CASE("string", "[mongocxx][v1][pipeline]") {
    using mem_fn_type = pipeline& (pipeline::*)(bsoncxx::v1::stdx::string_view);

    mem_fn_type mem_fn = {};
    char const* name = {};

    std::tie(mem_fn, name) = GENERATE(
        table<mem_fn_type, char const*>({
            {&pipeline::count, "$count"},
            {&pipeline::out, "$out"},
            {&pipeline::sort_by_count, "$sortByCount"},
            {&pipeline::unwind, "$unwind"},
        }));

    CAPTURE(name);

    auto const input = GENERATE(values({
        "",
        "x",
        "abc",
    }));

    CAPTURE(input);

    auto const v = (pipeline{}.*mem_fn)(input);
    auto const expected = scoped_bson{BCON_NEW("0", "{", name, BCON_UTF8(input), "}")};

    CHECK(v.view_array() == expected.view());
}

TEST_CASE("int32", "[mongocxx][v1][pipeline]") {
    auto const input = GENERATE(values({
        std::int32_t{INT32_MIN},
        std::int32_t{-1},
        std::int32_t{0},
        std::int32_t{1},
        std::int32_t{INT32_MAX},
    }));

    SECTION("common") {
        using mem_fn_type = pipeline& (pipeline::*)(std::int32_t);

        mem_fn_type mem_fn = {};
        char const* name = {};

        std::tie(mem_fn, name) = GENERATE(
            table<mem_fn_type, char const*>({
                {&pipeline::limit, "$limit"},
                {&pipeline::skip, "$skip"},
            }));

        CAPTURE(name);

        CAPTURE(input);

        auto const v = (pipeline{}.*mem_fn)(input);
        auto const expected = scoped_bson{BCON_NEW("0", "{", name, BCON_INT32(input), "}")};

        CHECK(v.view_array() == expected.view());
    }

    SECTION("sample") {
        auto const v = pipeline{}.sample(input);
        auto const expected = scoped_bson{BCON_NEW("0", "{", "$sample", "{", "size", BCON_INT32(input), "}", "}")};

        CHECK(v.view_array() == expected.view());
    }
}

TEST_CASE("int64", "[mongocxx][v1][pipeline]") {
    using mem_fn_type = pipeline& (pipeline::*)(std::int64_t);

    mem_fn_type mem_fn = {};
    char const* name = {};

    std::tie(mem_fn, name) = GENERATE(
        table<mem_fn_type, char const*>({
            {&pipeline::limit, "$limit"},
            {&pipeline::skip, "$skip"},
        }));

    CAPTURE(name);

    auto const input = GENERATE(values({
        std::int64_t{INT64_MIN},
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::int64_t{INT64_MAX},
    }));

    CAPTURE(input);

    auto const v = (pipeline{}.*mem_fn)(input);
    auto const expected = scoped_bson{BCON_NEW("0", "{", name, BCON_INT64(input), "}")};

    CHECK(v.view_array() == expected.view());
}

} // namespace v1
} // namespace mongocxx
