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

#include <bsoncxx/test/v1/types/id.hh>

//

#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <cstdint>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp>

namespace {

using bsoncxx::v1::types::binary_subtype;
using bsoncxx::v1::types::id;

TEST_CASE("to_string", "[bsoncxx][v1][types][id]") {
    SECTION("unknown") {
        CHECK(to_string(static_cast<id>(0)) == "?"); // 0x00 (BSON_TYPE_EOD)
    }

    SECTION("values") {
        // BSONCXX_V1_TYPES_XMACRO: update below.
        CHECK(to_string(id::k_minkey) == "minkey");
        CHECK(to_string(id::k_double) == "double");
        CHECK(to_string(id::k_string) == "string");
        CHECK(to_string(id::k_document) == "document");
        CHECK(to_string(id::k_array) == "array");
        CHECK(to_string(id::k_binary) == "binary");
        CHECK(to_string(id::k_undefined) == "undefined");
        CHECK(to_string(id::k_oid) == "oid");
        CHECK(to_string(id::k_bool) == "bool");
        CHECK(to_string(id::k_date) == "date");
        CHECK(to_string(id::k_null) == "null");
        CHECK(to_string(id::k_regex) == "regex");
        CHECK(to_string(id::k_dbpointer) == "dbpointer");
        CHECK(to_string(id::k_code) == "code");
        CHECK(to_string(id::k_symbol) == "symbol");
        CHECK(to_string(id::k_codewscope) == "codewscope");
        CHECK(to_string(id::k_int32) == "int32");
        CHECK(to_string(id::k_timestamp) == "timestamp");
        CHECK(to_string(id::k_int64) == "int64");
        CHECK(to_string(id::k_decimal128) == "decimal128");
        CHECK(to_string(id::k_maxkey) == "maxkey");
        // BSONCXX_V1_TYPES_XMACRO: update above.
    }
}

TEST_CASE("to_string", "[bsoncxx][v1][types][binary_subtype]") {
    SECTION("unknown") {
        for (int i = 0; i < int{UINT8_MAX} + 1; ++i) {
            CAPTURE(i);

#pragma push_macro("X")
#undef X
#define X(_name, _value)            \
    case binary_subtype::k_##_name: \
        break;

            switch (static_cast<binary_subtype>(i)) {
                // Ignore named enumerators: handled by the "values" section.
                BSONCXX_V1_BINARY_SUBTYPES_XMACRO(X)

                default:
                    // All BSON binary subtype values in the range [0x80, 0xFF] are "user defined".
                    if (i >= static_cast<int>(binary_subtype::k_user)) {
                        CHECK(to_string(static_cast<binary_subtype>(i)) == "user");
                    } else {
                        CHECK(to_string(static_cast<binary_subtype>(i)) == "?");
                    }
            }
#pragma pop_macro("X")
        }
    }

    SECTION("values") {
        // BSONCXX_V1_BINARY_SUBTYPES_XMACRO: update below.
        CHECK(to_string(binary_subtype::k_binary) == "binary");
        CHECK(to_string(binary_subtype::k_function) == "function");
        CHECK(to_string(binary_subtype::k_binary_deprecated) == "binary_deprecated");
        CHECK(to_string(binary_subtype::k_uuid_deprecated) == "uuid_deprecated");
        CHECK(to_string(binary_subtype::k_uuid) == "uuid");
        CHECK(to_string(binary_subtype::k_md5) == "md5");
        CHECK(to_string(binary_subtype::k_encrypted) == "encrypted");
        CHECK(to_string(binary_subtype::k_column) == "column");
        CHECK(to_string(binary_subtype::k_sensitive) == "sensitive");
        CHECK(to_string(binary_subtype::k_vector) == "vector");
        CHECK(to_string(binary_subtype::k_user) == "user");
        // BSONCXX_V1_BINARY_SUBTYPES_XMACRO: update above.
    }
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][types][id]") {
#pragma push_macro("X")
#undef X
#define X(_name, _value) id::k_##_name,
    id values[] = {BSONCXX_V1_TYPES_XMACRO(X)};
#pragma pop_macro("X")

    for (auto const& value : values) {
        CHECK(bsoncxx::test::stringify(value) == "k_" + to_string(value));
    }
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][types][binary_subtype]") {
#pragma push_macro("X")
#undef X
#define X(_name, _value) binary_subtype::k_##_name,
    binary_subtype values[] = {BSONCXX_V1_BINARY_SUBTYPES_XMACRO(X)};
#pragma pop_macro("X")

    for (auto const& value : values) {
        CHECK(bsoncxx::test::stringify(value) == "k_" + to_string(value));
    }
}

} // namespace
