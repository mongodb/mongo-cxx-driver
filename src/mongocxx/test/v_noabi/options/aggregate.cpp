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

#include <mongocxx/options/aggregate.hpp>

//

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/read_concern.hpp>
#include <mongocxx/v1/read_preference.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <chrono>
#include <cstdint>

#include <mongocxx/read_concern-fwd.hpp>
#include <mongocxx/read_preference-fwd.hpp>
#include <mongocxx/write_concern-fwd.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {

using namespace bsoncxx::builder::basic;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

TEST_CASE("aggregate", "[aggregate][option]") {
    mongocxx::options::aggregate agg;

    auto const collation = make_document(kvp("locale", "en_US"));
    auto const comment = make_document(kvp("$comment", "some_comment"));
    auto const hint = bsoncxx::document::view_or_value(make_document(kvp("_id", 1)));
    auto const let = make_document(kvp("x", "foo"));

    // Avoid error: use of overloaded operator '==' is ambiguous.
    auto const comment_value = bsoncxx::types::bson_value::view_or_value(comment["$comment"].get_value());

    CHECK_OPTIONAL_ARGUMENT(agg, allow_disk_use, true);
    CHECK_OPTIONAL_ARGUMENT(agg, batch_size, 500);
    CHECK_OPTIONAL_ARGUMENT(agg, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(agg, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(agg, comment, comment_value);
    CHECK_OPTIONAL_ARGUMENT(agg, hint, hint);
    CHECK_OPTIONAL_ARGUMENT(agg, let, let.view());
    CHECK_OPTIONAL_ARGUMENT(agg, max_time, std::chrono::milliseconds{1000});
    CHECK_OPTIONAL_ARGUMENT(agg, read_concern, mongocxx::read_concern());
    CHECK_OPTIONAL_ARGUMENT(agg, read_preference, mongocxx::read_preference());
    CHECK_OPTIONAL_ARGUMENT(agg, write_concern, mongocxx::write_concern());
}

} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][aggregate]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> allow_disk_use;
    bsoncxx::v1::stdx::optional<std::int32_t> batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> let;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_time;
    bsoncxx::v1::stdx::optional<v1::read_preference> read_preference;
    bsoncxx::v1::stdx::optional<bool> bypass_document_validation;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<v1::write_concern> write_concern;
    bsoncxx::v1::stdx::optional<v1::read_concern> read_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;

    if (has_value) {
        allow_disk_use.emplace();
        batch_size.emplace();
        collation.emplace();
        let.emplace();
        max_time.emplace();
        read_preference.emplace();
        bypass_document_validation.emplace();
        hint.emplace("hint");
        write_concern.emplace();
        read_concern.emplace();
        comment.emplace();
    }

    using v_noabi = v_noabi::options::aggregate;
    using v1 = v1::aggregate_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.allow_disk_use(*allow_disk_use);
            from.batch_size(*batch_size);
            from.collation(*collation);
            from.let(*let);
            from.max_time(*max_time);
            from.read_preference(*read_preference);
            from.bypass_document_validation(*bypass_document_validation);
            from.hint(*hint);
            from.write_concern(*write_concern);
            from.read_concern(*read_concern);
            from.comment(*comment);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.allow_disk_use() == *allow_disk_use);
            CHECK(to.batch_size() == *batch_size);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.let().value() == let->view());
            CHECK(to.max_time() == *max_time);
            CHECK(to.read_preference() == *read_preference);
            CHECK(to.bypass_document_validation() == *bypass_document_validation);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.write_concern() == *write_concern);
            CHECK(to.read_concern() == *read_concern);
            CHECK(to.comment() == *comment);
        } else {
            CHECK_FALSE(to.allow_disk_use().has_value());
            CHECK_FALSE(to.batch_size().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.read_preference().has_value());
            CHECK_FALSE(to.bypass_document_validation().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.read_concern().has_value());
            CHECK_FALSE(to.comment().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.allow_disk_use(*allow_disk_use);
            from.batch_size(*batch_size);
            from.collation(from_v1(collation->view()));
            from.let(from_v1(let->view()));
            from.max_time(*max_time);
            from.read_preference(*read_preference);
            from.bypass_document_validation(*bypass_document_validation);
            from.hint(*hint);
            from.write_concern(*write_concern);
            from.read_concern(*read_concern);
            from.comment(from_v1(comment->view()));
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.allow_disk_use() == *allow_disk_use);
            CHECK(to.batch_size() == *batch_size);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.let().value() == let->view());
            CHECK(to.max_time() == *max_time);
            CHECK(to.read_preference() == *read_preference);
            CHECK(to.bypass_document_validation() == *bypass_document_validation);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.write_concern() == *write_concern);
            CHECK(to.read_concern() == *read_concern);
            CHECK(to.comment() == *comment);
        } else {
            CHECK_FALSE(to.allow_disk_use().has_value());
            CHECK_FALSE(to.batch_size().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.let().has_value());
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.read_preference().has_value());
            CHECK_FALSE(to.bypass_document_validation().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.read_concern().has_value());
            CHECK_FALSE(to.comment().has_value());
        }
    }
}

} // namespace mongocxx
