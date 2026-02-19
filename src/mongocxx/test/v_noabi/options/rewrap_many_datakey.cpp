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

#include <mongocxx/options/rewrap_many_datakey.hpp>

//

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <string>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][rewrap_many_datakey]") {
    using bsoncxx::v_noabi::from_v1;

    auto const provider = GENERATE(as<std::string>(), "abc", "def");
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> master_key;

    if (has_value) {
        master_key.emplace();
    }

    using v_noabi = v_noabi::options::rewrap_many_datakey;
    using v1 = v1::rewrap_many_datakey_options;

    SECTION("from_v1") {
        v1 from;

        from.provider(provider);

        if (has_value) {
            from.master_key(*master_key);
        }

        v_noabi const to{from};

        CHECK(to.provider() == provider);

        if (has_value) {
            CHECK(to.master_key() == master_key->view());
        } else {
            CHECK_FALSE(to.master_key().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        from.provider(provider);

        if (has_value) {
            from.master_key(from_v1(master_key->view()));
        }

        v1 const to{from};

        CHECK(to.provider() == provider);

        if (has_value) {
            CHECK(to.master_key() == master_key->view());
        } else {
            CHECK_FALSE(to.master_key().has_value());
        }
    }
}

} // namespace mongocxx
