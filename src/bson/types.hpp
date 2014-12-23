// Copyright 2014 MongoDB Inc.
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

#include "driver/config/prelude.hpp"

#include "bson/string_or_literal.hpp"
#include "bson/document.hpp"
#include "bson/oid.hpp"

namespace bson {

enum class type : std::uint8_t {
#define MONGOCXX_ENUM(name, val) k_##name = val,
#include "bson/enums/type.hpp"
#undef MONGOCXX_ENUM
};

enum class binary_sub_type : std::uint8_t {
#define MONGOCXX_ENUM(name, val) k_##name = val,
#include "bson/enums/binary_sub_type.hpp"
#undef MONGOCXX_ENUM
};
std::ostream& operator<<(std::ostream& out, type rhs);
std::ostream& operator<<(std::ostream& out, binary_sub_type rhs);

namespace types {

struct b_eod {
    static constexpr auto type_id = type::k_eod;
};

struct b_double {
    static constexpr auto type_id = type::k_double;

    double value;

    operator double() { return value; }
};

struct b_utf8 {
    static constexpr auto type_id = type::k_utf8;

    template <typename T>
    explicit b_utf8(T&& value)
        : value(std::forward<T>(value)) {}

    string_or_literal value;

    operator string_or_literal() { return value; }
};

struct b_document {
    static constexpr auto type_id = type::k_document;

    document::view value;

    operator document::view() { return value; }
};

struct b_array {
    static constexpr auto type_id = type::k_array;

    document::view value;

    operator document::view() { return value; }
};

struct b_binary {
    static constexpr auto type_id = type::k_binary;

    binary_sub_type sub_type;
    uint32_t size;
    const uint8_t* bytes;
};

struct b_undefined {
    static constexpr auto type_id = type::k_undefined;
};

struct b_oid {
    static constexpr auto type_id = type::k_oid;

    oid value;
};

struct b_bool {
    static constexpr auto type_id = type::k_bool;

    bool value;

    operator bool() { return value; }
};

struct b_date {
    static constexpr auto type_id = type::k_date;

    int64_t value;

    operator int64_t() { return value; }
};

struct b_null {
    static constexpr auto type_id = type::k_null;
};

struct b_regex {
    static constexpr auto type_id = type::k_regex;

    template <typename T, typename U>
    explicit b_regex(T&& regex, U&& options)
        : regex(std::forward<T>(regex)), options(std::forward<U>(options)) {}

    string_or_literal regex;
    string_or_literal options;
};

struct b_dbpointer {
    static constexpr auto type_id = type::k_dbpointer;

    string_or_literal collection;
    oid value;
};

struct b_code {
    static constexpr auto type_id = type::k_code;

    template <typename T>
    explicit b_code(T&& code)
        : code(std::forward<T>(code)) {}

    string_or_literal code;

    operator string_or_literal() { return code; }
};

struct b_symbol {
    static constexpr auto type_id = type::k_symbol;

    template <typename T>
    explicit b_symbol(T&& symbol)
        : symbol(std::forward<T>(symbol)) {}

    string_or_literal symbol;

    operator string_or_literal() { return symbol; }
};

struct b_codewscope {
    static constexpr auto type_id = type::k_codewscope;

    template <typename T, typename U>
    explicit b_codewscope(T&& code, U&& scope)
        : code(std::forward<T>(code)), scope(std::forward<U>(scope)) {}

    string_or_literal code;
    document::view scope;
};

struct b_int32 {
    static constexpr auto type_id = type::k_int32;

    int32_t value;

    operator int32_t() { return value; }
};

struct b_timestamp {
    static constexpr auto type_id = type::k_timestamp;

    uint32_t increment;
    uint32_t timestamp;
};

struct b_int64 {
    static constexpr auto type_id = type::k_int64;

    int64_t value;

    operator int64_t() { return value; }
};

struct b_minkey {
    static constexpr auto type_id = type::k_minkey;
};

struct b_maxkey {
    static constexpr auto type_id = type::k_maxkey;
};

#define MONGOCXX_ENUM(name, val) std::ostream& operator<<(std::ostream& out, const b_##name& rhs);
#include "bson/enums/type.hpp"
#undef MONGOCXX_ENUM

}  // namespace types
}  // namespace bson

#include "driver/config/postlude.hpp"
