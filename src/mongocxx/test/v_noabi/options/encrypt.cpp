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

#include <mongocxx/options/encrypt.hpp>

//

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <cstdint>
#include <string>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/options/range.hpp>
#include <mongocxx/options/text.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][encrypt]") {
    using bsoncxx::v_noabi::from_v1;
    using mongocxx::v_noabi::from_v1;

    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> key_id;
    bsoncxx::v1::stdx::optional<std::string> key_alt_name;
    bsoncxx::v1::stdx::optional<v1::encrypt_options::encryption_algorithm> algorithm;
    bsoncxx::v1::stdx::optional<std::int64_t> contention_factor;
    bsoncxx::v1::stdx::optional<v1::encrypt_options::encryption_query_type> query_type;
    bsoncxx::v1::stdx::optional<v1::range_options> range_opts;
    bsoncxx::v1::stdx::optional<v1::text_options> text_opts;

    if (has_value) {
        key_id.emplace();
        key_alt_name.emplace();
        algorithm.emplace();
        contention_factor.emplace();
        query_type.emplace();
        range_opts.emplace();
        text_opts.emplace();
    }

    using v_noabi = v_noabi::options::encrypt;
    using v1 = v1::encrypt_options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.key_id(*key_id);
            from.key_alt_name(*key_alt_name);
            from.algorithm(*algorithm);
            from.contention_factor(*contention_factor);
            from.query_type(*query_type);
            from.range_opts(*range_opts);
            from.text_opts(*text_opts);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.key_id() == *key_id);
            CHECK(to.key_alt_name() == *key_alt_name);
            CHECK(to.algorithm() == *algorithm);
            CHECK(to.contention_factor() == *contention_factor);
            CHECK(to.query_type() == *query_type);
            CHECK(to.range_opts().has_value());
            CHECK(to.text_opts().has_value());
        } else {
            CHECK_FALSE(to.key_id().has_value());
            CHECK_FALSE(to.key_alt_name().has_value());
            CHECK_FALSE(to.algorithm().has_value());
            CHECK_FALSE(to.contention_factor().has_value());
            CHECK_FALSE(to.query_type().has_value());
            CHECK_FALSE(to.range_opts().has_value());
            CHECK_FALSE(to.text_opts().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.key_id(from_v1(key_id->view()));
            from.key_alt_name(*key_alt_name);
            from.algorithm(*algorithm);
            from.contention_factor(*contention_factor);
            from.query_type(*query_type);
            from.range_opts(from_v1(*range_opts));
            from.text_opts(*text_opts);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.key_id() == *key_id);
            CHECK(to.key_alt_name() == *key_alt_name);
            CHECK(to.algorithm() == *algorithm);
            CHECK(to.contention_factor() == *contention_factor);
            CHECK(to.query_type() == *query_type);
            CHECK(to.range_opts().has_value());
            CHECK(to.text_opts().has_value());
        } else {
            CHECK_FALSE(to.key_id().has_value());
            CHECK_FALSE(to.key_alt_name().has_value());
            CHECK_FALSE(to.algorithm().has_value());
            CHECK_FALSE(to.contention_factor().has_value());
            CHECK_FALSE(to.query_type().has_value());
            CHECK_FALSE(to.range_opts().has_value());
            CHECK_FALSE(to.text_opts().has_value());
        }
    }
}

} // namespace mongocxx
