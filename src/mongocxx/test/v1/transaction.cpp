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

#include <mongocxx/v1/transaction.hpp>

//

#include <mongocxx/test/v1/read_concern.hh>
#include <mongocxx/test/v1/read_preference.hh>
#include <mongocxx/test/v1/write_concern.hh>

#include <chrono>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][transaction]") {
    using secs = std::chrono::seconds;

    transaction source;
    transaction target;

    auto const source_secs = secs{123};
    auto const target_secs = secs{456};

    source.max_commit_time_ms(source_secs);
    target.max_commit_time_ms(target_secs);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.max_commit_time_ms() == source_secs);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.max_commit_time_ms() == source_secs);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.max_commit_time_ms() == source_secs);
        CHECK(copy.max_commit_time_ms() == source_secs);

        target = copy;

        CHECK(copy.max_commit_time_ms() == source_secs);
        CHECK(target.max_commit_time_ms() == source_secs);
    }
}

TEST_CASE("default", "[mongocxx][v1][transaction]") {
    transaction const txn;

    CHECK_FALSE(txn.max_commit_time_ms().has_value());
    CHECK_FALSE(txn.read_concern().has_value());
    CHECK_FALSE(txn.read_preference().has_value());
    CHECK_FALSE(txn.write_concern().has_value());
}

TEST_CASE("max_commit_time_ms", "[mongocxx][v1][transaction]") {
    using msecs = std::chrono::milliseconds;

    auto const v = GENERATE(values({
        msecs::min(),
        msecs{-2},
        msecs{-1},
        msecs{0},
        msecs{1},
        msecs::max(),
    }));

    CAPTURE(v);

    transaction txn;
    CHECK_NOTHROW(txn.max_commit_time_ms(v));

    // DEFAULT_MAX_COMMIT_TIME_MS (0) is equivalent to "unset".
    CHECKED_IF(v.count() == 0) {
        CHECK_FALSE(txn.max_commit_time_ms().has_value());
    }

    else {
        CHECK(txn.max_commit_time_ms() == v);
    }
}

TEST_CASE("read_concern", "[mongocxx][v1][transaction]") {
    auto const v = GENERATE(values({
        read_concern{},
        std::move(read_concern{}.acknowledge_level(read_concern::level::k_majority)),
    }));
    CAPTURE(v.acknowledge_level());

    CHECK(transaction{}.read_concern(v).read_concern() == v);
}

TEST_CASE("read_preference", "[mongocxx][v1][transaction]") {
    auto const v = GENERATE(values({
        read_preference{},
        std::move(read_preference{}.mode(read_preference::read_mode::k_primary_preferred)),
    }));
    CAPTURE(v.mode());

    CHECK(transaction{}.read_preference(v).read_preference() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][transaction]") {
    auto const v = GENERATE(values({
        write_concern{},
        std::move(write_concern{}.acknowledge_level(write_concern::level::k_majority)),
    }));
    CAPTURE(v.acknowledge_level());

    CHECK(transaction{}.write_concern(v).write_concern() == v);
}

} // namespace v1
} // namespace mongocxx
