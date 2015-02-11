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

#include <bsoncxx/config/prelude.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <type_traits>

#include <bsoncxx/types.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace document {
class element;
}  // namespace document

namespace types {

    class BSONCXX_API value {

       public:
        value();

        explicit value(const document::element& element);

        explicit value(b_double);
        explicit value(b_utf8);
        explicit value(b_document);
        explicit value(b_array);
        explicit value(b_binary);
        explicit value(b_undefined);
        explicit value(b_oid);
        explicit value(b_bool);
        explicit value(b_date);
        explicit value(b_null);
        explicit value(b_regex);
        explicit value(b_dbpointer);
        explicit value(b_code);
        explicit value(b_symbol);
        explicit value(b_codewscope);
        explicit value(b_int32);
        explicit value(b_timestamp);
        explicit value(b_int64);
        explicit value(b_minkey);
        explicit value(b_maxkey);

        value(const value&);
        value& operator=(const value&);

        value(value&&);
        value& operator=(value&&);

        ~value();

        explicit operator bool() const;

        friend BSONCXX_API bool operator==(const value&, const value&);
        friend BSONCXX_API bool operator!=(const value&, const value&);

        bsoncxx::type type() const;

        b_double get_double() const;
        b_utf8 get_utf8() const;
        b_document get_document() const;
        b_array get_array() const;
        b_binary get_binary() const;
        b_undefined get_undefined() const;
        b_oid get_oid() const;
        b_bool get_bool() const;
        b_date get_date() const;
        b_null get_null() const;
        b_regex get_regex() const;
        b_dbpointer get_dbpointer() const;
        b_code get_code() const;
        b_symbol get_symbol() const;
        b_codewscope get_codewscope() const;
        b_int32 get_int32() const;
        b_timestamp get_timestamp() const;
        b_int64 get_int64() const;
        b_minkey get_minkey() const;
        b_maxkey get_maxkey() const;

       private:
        enum class mode : std::uint8_t {
            unset,
            element,
            variant,
        } _mode;
        const document::element* _element;

        bsoncxx::type _type;
        union {
            struct b_double _b_double;
            struct b_utf8 _b_utf8;
            struct b_document _b_document;
            struct b_array _b_array;
            struct b_binary _b_binary;
            struct b_undefined _b_undefined;
            struct b_oid _b_oid;
            struct b_bool _b_bool;
            struct b_date _b_date;
            struct b_null _b_null;
            struct b_regex _b_regex;
            struct b_dbpointer _b_dbpointer;
            struct b_code _b_code;
            struct b_symbol _b_symbol;
            struct b_codewscope _b_codewscope;
            struct b_int32 _b_int32;
            struct b_timestamp _b_timestamp;
            struct b_int64 _b_int64;
            struct b_minkey _b_minkey;
            struct b_maxkey _b_maxkey;
        };
    };

}  // namespace types

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
