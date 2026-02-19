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

#include <mongocxx/v1/gridfs/upload_options.hh>

//

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {
namespace gridfs {

TEST_CASE("ownership", "[mongocxx][v1][gridfs][upload_options]") {
    auto const source_value = 1;
    auto const target_value = 2;

    upload_options source;
    upload_options target;

    source.chunk_size_bytes(source_value);
    target.chunk_size_bytes(target_value);

    REQUIRE(source.chunk_size_bytes() == source_value);
    REQUIRE(target.chunk_size_bytes() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.chunk_size_bytes() == source_value);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.chunk_size_bytes() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.chunk_size_bytes() == source_value);
        CHECK(copy.chunk_size_bytes() == source_value);

        target = copy;

        CHECK(copy.chunk_size_bytes() == source_value);
        CHECK(target.chunk_size_bytes() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][gridfs][upload_options]") {
    upload_options const opts;

    CHECK_FALSE(opts.chunk_size_bytes().has_value());
    CHECK_FALSE(opts.metadata().has_value());
}

TEST_CASE("chunk_size_bytes", "[mongocxx][v1][gridfs][upload_options]") {
    auto const v = GENERATE(
        values<std::int32_t>({
            INT32_MIN,
            -1,
            0,
            1,
            INT32_MAX,
        }));

    CHECK(upload_options{}.chunk_size_bytes(v).chunk_size_bytes() == v);
}

TEST_CASE("metadata", "[mongocxx][v1][gridfs][upload_options]") {
    auto const v = GENERATE(as<scoped_bson>(), R"({})", R"({"x": 1})").value();

    CHECK(upload_options{}.metadata(v).metadata() == v);
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
