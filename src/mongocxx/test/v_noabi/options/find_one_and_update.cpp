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

#include <mongocxx/options/find_one_and_update.hpp>

//

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/return_document.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <chrono>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/options/find_one_common_options.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("find_one_and_update", "[find_one_and_update][option]") {
    options::find_one_and_update opts{};

    auto collation = make_document(kvp("locale", "en_US"));
    std::chrono::milliseconds ms{400};
    auto projection = make_document(kvp("_id", false));
    auto sort = make_document(kvp("x", -1));
    auto array_filters = make_array("a", "b");

    CHECK_OPTIONAL_ARGUMENT(opts, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(opts, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(opts, max_time, ms);
    CHECK_OPTIONAL_ARGUMENT(opts, projection, projection.view());
    CHECK_OPTIONAL_ARGUMENT(opts, return_document, options::return_document::k_before);
    CHECK_OPTIONAL_ARGUMENT(opts, sort, sort.view());
    CHECK_OPTIONAL_ARGUMENT(opts, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(opts, array_filters, array_filters.view());
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][find_one_and_update]") {
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> bypass_document_validation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> projection;
    bsoncxx::v1::stdx::optional<v1::return_document> return_document;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> ordering;
    bsoncxx::v1::stdx::optional<bool> upsert;
    bsoncxx::v1::stdx::optional<v1::write_concern> write_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> array_filters;

    if (has_value) {
        bypass_document_validation.emplace();
        collation.emplace();
        hint.emplace("hint");
        let.emplace();
        comment.emplace();
        max_time.emplace();
        projection.emplace();
        return_document.emplace();
        ordering.emplace();
        upsert.emplace();
        write_concern.emplace();
        array_filters.emplace();
    }

    using v_noabi = v_noabi::options::find_one_and_update;
    using v1 = v1::find_one_and_update_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.bypass_document_validation(*bypass_document_validation);
            from.collation(*collation);
            from.hint(*hint);
            from.let(*let);
            from.comment(*comment);
            from.max_time(*max_time);
            from.projection(*projection);
            from.return_document(*return_document);
            from.sort(*ordering);
            from.upsert(*upsert);
            from.write_concern(*write_concern);
            from.array_filters(*array_filters);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.bypass_document_validation() == *bypass_document_validation);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.let().value() == let->view());
            CHECK(to.comment().value() == *comment);
            CHECK(to.max_time() == *max_time);
            CHECK(to.projection().value() == projection->view());
            CHECK(to.return_document() == *return_document);
            CHECK(to.sort().value() == ordering->view());
            CHECK(to.upsert() == *upsert);
            CHECK(to.write_concern() == *write_concern);
            CHECK(to.array_filters().value() == array_filters->view());
        } else {
            CHECK_FALSE(to.bypass_document_validation().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.projection().has_value());
            CHECK_FALSE(to.return_document().has_value());
            CHECK_FALSE(to.sort().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.array_filters().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.bypass_document_validation(*bypass_document_validation);
            from.collation(from_v1(collation->view()));
            from.hint(*hint);
            from.let(from_v1(let->view()));
            from.comment(from_v1(comment->view()));
            from.max_time(*max_time);
            from.projection(from_v1(projection->view()));
            from.return_document(*return_document);
            from.sort(from_v1(ordering->view()));
            from.upsert(*upsert);
            from.write_concern(*write_concern);
            from.array_filters(from_v1(array_filters->view()));
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.bypass_document_validation() == *bypass_document_validation);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.let().value() == let->view());
            CHECK(to.comment().value() == *comment);
            CHECK(to.max_time() == *max_time);
            CHECK(to.projection().value() == projection->view());
            CHECK(to.return_document() == *return_document);
            CHECK(to.sort().value() == ordering->view());
            CHECK(to.upsert() == *upsert);
            CHECK(to.write_concern() == *write_concern);
            CHECK(to.array_filters().value() == array_filters->view());
        } else {
            CHECK_FALSE(to.bypass_document_validation().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.projection().has_value());
            CHECK_FALSE(to.return_document().has_value());
            CHECK_FALSE(to.sort().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.array_filters().has_value());
        }
    }
}

} // namespace mongocxx
