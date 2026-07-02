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

#include <mongocxx/options/server_api.hpp>

//

#include <bsoncxx/test/v1/stdx/optional.hh>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][server_api]") {
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> strict;
    bsoncxx::v1::stdx::optional<bool> deprecation_errors;

    if (has_value) {
        strict.emplace();
        deprecation_errors.emplace();
    }

    using v_noabi = v_noabi::options::server_api;
    using v1 = v1::server_api;

    SECTION("from_v1") {
        v1 from{v1::version::k_version_1};

        if (has_value) {
            from.strict(*strict);
            from.deprecation_errors(*deprecation_errors);
        }

        v_noabi const to{from};

        CHECK(to.get_version() == v_noabi::version::k_version_1);

        if (has_value) {
            CHECK(to.strict() == *strict);
            CHECK(to.deprecation_errors() == *deprecation_errors);
        } else {
            CHECK_FALSE(to.strict().has_value());
            CHECK_FALSE(to.deprecation_errors().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from{v_noabi::version::k_version_1};

        if (has_value) {
            from.strict(*strict);
            from.deprecation_errors(*deprecation_errors);
        }

        v1 const to{from};

        CHECK(to.get_version() == v1::version::k_version_1);

        if (has_value) {
            CHECK(to.strict() == *strict);
            CHECK(to.deprecation_errors() == *deprecation_errors);
        } else {
            CHECK_FALSE(to.strict().has_value());
            CHECK_FALSE(to.deprecation_errors().has_value());
        }
    }
}

} // namespace mongocxx
