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

#include <mongocxx/private/scoped_bson_value.hh>

//

#include <bsoncxx/test/v1/types/id.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <cstdint>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/private/bson.hh>

#include <bsoncxx/test/catch.hh>

namespace {

using value = mongocxx::detail::scoped_bson_value;

using namespace bsoncxx::v_noabi::types;

TEST_CASE("basic", "[mongocxx][private][scoped_bson_value]") {
    SECTION("default") {
        value v;
        CHECK(v.get()->value_type == BSON_TYPE_NULL);
    }

    SECTION("b_types") {
        SECTION("int32") {
            value v{b_int32{123}};
            REQUIRE(v.get()->value_type == BSON_TYPE_INT32);
            CHECK(v.get()->value.v_int32 == 123);
        }

        SECTION("string") {
            SECTION("empty") {
                value v{b_string{bsoncxx::stdx::string_view{}}};
                REQUIRE(v.get()->value_type == BSON_TYPE_UTF8);
                auto const& v_utf8 = v.get()->value.v_utf8;
                CHECK(v_utf8.len == 0u);
                CHECK(static_cast<void const*>(v_utf8.str) != nullptr);
                CHECK(v_utf8.str[0] == '\0');
            }

            SECTION("owning") {
                auto const str = b_string{"value"};
                value v{str};
                REQUIRE(v.get()->value_type == BSON_TYPE_UTF8);
                auto const& v_utf8 = v.get()->value.v_utf8;
                CHECK(static_cast<void const*>(v_utf8.str) != static_cast<void const*>(str.value.data()));
                CHECK(bsoncxx::stdx::string_view{v_utf8.str, v_utf8.len} == str.value);
            }
        }

        SECTION("binary") {
            SECTION("empty") {
                value v{b_binary{}};
                REQUIRE(v.get()->value_type == BSON_TYPE_BINARY);
                auto const& v_binary = v.get()->value.v_binary;
                CHECK(v_binary.subtype == BSON_SUBTYPE_BINARY);
                CHECK(v_binary.data_len == 0u);
                CHECK(static_cast<void const*>(v_binary.data) == nullptr);
            }

            SECTION("owning") {
                std::uint8_t data[1]{0x12};
                value v{b_binary{bsoncxx::v_noabi::binary_sub_type::k_binary, sizeof(data), data}};
                REQUIRE(v.get()->value_type == BSON_TYPE_BINARY);
                auto const& v_binary = v.get()->value.v_binary;
                CHECK(v_binary.subtype == BSON_SUBTYPE_BINARY);
                CHECK(v_binary.data_len == 1u);
                CHECK(static_cast<void const*>(v_binary.data) != static_cast<void const*>(data));
                REQUIRE(v_binary.data != nullptr);
                CHECK(v_binary.data[0] == data[0]);
            }
        }
    }

    SECTION("view") {
        SECTION("int32") {
            value v{bson_value::view{b_int32{123}}};
            REQUIRE(v.get()->value_type == BSON_TYPE_INT32);
            CHECK(v.get()->value.v_int32 == 123);
        }

        SECTION("string") {
            auto const str = b_string{"value"};
            value v{bson_value::view{str}};
            REQUIRE(v.get()->value_type == BSON_TYPE_UTF8);
            auto const& v_utf8 = v.get()->value.v_utf8;
            CHECK(static_cast<void const*>(v_utf8.str) != static_cast<void const*>(str.value.data()));
            CHECK(bsoncxx::stdx::string_view{v_utf8.str, v_utf8.len} == str);
        }
    }

    SECTION("ownership") {
        value v{b_string{"one"}};
        {
            bsoncxx::v1::types::value copy{std::move(v)};
            REQUIRE(v.get()->value_type == BSON_TYPE_NULL);
            CHECK(copy.type_id() == bsoncxx::v1::types::id::k_string);
            CHECK(copy.get_string().value == "one");
        }

        v = value{b_string{"two"}};
        v = value{b_string{"three"}};

        {
            bsoncxx::v1::types::value copy{std::move(v)};
            REQUIRE(v.get()->value_type == BSON_TYPE_NULL);
            CHECK(copy.type_id() == bsoncxx::v1::types::id::k_string);
            CHECK(copy.get_string().value == "three");
        }
    }
}

} // namespace
