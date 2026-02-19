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

#include <mongocxx/options/change_stream.hpp>

//

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <chrono>
#include <cstdint>
#include <string>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][change_streams]") {
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<std::string> full_document;
    bsoncxx::v1::stdx::optional<std::string> full_document_before_change;
    bsoncxx::v1::stdx::optional<std::int32_t> batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> comment;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> resume_after;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> start_after;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_await_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp> start_at_operation_time;

    if (has_value) {
        full_document.emplace();
        full_document_before_change.emplace();
        batch_size.emplace();
        comment.emplace();
        collation.emplace();
        resume_after.emplace();
        start_after.emplace();
        max_await_time.emplace();
        start_at_operation_time.emplace();
    }

    using v_noabi = v_noabi::options::change_stream;
    using v1 = v1::change_stream::options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.full_document(*full_document);
            from.full_document_before_change(*full_document_before_change);
            from.batch_size(*batch_size);
            from.comment(*comment);
            from.collation(*collation);
            from.resume_after(*resume_after);
            from.start_after(*start_after);
            from.max_await_time(*max_await_time);
            from.start_at_operation_time(*start_at_operation_time);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.full_document() == *full_document);
            CHECK(to.full_document_before_change() == *full_document_before_change);
            CHECK(to.batch_size() == *batch_size);
            CHECK(to.comment().value() == *comment);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.resume_after() == resume_after->view());
            CHECK(to.start_after() == start_after->view());
            CHECK(to.max_await_time() == *max_await_time);
            CHECK(to.start_at_operation_time() == from_v1(*start_at_operation_time));
        } else {
            CHECK_FALSE(to.full_document().has_value());
            CHECK_FALSE(to.full_document_before_change().has_value());
            CHECK_FALSE(to.batch_size().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.resume_after().has_value());
            CHECK_FALSE(to.start_after().has_value());
            CHECK_FALSE(to.max_await_time().has_value());
            CHECK_FALSE(to.start_at_operation_time().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.full_document(*full_document);
            from.full_document_before_change(*full_document_before_change);
            from.batch_size(*batch_size);
            from.comment(from_v1(comment->view()));
            from.collation(from_v1(collation->view()));
            from.resume_after(from_v1(resume_after->view()));
            from.start_after(from_v1(start_after->view()));
            from.max_await_time(*max_await_time);
            from.start_at_operation_time(from_v1(*start_at_operation_time));
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.full_document() == *full_document);
            CHECK(to.full_document_before_change() == *full_document_before_change);
            CHECK(to.batch_size() == *batch_size);
            CHECK(to.comment().value() == *comment);
            CHECK(to.collation().value() == collation->view());
            CHECK(to.resume_after() == resume_after->view());
            CHECK(to.start_after() == start_after->view());
            CHECK(to.max_await_time() == *max_await_time);
            CHECK(to.start_at_operation_time() == *start_at_operation_time);
        } else {
            CHECK_FALSE(to.full_document().has_value());
            CHECK_FALSE(to.full_document_before_change().has_value());
            CHECK_FALSE(to.batch_size().has_value());
            CHECK_FALSE(to.comment().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.resume_after().has_value());
            CHECK_FALSE(to.start_after().has_value());
            CHECK_FALSE(to.max_await_time().has_value());
            CHECK_FALSE(to.start_at_operation_time().has_value());
        }
    }
}

} // namespace mongocxx
