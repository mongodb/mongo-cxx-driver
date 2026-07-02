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

#include <mongocxx/options/data_key.hpp>

//

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <cstdint>
#include <string>
#include <vector>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][data_key]") {
    using bsoncxx::v_noabi::from_v1;

    auto const key_alt_names = GENERATE(values<std::vector<std::string>>({{}, {"abc", "def"}}));
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> master_key;
    bsoncxx::v1::stdx::optional<std::vector<std::uint8_t>> key_material;

    if (has_value) {
        master_key.emplace();
        key_material.emplace();
    }

    using v_noabi = v_noabi::options::data_key;
    using v1 = v1::data_key_options;

    SECTION("from_v1") {
        v1 from;

        from.key_alt_names(key_alt_names);

        if (has_value) {
            from.master_key(*master_key);
            from.key_material(*key_material);
        }

        v_noabi to_raw{from};
        auto const& to = to_raw;

        CHECK(to.key_alt_names() == key_alt_names);

        if (has_value) {
            CHECK(to.master_key() == master_key->view());
            CHECK(to_raw.key_material() == *key_material);
        } else {
            CHECK_FALSE(to.master_key().has_value());
            CHECK_FALSE(to_raw.key_material().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        from.key_alt_names(key_alt_names);

        if (has_value) {
            from.master_key(from_v1(master_key->view()));
            from.key_material(*key_material);
        }

        v1 const to{from};

        CHECK(to.key_alt_names() == key_alt_names);

        if (has_value) {
            CHECK(to.master_key() == master_key->view());
            CHECK(to.key_material() == *key_material);
        } else {
            CHECK_FALSE(to.master_key().has_value());
            CHECK_FALSE(to.key_material().has_value());
        }
    }
}

} // namespace mongocxx
