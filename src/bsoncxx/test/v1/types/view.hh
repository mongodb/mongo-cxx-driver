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

#pragma once

#include <bsoncxx/v1/types/view.hpp>

//

#include <bsoncxx/test/v1/array/view.hh>       // StringMaker<bsoncxx::v1::array::view>
#include <bsoncxx/test/v1/decimal128.hh>       // StringMaker<bsoncxx::v1::decimal128>
#include <bsoncxx/test/v1/document/view.hh>    // StringMaker<bsoncxx::v1::document::view>
#include <bsoncxx/test/v1/oid.hh>              // StringMaker<bsoncxx::v1::oid>
#include <bsoncxx/test/v1/stdx/string_view.hh> // StringMaker<bsoncxx::v1::stdx::string_view>

#include <iomanip>
#include <sstream>
#include <utility>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_tostring.hpp>

CATCH_REGISTER_ENUM(
    bsoncxx::v1::types::view::errc,
    bsoncxx::v1::types::view::errc::zero,
    bsoncxx::v1::types::view::errc::type_mismatch)

// BSONCXX_V1_TYPES_XMACRO: update below.

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_double> : StringMaker<double> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_string> : StringMaker<bsoncxx::v1::stdx::string_view> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_document> : StringMaker<bsoncxx::v1::document::view> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_array> : StringMaker<bsoncxx::v1::array::view> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_binary> {
    static std::string convert(bsoncxx::v1::types::b_binary const& value) {
        if (!value.bytes || value.size == 0u) {
            return "empty";
        }

        std::ostringstream oss;
        oss << std::hex;
        auto const end = value.bytes + value.size;
        for (auto const* ptr = value.bytes; ptr != end; ++ptr) {
            oss << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(*ptr);
        }
        return std::move(oss).str();
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_undefined> {
    static std::string convert(bsoncxx::v1::types::b_undefined const&) {
        return "undefined";
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_oid> : StringMaker<bsoncxx::v1::oid> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_bool> : StringMaker<bool> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_date> {
    static std::string convert(bsoncxx::v1::types::b_date const& value) {
        using time_point = std::chrono::time_point<std::chrono::system_clock>;
        return bsoncxx::test::stringify(time_point{value.value});
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_null> {
    static std::string convert(bsoncxx::v1::types::b_null const&) {
        return "null";
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_regex> {
    static std::string convert(bsoncxx::v1::types::b_regex const& value) {
        std::string res;
        res += R"({"pattern": )";
        res += bsoncxx::test::stringify(value.regex);
        res += R"(, "options": )";
        res += bsoncxx::test::stringify(value.options);
        res += '}';
        return res;
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_dbpointer> {
    static std::string convert(bsoncxx::v1::types::b_dbpointer const& value) {
        std::string res;
        res += R"({"$ref": )";
        res += bsoncxx::test::stringify(value.collection);
        res += R"(, "$oid": )";
        res += bsoncxx::test::stringify(value.value);
        res += '}';
        return res;
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_code> : StringMaker<bsoncxx::v1::stdx::string_view> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_symbol> : StringMaker<bsoncxx::v1::stdx::string_view> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_codewscope> {
    static std::string convert(bsoncxx::v1::types::b_codewscope const& value) {
        std::string res;
        res += R"({"code": )";
        res += bsoncxx::test::stringify(value.code);
        res += R"(, "scope": )";
        res += bsoncxx::test::stringify(value.scope);
        res += '}';
        return res;
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_int32> : StringMaker<std::int32_t> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_timestamp> {
    static std::string convert(bsoncxx::v1::types::b_timestamp const& value) {
        std::string res;
        res += R"({"t": )";
        res += bsoncxx::test::stringify(value.timestamp);
        res += R"(, "i": )";
        res += bsoncxx::test::stringify(value.increment);
        res += '}';
        return res;
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_int64> : StringMaker<std::int64_t> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_decimal128> : StringMaker<bsoncxx::v1::decimal128> {};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_maxkey> {
    static std::string convert(bsoncxx::v1::types::b_maxkey const&) {
        return "maxkey";
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::b_minkey> {
    static std::string convert(bsoncxx::v1::types::b_minkey const&) {
        return "minkey";
    }
};

// BSONCXX_V1_TYPES_XMACRO: update above.

template <>
struct Catch::StringMaker<bsoncxx::v1::types::view> {
    static std::string convert(bsoncxx::v1::types::view const& value) {
#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case bsoncxx::v1::types::id::k_##_name: \
        return bsoncxx::test::stringify(value.get_##_name());

        switch (value.type_id()) {
            BSONCXX_V1_TYPES_XMACRO(X)

            default:
                return "?";
        }
#pragma pop_macro("X")
    }
};
