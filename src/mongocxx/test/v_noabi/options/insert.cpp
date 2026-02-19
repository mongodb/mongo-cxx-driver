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

#include <mongocxx/options/insert.hpp>

//

#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/write_concern.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace mongocxx;

TEST_CASE("insert opts", "[insert][option]") {
    options::insert ins;

    CHECK_OPTIONAL_ARGUMENT(ins, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(ins, write_concern, write_concern{});
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][insert]") {
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<v1::write_concern> write_concern;
    bsoncxx::v1::stdx::optional<bool> bypass_document_validation;
    bsoncxx::v1::stdx::optional<bool> ordered;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;

    if (has_value) {
        write_concern.emplace();
        bypass_document_validation.emplace();
        ordered.emplace();
        comment.emplace();
    }

    using v_noabi = v_noabi::options::insert;

    SECTION("from_v1") {
        SECTION("insert_many") {
            using v1 = v1::insert_many_options;

            v1 from;

            if (has_value) {
                from.write_concern(*write_concern);
                from.bypass_document_validation(*bypass_document_validation);
                from.ordered(*ordered);
                from.comment(*comment);
            }

            v_noabi const to{from};

            if (has_value) {
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.ordered() == *ordered);
                CHECK(to.comment() == *comment);
            } else {
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.ordered().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }

        SECTION("insert_one") {
            using v1 = v1::insert_one_options;

            v1 from;

            if (has_value) {
                from.write_concern(*write_concern);
                from.bypass_document_validation(*bypass_document_validation);
                from.comment(*comment);
            }

            v_noabi const to{from};

            if (has_value) {
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.comment() == *comment);
            } else {
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }
    }

    SECTION("to_v1") {
        SECTION("insert_many") {
            using v1 = v1::insert_many_options;

            v_noabi from;

            if (has_value) {
                from.write_concern(*write_concern);
                from.bypass_document_validation(*bypass_document_validation);
                from.ordered(*ordered);
                from.comment(from_v1(comment->view()));
            }

            v1 const to{from};

            if (has_value) {
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.ordered() == *ordered);
                CHECK(to.comment() == *comment);
            } else {
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.ordered().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }

        SECTION("insert_one") {
            using v1 = v1::insert_one_options;

            v_noabi from;

            if (has_value) {
                from.write_concern(*write_concern);
                from.bypass_document_validation(*bypass_document_validation);
                from.comment(from_v1(comment->view()));
            }

            v1 const to{from};

            if (has_value) {
                CHECK(to.write_concern() == *write_concern);
                CHECK(to.bypass_document_validation() == *bypass_document_validation);
                CHECK(to.comment() == *comment);
            } else {
                CHECK_FALSE(to.write_concern().has_value());
                CHECK_FALSE(to.bypass_document_validation().has_value());
                CHECK_FALSE(to.comment().has_value());
            }
        }
    }
}

} // namespace mongocxx
