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

#include <mongocxx/v1/gridfs/bucket.hh>

//

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/read_concern.hpp>    // IWYU pragma: keep
#include <mongocxx/v1/read_preference.hpp> // IWYU pragma: keep
#include <mongocxx/v1/write_concern.hpp>   // IWYU pragma: keep

#include <mongocxx/v1/collection.hh>

#include <mongocxx/test/private/scoped_bson.hh>
#include <mongocxx/test/v1/read_concern.hh>
#include <mongocxx/test/v1/read_preference.hh>
#include <mongocxx/test/v1/write_concern.hh>

#include <cstdint>
#include <string>
#include <system_error>
#include <utility>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {
namespace gridfs {

using code = bucket::errc;

TEST_CASE("error code", "[mongocxx][v1][gridfs][bucket][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = bucket::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::gridfs::bucket"));

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
        std::error_code const ec = code::invalid_bucket_name;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::invalid_bucket_name) == source_errc::mongocxx);
        CHECK(make_error_code(code::invalid_chunk_size_bytes) == source_errc::mongocxx);
        CHECK(make_error_code(code::not_found) == source_errc::mongocxx);
        CHECK(make_error_code(code::corrupt_data) == source_errc::mongocxx);
        CHECK(make_error_code(code::invalid_byte_range) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::invalid_bucket_name) == type_errc::invalid_argument);
        CHECK(make_error_code(code::invalid_chunk_size_bytes) == type_errc::invalid_argument);
        CHECK(make_error_code(code::not_found) == type_errc::runtime_error);
        CHECK(make_error_code(code::corrupt_data) == type_errc::runtime_error);
        CHECK(make_error_code(code::invalid_byte_range) == type_errc::invalid_argument);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][gridfs][bucket]") {
    struct identity_type {};

    identity_type id1;
    identity_type id2;
    identity_type id3;

    auto const coll1 = reinterpret_cast<mongoc_collection_t*>(&id1);
    auto const coll2 = reinterpret_cast<mongoc_collection_t*>(&id2);
    auto const coll3 = reinterpret_cast<mongoc_collection_t*>(&id3);

    auto destroy = libmongoc::collection_destroy.create_instance();
    auto copy = libmongoc::collection_copy.create_instance();

    int destroy_count = 0;
    int copy_count = 0;

    destroy
        ->interpose([&](mongoc_collection_t* ptr) -> void {
            if (ptr) {
                if (ptr != coll1 && ptr != coll2 && ptr != coll3) {
                    FAIL("unexpected mongoc_collection_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    copy->interpose([&](mongoc_collection_t* ptr) -> mongoc_collection_t* {
            if (ptr == coll1) {
                ++copy_count;
                return coll2;
            } else if (ptr == coll2) {
                ++copy_count;
                return coll3;
            } else {
                FAIL("unexpected mongoc_collection_t");
                return nullptr;
            }
        })
        .forever();

    auto source = bucket::internal::make(
        v1::collection::internal::make(coll1, nullptr), v1::collection::internal::make(coll1, nullptr), "source", 123);
    auto target = bucket::internal::make(
        v1::collection::internal::make(coll2, nullptr), v1::collection::internal::make(coll2, nullptr), "target", 456);

    REQUIRE(v1::collection::internal::as_mongoc(bucket::internal::files(source)) == coll1);
    REQUIRE(v1::collection::internal::as_mongoc(bucket::internal::files(target)) == coll2);

    REQUIRE(source.bucket_name() == "source");
    REQUIRE(target.bucket_name() == "target");

    REQUIRE(bucket::internal::default_chunk_size(source) == 123);
    REQUIRE(bucket::internal::default_chunk_size(target) == 456);

    SECTION("move") {
        {
            auto move = std::move(source);

            CHECK_FALSE(source);
            REQUIRE(move);
            CHECK(v1::collection::internal::as_mongoc(bucket::internal::files(move)) == coll1);
            CHECK(move.bucket_name() == "source");
            CHECK(bucket::internal::default_chunk_size(move) == 123);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 0);

            target = std::move(move);

            CHECK_FALSE(move);
            REQUIRE(target);
            CHECK(v1::collection::internal::as_mongoc(bucket::internal::files(target)) == coll1);
            CHECK(target.bucket_name() == "source");
            CHECK(bucket::internal::default_chunk_size(target) == 123);
            CHECK(destroy_count == 2);
            CHECK(copy_count == 0);
        }

        CHECK(destroy_count == 2);
    }

    SECTION("copy") {
        {
            auto copy = source;

            REQUIRE(source);

            CHECK(v1::collection::internal::as_mongoc(bucket::internal::files(source)) == coll1);
            CHECK(source.bucket_name() == "source");
            CHECK(bucket::internal::default_chunk_size(source) == 123);

            REQUIRE(copy);

            CHECK(v1::collection::internal::as_mongoc(bucket::internal::files(copy)) == coll2);
            CHECK(copy.bucket_name() == "source");
            CHECK(bucket::internal::default_chunk_size(copy) == 123);

            CHECK(destroy_count == 0);
            CHECK(copy_count == 2);

            target = copy;

            REQUIRE(copy);
            CHECK(v1::collection::internal::as_mongoc(bucket::internal::files(copy)) == coll2);
            CHECK(copy.bucket_name() == "source");
            CHECK(bucket::internal::default_chunk_size(copy) == 123);

            REQUIRE(target);
            CHECK(v1::collection::internal::as_mongoc(bucket::internal::files(target)) == coll3);
            CHECK(target.bucket_name() == "source");
            CHECK(bucket::internal::default_chunk_size(target) == 123);

            CHECK(destroy_count == 2);
            CHECK(copy_count == 4);
        }

        CHECK(destroy_count == 4);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][gridfs][bucket][options]") {
    bucket::options source;
    bucket::options target;

    source.bucket_name("source");
    target.bucket_name("target");

    REQUIRE(source.bucket_name() == "source");
    REQUIRE(target.bucket_name() == "target");

    auto const source_value = source.bucket_name();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.bucket_name() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.bucket_name() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.bucket_name() == source_value);
        CHECK(copy.bucket_name() == source_value);

        target = copy;

        CHECK(copy.bucket_name() == source_value);
        CHECK(target.bucket_name() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][gridfs][bucket][options]") {
    bucket::options const opts;

    CHECK_FALSE(opts.bucket_name().has_value());
    CHECK_FALSE(opts.chunk_size_bytes().has_value());
    CHECK_FALSE(opts.read_concern().has_value());
    CHECK_FALSE(opts.read_preference().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("bucket_name", "[mongocxx][v1][gridfs][bucket][options]") {
    auto const v = GENERATE(as<std::string>(), "", "abc");

    CHECK(bucket::options{}.bucket_name(v).bucket_name() == v);
}

TEST_CASE("chunk_size_bytes", "[mongocxx][v1][gridfs][bucket][options]") {
    auto const v = GENERATE(as<std::int32_t>(), INT32_MIN, -1, 0, 1, INT32_MAX);

    CHECK(bucket::options{}.chunk_size_bytes(v).chunk_size_bytes() == v);
}

TEST_CASE("read_concern", "[mongocxx][v1][gridfs][bucket][options]") {
    using T = v1::read_concern;

    auto const v = GENERATE(T{}, std::move(T{}.acknowledge_level(T::level::k_majority)));

    CHECK(bucket::options{}.read_concern(v).read_concern() == v);
}

TEST_CASE("read_preference", "[mongocxx][v1][gridfs][bucket][options]") {
    using T = mongocxx::v1::read_preference;

    auto const v = GENERATE(values({
        T{},
        T{}.mode(T::read_mode::k_secondary),
        T{}.tags(scoped_bson{R"([1, 2.0, "3"])"}.array_view()),
    }));

    CHECK(bucket::options{}.read_preference(v).read_preference() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][gridfs][bucket][options]") {
    using T = v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    CHECK(bucket::options{}.write_concern(v).write_concern() == v);
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
