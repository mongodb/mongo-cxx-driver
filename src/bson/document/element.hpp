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

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

#include "bson/string_or_literal.hpp"

namespace bson {

enum class type : std::uint8_t;
enum class binary_sub_type : std::uint8_t;

namespace types {
struct b_eod;
struct b_double;
struct b_utf8;
struct b_document;
struct b_array;
struct b_binary;
struct b_undefined;
struct b_oid;
struct b_bool;
struct b_date;
struct b_null;
struct b_regex;
struct b_dbpointer;
struct b_code;
struct b_symbol;
struct b_codewscope;
struct b_int32;
struct b_timestamp;
struct b_int64;
struct b_minkey;
struct b_maxkey;
}  // namespace types

namespace builder {
class concrete;
}  // namespace builder

namespace document {

class view;

class LIBMONGOCXX_EXPORT element {
    friend class document::view;
    friend class builder::concrete;

   public:
    element();
    element(const void* iter);

    bool operator==(const element& rhs) const;

    bson::type type() const;

    string_or_literal key() const;

    types::b_eod get_eod() const;
    types::b_double get_double() const;
    types::b_utf8 get_utf8() const;
    types::b_document get_document() const;
    types::b_array get_array() const;
    types::b_binary get_binary() const;
    types::b_undefined get_undefined() const;
    types::b_oid get_oid() const;
    types::b_bool get_bool() const;
    types::b_date get_date() const;
    types::b_null get_null() const;
    types::b_regex get_regex() const;
    types::b_dbpointer get_dbpointer() const;
    types::b_code get_code() const;
    types::b_symbol get_symbol() const;
    types::b_codewscope get_codewscope() const;
    types::b_int32 get_int32() const;
    types::b_timestamp get_timestamp() const;
    types::b_int64 get_int64() const;
    types::b_minkey get_minkey() const;
    types::b_maxkey get_maxkey() const;

    friend std::ostream& operator<<(std::ostream& out, const element& element);

   private:
    const uint8_t* _raw;
    uint32_t _len;
    uint32_t _off;
};

}  // namespace document
}  // namespace bson

#include "driver/config/postlude.hpp"
