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

#include <mongocxx/options/client_session.hpp>

//

#include <mongocxx/v1/transaction_options.hpp>

#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/options/transaction.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace mongocxx;

TEST_CASE("session option", "[session][option]") {
    options::client_session opts;

    // Causal consistency is enabled by default.
    REQUIRE(opts.causal_consistency());
    opts.causal_consistency(false);
    REQUIRE(!opts.causal_consistency());
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][client_session]") {
    using bsoncxx::v_noabi::from_v1;
    using mongocxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> causal_consistency;
    bsoncxx::v1::stdx::optional<bool> snapshot;
    bsoncxx::v1::stdx::optional<v1::transaction_options> default_transaction_opts;

    if (has_value) {
        causal_consistency.emplace();
        snapshot.emplace();
        default_transaction_opts.emplace();
    }

    using v_noabi = v_noabi::options::client_session;
    using v1 = v1::client_session::options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.causal_consistency(*causal_consistency);
            from.snapshot(*snapshot);
            from.default_transaction_opts(*default_transaction_opts);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.causal_consistency() == *causal_consistency);
            CHECK(to.snapshot() == *snapshot);
            CHECK(to.default_transaction_opts().has_value());
        } else {
            CHECK(to.causal_consistency() == true);
            CHECK(to.snapshot() == false);
            CHECK(to.default_transaction_opts().has_value()); // Never null?
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.causal_consistency(*causal_consistency);
            from.snapshot(*snapshot);
            from.default_transaction_opts(from_v1(*default_transaction_opts));
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.causal_consistency() == *causal_consistency);
            CHECK(to.snapshot() == *snapshot);
            CHECK(to.default_transaction_opts().has_value());
        } else {
            CHECK(to.causal_consistency() == true);
            CHECK(to.snapshot() == false);
            CHECK(to.default_transaction_opts().has_value()); // Never null?
        }
    }
}

} // namespace mongocxx
