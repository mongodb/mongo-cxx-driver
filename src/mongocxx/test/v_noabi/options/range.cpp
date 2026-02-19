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

#include <mongocxx/options/range.hpp>

//

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <cstdint>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][range]") {
    using bsoncxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> min;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> max;
    bsoncxx::v1::stdx::optional<std::int64_t> sparsity;
    bsoncxx::v1::stdx::optional<std::int32_t> trim_factor;
    bsoncxx::v1::stdx::optional<std::int32_t> precision;

    if (has_value) {
        min.emplace();
        max.emplace();
        sparsity.emplace();
        trim_factor.emplace();
        precision.emplace();
    }

    using v_noabi = v_noabi::options::range;
    using v1 = v1::range_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.min(*min);
            from.max(*max);
            from.sparsity(*sparsity);
            from.trim_factor(*trim_factor);
            from.precision(*precision);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.min() == *min);
            CHECK(to.max() == *max);
            CHECK(to.sparsity() == *sparsity);
            CHECK(to.trim_factor() == *trim_factor);
            CHECK(to.precision() == *precision);
        } else {
            CHECK_FALSE(to.min().has_value());
            CHECK_FALSE(to.max().has_value());
            CHECK_FALSE(to.sparsity().has_value());
            CHECK_FALSE(to.trim_factor().has_value());
            CHECK_FALSE(to.precision().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.min(from_v1(min->view()));
            from.max(from_v1(max->view()));
            from.sparsity(*sparsity);
            from.trim_factor(*trim_factor);
            from.precision(*precision);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.min() == *min);
            CHECK(to.max() == *max);
            CHECK(to.sparsity() == *sparsity);
            CHECK(to.trim_factor() == *trim_factor);
            CHECK(to.precision() == *precision);
        } else {
            CHECK_FALSE(to.min().has_value());
            CHECK_FALSE(to.max().has_value());
            CHECK_FALSE(to.sparsity().has_value());
            CHECK_FALSE(to.trim_factor().has_value());
            CHECK_FALSE(to.precision().has_value());
        }
    }
}

} // namespace mongocxx
