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

#include <mongocxx/v1/gridfs/uploader.hh>

//

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/gridfs/upload_result.hpp> // IWYU pragma: keep

#include <mongocxx/v1/collection.hh>

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <cstdint>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {
namespace gridfs {

using code = uploader::errc;

TEST_CASE("error code", "[mongocxx][v1][gridfs][uploader][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = uploader::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::gridfs::uploader"));

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
        std::error_code const ec = code::is_closed;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::is_closed) == source_errc::mongocxx);
        CHECK(make_error_code(code::too_many_chunks) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::is_closed) == type_errc::runtime_error);
        CHECK(make_error_code(code::too_many_chunks) == type_errc::runtime_error);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][gridfs][uploader]") {
    struct identity_type {};

    identity_type files_identity;
    auto const chunks_id = reinterpret_cast<mongoc_collection_t*>(&files_identity);

    auto collection_destroy = libmongoc::collection_destroy.create_instance();
    auto collection_create_bulk_operation_with_opts =
        libmongoc::collection_create_bulk_operation_with_opts.create_instance();
    auto bulk_operation_remove_many_with_opts = libmongoc::bulk_operation_remove_many_with_opts.create_instance();
    auto bulk_operation_execute = libmongoc::bulk_operation_execute.create_instance();

    int destroy_count = 0;
    collection_destroy
        ->interpose([&](mongoc_collection_t* ptr) -> void {
            if (ptr) {
                if (ptr != chunks_id) {
                    FAIL("unexpected mongoc_collection_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    collection_create_bulk_operation_with_opts
        ->interpose([&](mongoc_collection_t* coll, bson_t const* opts) -> mongoc_bulk_operation_t* {
            if (coll != chunks_id) {
                FAIL("unexpected mongoc_collection_t");
            }
            CHECK(opts != nullptr);
            return nullptr;
        })
        .forever();

    bulk_operation_remove_many_with_opts
        ->interpose(
            [&](mongoc_bulk_operation_t* bulk, bson_t const* selector, bson_t const* opts, bson_error_t* error)
                -> bool {
                CHECK(bulk == nullptr);
                CHECK(selector != nullptr);
                CHECK(opts != nullptr);
                CHECK(error != nullptr);
                return true;
            })
        .forever();

    bulk_operation_execute
        ->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply, bson_error_t* error) -> std::uint32_t {
            CHECK(bulk == nullptr);
            CHECK(reply != nullptr);
            CHECK(error != nullptr);
            return 1u;
        })
        .forever();

    auto source = uploader::internal::make(
        v1::collection{},
        v1::collection::internal::make(chunks_id, nullptr),
        nullptr,
        "source",
        bsoncxx::v1::types::value{},
        123,
        bsoncxx::v1::stdx::nullopt);

    auto target = uploader::internal::make(
        v1::collection{},
        v1::collection::internal::make(chunks_id, nullptr),
        nullptr,
        "target",
        bsoncxx::v1::types::value{},
        456,
        bsoncxx::v1::stdx::nullopt);

    source.abort();

    REQUIRE_FALSE(source.is_open());
    REQUIRE(target.is_open());

    REQUIRE(source.chunk_size() == 123);
    REQUIRE(target.chunk_size() == 456);

    SECTION("move") {
        auto move = std::move(source);

        CHECK_FALSE(source);
        REQUIRE(move);
        CHECK_FALSE(move.is_open());
        CHECK(move.chunk_size() == 123);

        target = std::move(move);

        CHECK_FALSE(move);
        REQUIRE(target);
        CHECK_FALSE(target.is_open());
        CHECK(target.chunk_size() == 123);
    }
}

TEST_CASE("default", "[mongocxx][v1][gridfs][uploader]") {
    uploader const v;

    CHECK_FALSE(v);
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
