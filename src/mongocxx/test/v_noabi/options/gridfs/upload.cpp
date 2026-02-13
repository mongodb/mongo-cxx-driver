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

#include <mongocxx/options/gridfs/upload.hpp>

//

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <cstdint>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace bsoncxx;
using namespace mongocxx;

using bsoncxx::builder::basic::kvp;

TEST_CASE("options::gridfs::upload accessors/mutators", "[options::gridfs::upload]") {
    options::gridfs::upload upload_options;

    auto document = builder::basic::make_document(kvp("foo", 1));

    CHECK_OPTIONAL_ARGUMENT(upload_options, chunk_size_bytes, 100);
    CHECK_OPTIONAL_ARGUMENT(upload_options, metadata, document.view());
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][gridfs][upload]") {
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<std::int32_t> chunk_size_bytes;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> metadata;

    if (has_value) {
        chunk_size_bytes.emplace();
        metadata.emplace();
    }

    using bsoncxx::v_noabi::from_v1;

    using v_noabi = v_noabi::options::gridfs::upload;
    using v1 = v1::gridfs::upload_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.chunk_size_bytes(*chunk_size_bytes);
            from.metadata(*metadata);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.chunk_size_bytes() == chunk_size_bytes);
            CHECK(to.metadata() == metadata->view());
        } else {
            CHECK_FALSE(to.chunk_size_bytes().has_value());
            CHECK_FALSE(to.metadata().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.chunk_size_bytes(*chunk_size_bytes);
            from.metadata(from_v1(*metadata));
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.chunk_size_bytes() == chunk_size_bytes);
            CHECK(to.metadata() == metadata->view());
        } else {
            CHECK_FALSE(to.chunk_size_bytes().has_value());
            CHECK_FALSE(to.metadata().has_value());
        }
    }
}

} // namespace mongocxx
