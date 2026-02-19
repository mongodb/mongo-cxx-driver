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

#include <mongocxx/options/bulk_write.hpp>

//

#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/write_concern.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace mongocxx;

TEST_CASE("bulk_write opts", "[bulk_write][options]") {
    options::bulk_write bulk_write_opts;

    REQUIRE(bulk_write_opts.ordered());
    CHECK_OPTIONAL_ARGUMENT(bulk_write_opts, write_concern, write_concern{});
    CHECK_OPTIONAL_ARGUMENT(bulk_write_opts, bypass_document_validation, true);
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][bulk_write]") {
    using bsoncxx::v_noabi::from_v1;

    auto const ordered = GENERATE(false, true);
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<v1::write_concern> write_concern;
    bsoncxx::v1::stdx::optional<bool> bypass_document_validation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;

    if (has_value) {
        write_concern.emplace();
        bypass_document_validation.emplace();
        let.emplace();
        comment.emplace();
    }

    using v_noabi = v_noabi::options::bulk_write;
    using v1 = v1::bulk_write::options;

    SECTION("from_v1") {
        v1 from;

        from.ordered(ordered);

        if (has_value) {
            from.write_concern(*write_concern);
            from.bypass_document_validation(*bypass_document_validation);
            from.let(*let);
            from.comment(*comment);
        }

        v_noabi const to{from};

        CHECK(to.ordered() == ordered);

        if (has_value) {
            CHECK(to.write_concern() == *write_concern);
            CHECK(to.bypass_document_validation() == *bypass_document_validation);
            CHECK(to.let().value() == let->view());
            CHECK(to.comment().value() == *comment);
        } else {
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.bypass_document_validation().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.comment().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        from.ordered(ordered);

        if (has_value) {
            from.write_concern(*write_concern);
            from.bypass_document_validation(*bypass_document_validation);
            from.let(from_v1(let->view()));
            from.comment(from_v1(comment->view()));
        }

        v1 const to{from};

        CHECK(to.ordered() == ordered);

        if (has_value) {
            CHECK(to.write_concern() == *write_concern);
            CHECK(to.bypass_document_validation() == *bypass_document_validation);
            CHECK(to.let().value() == let->view());
            CHECK(to.comment().value() == *comment);
        } else {
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.bypass_document_validation().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.comment().has_value());
        }
    }
}

} // namespace mongocxx
