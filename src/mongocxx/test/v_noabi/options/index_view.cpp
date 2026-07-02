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

#include <mongocxx/options/index_view.hpp>

//

#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <chrono>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

namespace {

template <typename T>
void test_create_options() {
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> max_time;
    bsoncxx::v1::stdx::optional<v1::write_concern> write_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> commit_quorum;

    if (has_value) {
        max_time.emplace();
        write_concern.emplace();
        commit_quorum.emplace();
    }

    using bsoncxx::v_noabi::from_v1;
    using mongocxx::v_noabi::from_v1;
    using mongocxx::v_noabi::to_v1;

    using v1 = T;
    using v_noabi = v_noabi::options::index_view;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.max_time(*max_time);
            from.write_concern(*write_concern);
            from.commit_quorum(*commit_quorum);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.max_time() == max_time);
            CHECK(to.write_concern() == write_concern);
            CHECK(to.commit_quorum().value().view() == commit_quorum->view());
        } else {
            CHECK_FALSE(to.max_time().has_value());
            CHECK_FALSE(to.write_concern().has_value());
            CHECK_FALSE(to.commit_quorum().has_value());
        }
    }
}

} // namespace

TEST_CASE("v1", "[mongocxx][v_noabi][options][index_view]") {
    SECTION("create_many_options") {
        test_create_options<v1::indexes::create_many_options>();
    }

    SECTION("create_one_options") {
        test_create_options<v1::indexes::create_one_options>();
    }
}

} // namespace mongocxx
