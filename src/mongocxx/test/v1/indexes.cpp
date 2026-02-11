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

#include <mongocxx/v1/indexes.hh>

//

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <chrono>
#include <cstdint>
#include <string>
#include <system_error>
#include <utility>

#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = mongocxx::v1::indexes::errc;

namespace {

struct identity_type {};

} // namespace

TEST_CASE("error code", "[mongocxx][v1][indexes][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = mongocxx::v1::indexes::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::indexes"));

    auto const zero_errc = make_error_condition(static_cast<std::errc>(0));

    SECTION("unknown") {
        std::error_code const ec = static_cast<code>(-1);

        CHECK(ec.category() == category);
        CHECK(ec.value() == -1);
        CHECK(ec);
        CHECK(ec.message() == std::string(category.name()) + ":-1");
    }

    SECTION("zero") {
        std::error_code const ec = code::zero;

        CHECK(ec.category() == category);
        CHECK(ec.value() == 0);
        CHECK_FALSE(ec);
        CHECK(ec.message() == "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("non-zero") {
        std::error_code const ec = code::invalid_name;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_name) == source_errc::mongocxx);
        CHECK(make_error_code(code::expired_after_i32) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_name) == type_errc::invalid_argument);
        CHECK(make_error_code(code::expired_after_i32) == type_errc::invalid_argument);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes]") {
    identity_type coll1_identity;
    identity_type coll2_identity;

    auto const coll1_id = reinterpret_cast<mongoc_collection_t*>(&coll1_identity);
    auto const coll2_id = reinterpret_cast<mongoc_collection_t*>(&coll2_identity);

    identity_type client1_identity;
    identity_type client2_identity;

    auto const client1_id = reinterpret_cast<mongoc_client_t*>(&client1_identity);
    auto const client2_id = reinterpret_cast<mongoc_client_t*>(&client2_identity);

    auto source = v1::indexes::internal::make(coll1_id, client1_id);
    auto target = v1::indexes::internal::make(coll2_id, client2_id);

    auto const get_collection = [](indexes const& idx) { return v1::indexes::internal::get_collection(idx); };

    REQUIRE(get_collection(source) == coll1_id);
    REQUIRE(get_collection(target) == coll2_id);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(get_collection(move) == coll1_id);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(get_collection(target) == coll1_id);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(get_collection(source) == coll1_id);
        CHECK(get_collection(copy) == coll1_id);

        target = copy;

        CHECK(get_collection(copy) == coll1_id);
        CHECK(get_collection(target) == coll1_id);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes][options]") {
    indexes::options source;
    indexes::options target;

    auto const source_value = "source";
    auto const target_value = "target";

    source.name(source_value);
    target.name(target_value);

    REQUIRE(source.name() == source_value);
    REQUIRE(target.name() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.name() == source_value);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.name() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.name() == source_value);
        CHECK(copy.name() == source_value);

        target = copy;

        CHECK(copy.name() == source_value);
        CHECK(target.name() == source_value);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes][model]") {
    scoped_bson source_value{R"({"source": 1})"};
    scoped_bson target_value{R"({"target": 2})"};

    indexes::model source{source_value.value()};
    indexes::model target{target_value.value()};

    REQUIRE(source.keys() == source_value.view());
    REQUIRE(target.keys() == target_value.view());

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.keys() == source_value.view());

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.keys() == source_value.view());
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.keys() == source_value.view());
        CHECK(copy.keys() == source_value.view());

        target = copy;

        CHECK(copy.keys() == source_value.view());
        CHECK(target.keys() == source_value.view());
    }
}

