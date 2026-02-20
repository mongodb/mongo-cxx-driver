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

#include <mongocxx/options/update.hpp>

//

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/write_concern.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("update opts", "[update][option]") {
    options::update updt;

    auto collation = make_document(kvp("locale", "en_US"));

    CHECK_OPTIONAL_ARGUMENT(updt, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(updt, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(updt, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(updt, write_concern, write_concern{});
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][update]") {
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> bypass_document_validation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bool> upsert;
    bsoncxx::v1::stdx::optional<v1::write_concern> write_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> array_filters;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> sort;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;

    if (has_value) {
        bypass_document_validation.emplace(true);
        collation.emplace();
        upsert.emplace(true);
        write_concern.emplace();
        array_filters.emplace();
        hint.emplace("hint");
        let.emplace();
        sort.emplace();
        comment.emplace();
    }

    using v_noabi = v_noabi::options::update;

    SECTION("from_v1") {
        SECTION("update_many") {
            using v1 = v1::update_many_options;

            v1 from;

            if (has_value) {
                from.bypass_document_validation(*bypass_document_validation);
                from.collation(*collation);
                from.upsert(*upsert);
                from.write_concern(*write_concern);
                from.array_filters(*array_filters);
                from.hint(*hint);
                from.let(*let);
                from.comment(*comment);
            }

            v_noabi const to{from};

            if (has_value) {
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.collation().value() == collation->view());
                CHECK(to.upsert() == *upsert);
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.array_filters().value() == array_filters->view());
                CHECK(to.hint().value().to_value() == hint->to_value());
                CHECK(to.let().value() == let->view());
                CHECK(to.comment().value() == *comment);
            } else {
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.collation().has_value());
                CHECK_FALSE(to.upsert().has_value());
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.array_filters().has_value());
                CHECK_FALSE(to.hint().has_value());
                CHECK_FALSE(to.let().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }

        SECTION("update_one") {
            using v1 = v1::update_one_options;

            v1 from;

            if (has_value) {
                from.bypass_document_validation(*bypass_document_validation);
                from.collation(*collation);
                from.upsert(*upsert);
                from.write_concern(*write_concern);
                from.array_filters(*array_filters);
                from.hint(*hint);
                from.let(*let);
                from.sort(*sort);
                from.comment(*comment);
            }

            v_noabi const to{from};

            if (has_value) {
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.collation().value() == collation->view());
                CHECK(to.upsert() == *upsert);
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.array_filters().value() == array_filters->view());
                CHECK(to.hint().value().to_value() == hint->to_value());
                CHECK(to.let().value() == let->view());
                CHECK(to.sort().value() == sort->view());
                CHECK(to.comment().value() == *comment);
            } else {
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.collation().has_value());
                CHECK_FALSE(to.upsert().has_value());
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.array_filters().has_value());
                CHECK_FALSE(to.hint().has_value());
                CHECK_FALSE(to.let().has_value());
                CHECK_FALSE(to.sort().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }
    }

    SECTION("to_v1") {
        SECTION("update_many") {
            using v1 = v1::update_many_options;

            v_noabi from;

            if (has_value) {
                from.bypass_document_validation(*bypass_document_validation);
                from.collation(from_v1(collation->view()));
                from.upsert(*upsert);
                from.write_concern(*write_concern);
                from.array_filters(from_v1(array_filters->view()));
                from.hint(*hint);
                from.let(from_v1(let->view()));
                from.comment(from_v1(comment->view()));
            }

            v1 const to{from};

            if (has_value) {
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.collation().value() == collation->view());
                CHECK(to.upsert() == *upsert);
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.array_filters().value() == array_filters->view());
                CHECK(to.hint().value().to_value() == hint->to_value());
                CHECK(to.let().value() == let->view());
                CHECK(to.comment().value() == *comment);
            } else {
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.collation().has_value());
                CHECK_FALSE(to.upsert().has_value());
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.array_filters().has_value());
                CHECK_FALSE(to.hint().has_value());
                CHECK_FALSE(to.let().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }

        SECTION("update_one") {
            using v1 = v1::update_one_options;

            v_noabi from;

            if (has_value) {
                from.bypass_document_validation(*bypass_document_validation);
                from.collation(from_v1(collation->view()));
                from.upsert(*upsert);
                from.write_concern(*write_concern);
                from.array_filters(from_v1(array_filters->view()));
                from.hint(*hint);
                from.let(from_v1(let->view()));
                from.sort(from_v1(sort->view()));
                from.comment(from_v1(comment->view()));
            }

            v1 const to{from};

            if (has_value) {
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.collation().value() == collation->view());
                CHECK(to.upsert() == *upsert);
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.array_filters().value() == array_filters->view());
                CHECK(to.hint().value().to_value() == hint->to_value());
                CHECK(to.let().value() == let->view());
                CHECK(to.sort().value() == sort->view());
                CHECK(to.comment().value() == *comment);
            } else {
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.collation().has_value());
                CHECK_FALSE(to.upsert().has_value());
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.array_filters().has_value());
                CHECK_FALSE(to.hint().has_value());
                CHECK_FALSE(to.let().has_value());
                CHECK_FALSE(to.sort().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }
    }
}

} // namespace mongocxx