TEST_CASE("ownership", "[mongocxx][v1][indexes][create_one_options]") {
    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    indexes::create_one_options source;
    indexes::create_one_options target;

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

TEST_CASE("ownership", "[mongocxx][v1][indexes][create_many_options]") {
    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    indexes::create_many_options source;
    indexes::create_many_options target;

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

TEST_CASE("ownership", "[mongocxx][v1][indexes][drop_one_options]") {
    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    indexes::drop_one_options source;
    indexes::drop_one_options target;

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

TEST_CASE("ownership", "[mongocxx][v1][indexes][drop_all_options]") {
    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    indexes::drop_all_options source;
    indexes::drop_all_options target;

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

TEST_CASE("ownership", "[mongocxx][v1][indexes][list_options]") {
    auto const source_value = 1;
    auto const target_value = 2;

    indexes::list_options source;
    indexes::list_options target;

    source.batch_size(source_value);
    target.batch_size(target_value);

    REQUIRE(source.batch_size() == source_value);
    REQUIRE(target.batch_size() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.batch_size() == source_value);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.batch_size() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.batch_size() == source_value);
        CHECK(copy.batch_size() == source_value);

        target = copy;

        CHECK(copy.batch_size() == source_value);
        CHECK(target.batch_size() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][indexes][options]") {
    indexes::options const opts;

    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.partial_filter_expression().has_value());
    CHECK_FALSE(opts.storage_engine().has_value());
    CHECK_FALSE(opts.weights().has_value());
    CHECK_FALSE(opts.wildcard_projection().has_value());
    CHECK_FALSE(opts.default_language().has_value());
    CHECK_FALSE(opts.language_override().has_value());
    CHECK_FALSE(opts.name().has_value());
    CHECK_FALSE(opts.twod_location_max().has_value());
    CHECK_FALSE(opts.twod_location_min().has_value());
    CHECK_FALSE(opts.expire_after().has_value());
    CHECK_FALSE(opts.text_index_version().has_value());
    CHECK_FALSE(opts.version().has_value());
    CHECK_FALSE(opts.twod_bits_precision().has_value());
    CHECK_FALSE(opts.twod_sphere_version().has_value());
    CHECK_FALSE(opts.background().has_value());
    CHECK_FALSE(opts.hidden().has_value());
    CHECK_FALSE(opts.sparse().has_value());
    CHECK_FALSE(opts.unique().has_value());
}

TEST_CASE("default", "[mongocxx][v1][indexes][create_one_options]") {
    indexes::create_one_options const opts;

    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.commit_quorum().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("default", "[mongocxx][v1][indexes][create_many_options]") {
    indexes::create_many_options const opts;

    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.commit_quorum().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("default", "[mongocxx][v1][indexes][drop_one_options]") {
    indexes::drop_one_options const opts;

    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("default", "[mongocxx][v1][indexes][drop_all_options]") {
    indexes::drop_all_options const opts;

    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.max_time().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("default", "[mongocxx][v1][indexes][list_options]") {
    indexes::list_options const opts;

    CHECK_FALSE(opts.batch_size().has_value());
    CHECK_FALSE(opts.comment().has_value());
}

TEST_CASE("collation", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"}).value();

    CHECK(indexes::options{}.collation(v).collation() == v);
}

TEST_CASE("partial_filter_expression", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"}).value();

    CHECK(indexes::options{}.partial_filter_expression(v).partial_filter_expression() == v);
}

TEST_CASE("storage_engine", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"}).value();

    CHECK(indexes::options{}.storage_engine(v).storage_engine() == v);
}

TEST_CASE("weights", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"}).value();

    CHECK(indexes::options{}.weights(v).weights() == v);
}

TEST_CASE("wildcard_projection", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"}).value();

    CHECK(indexes::options{}.wildcard_projection(v).wildcard_projection() == v);
}

TEST_CASE("default_language", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(values<std::string>({{}, "abc"}));

    CHECK(indexes::options{}.default_language(v).default_language() == v);
}

TEST_CASE("language_override", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(values<std::string>({{}, "abc"}));

    CHECK(indexes::options{}.language_override(v).language_override() == v);
}

TEST_CASE("name", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(values<std::string>({{}, "abc"}));

    CHECK(indexes::options{}.name(v).name() == v);
}

TEST_CASE("twod_location_max", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(0.0, 1.0);

    CHECK(indexes::options{}.twod_location_max(v).twod_location_max() == v);
}

TEST_CASE("twod_location_min", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(0.0, 1.0);

    CHECK(indexes::options{}.twod_location_min(v).twod_location_min() == v);
}

TEST_CASE("expire_after", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(as<std::chrono::seconds>(), 0, 1);

    CHECK(indexes::options{}.expire_after(v).expire_after() == v);
}

TEST_CASE("text_index_version", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(as<std::int32_t>(), INT32_MIN, -1, 0, 1, INT32_MAX);

    CHECK(indexes::options{}.text_index_version(v).text_index_version() == v);
}

TEST_CASE("version", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(as<std::int32_t>(), INT32_MIN, -1, 0, 1, INT32_MAX);

    CHECK(indexes::options{}.version(v).version() == v);
}

TEST_CASE("twod_bits_precision", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(as<std::uint8_t>(), 0u, 1u, UINT8_MAX - 1u, UINT8_MAX);

    CHECK(indexes::options{}.twod_bits_precision(v).twod_bits_precision() == v);
}

TEST_CASE("twod_sphere_version", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(as<std::uint8_t>(), 0u, 1u, UINT8_MAX - 1u, UINT8_MAX);

    CHECK(indexes::options{}.twod_sphere_version(v).twod_sphere_version() == v);
}

TEST_CASE("background", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(false, true);

    CHECK(indexes::options{}.background(v).background() == v);
}

TEST_CASE("hidden", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(false, true);

    CHECK(indexes::options{}.hidden(v).hidden() == v);
}

TEST_CASE("sparse", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(false, true);

    CHECK(indexes::options{}.sparse(v).sparse() == v);
}

TEST_CASE("unique", "[mongocxx][v1][indexes][options]") {
    auto const v = GENERATE(false, true);

    CHECK(indexes::options{}.unique(v).unique() == v);
}

namespace {

template <typename CreateIndexOptions>
void test_create_index_options() {
    SECTION("comment") {
        auto const v = GENERATE(as<bsoncxx::v1::types::value>(), bsoncxx::v1::types::b_null{}, 1, 2.0, "three");
    }

    SECTION("commit_quorum") {
        auto const v = GENERATE(as<scoped_bson>(), scoped_bson{}, R"({"x": 1})").value();

        CHECK(CreateIndexOptions{}.commit_quorum(v).commit_quorum() == v);
    }

    SECTION("max_time") {
        auto const v = GENERATE(as<std::chrono::milliseconds>(), 0, 1);

        CHECK(CreateIndexOptions{}.max_time(v).max_time() == v);
    }

    SECTION("write_concern") {
        using T = v1::write_concern;

        auto const v = GENERATE(values({
            T{},
            T{}.acknowledge_level(T::level::k_majority),
            T{}.tag("abc"),
        }));

        CHECK(CreateIndexOptions{}.write_concern(v).write_concern() == v);
    }
}

} // namespace

TEST_CASE("CreateIndexOptions", "[mongocxx][v1][indexes][create_one_options]") {
    test_create_index_options<indexes::create_one_options>();
}

TEST_CASE("CreateIndexOptions", "[mongocxx][v1][indexes][create_many_options]") {
    test_create_index_options<indexes::create_many_options>();
}

namespace {

template <typename DropIndexOptions>
void test_drop_index_options() {
    SECTION("comment") {
        using T = bsoncxx::v1::types::value;

        auto const v = GENERATE(values({
            T{},
            T{std::int32_t{123}},
            T{std::int64_t{456}},
            T{123.456},
            T{"abc"},
        }));

        CHECK(DropIndexOptions{}.comment(v).comment() == v);
    }

    SECTION("max_time") {
        auto const v = GENERATE(as<std::chrono::milliseconds>(), 0, 1);

        CHECK(DropIndexOptions{}.max_time(v).max_time() == v);
    }

    SECTION("write_concern") {
        using T = v1::write_concern;

        auto const v = GENERATE(values({
            T{},
            T{}.acknowledge_level(T::level::k_majority),
            T{}.tag("abc"),
        }));

        CHECK(DropIndexOptions{}.write_concern(v).write_concern() == v);
    }
}

} // namespace

TEST_CASE("DropIndexOptions", "[mongocxx][v1][indexes][drop_one_options]") {
    test_drop_index_options<indexes::create_one_options>();
}

TEST_CASE("DropIndexOptions", "[mongocxx][v1][indexes][drop_all_options]") {
    test_drop_index_options<indexes::create_many_options>();
}

TEST_CASE("batch_size", "[mongocxx][v1][indexes][list_options]") {
    auto const v = GENERATE(values({
        std::int32_t{INT32_MIN},
        std::int32_t{-1},
        std::int32_t{0},
        std::int32_t{1},
        std::int32_t{INT32_MAX},
    }));

    CHECK(indexes::list_options{}.batch_size(v).batch_size() == v);
}

TEST_CASE("comment", "[mongocxx][v1][indexes][list_options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(indexes::list_options{}.comment(v).comment() == v);
}

} // namespace v1
} // namespace mongocxx
