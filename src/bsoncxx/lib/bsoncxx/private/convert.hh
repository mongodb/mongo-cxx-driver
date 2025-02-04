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

#include <bsoncxx/v1/detail/macros.hpp>

#include <cstdlib>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>

namespace bsoncxx {
namespace v_noabi {
namespace types {

inline char* make_copy_for_libbson(stdx::string_view s, uint32_t* len_out = nullptr) {
    // Append a nul byte to the end of the string
    auto copy = static_cast<char*>(bson_malloc0(s.length() + 1));
    std::memcpy(copy, s.data(), s.length());
    copy[s.length()] = '\0';

    if (len_out) {
        *len_out = static_cast<std::uint32_t>(s.length());
    }

    return copy;
}

inline void convert_to_libbson(b_binary const& binary, bson_value_t* v) {
    v->value_type = BSON_TYPE_BINARY;

    v->value.v_binary.subtype = static_cast<bson_subtype_t>(binary.sub_type);
    v->value.v_binary.data_len = binary.size;
    v->value.v_binary.data = static_cast<std::uint8_t*>(bson_malloc0(binary.size));
    std::memcpy(v->value.v_binary.data, binary.bytes, binary.size);
}

inline void convert_to_libbson(b_string const& str, bson_value_t* v) {
    v->value_type = BSON_TYPE_UTF8;
    v->value.v_utf8.str = make_copy_for_libbson(str.value, &(v->value.v_utf8.len));
}

inline void convert_to_libbson(b_double const& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_DOUBLE;
    v->value.v_double = val.value;
}

inline void convert_to_libbson(b_int32 const& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_INT32;
    v->value.v_int32 = val.value;
}

inline void convert_to_libbson(b_int64 const& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_INT64;
    v->value.v_int64 = val.value;
}

inline void convert_to_libbson(b_undefined const&, bson_value_t* v) {
    v->value_type = BSON_TYPE_UNDEFINED;
}

inline void convert_to_libbson(b_oid const& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_OID;
    std::memcpy(&v->value.v_oid.bytes, val.value.bytes(), val.value.k_oid_length);
}

inline void convert_to_libbson(b_decimal128 const& decimal, bson_value_t* v) {
    v->value_type = BSON_TYPE_DECIMAL128;

    v->value.v_decimal128.high = decimal.value.high();
    v->value.v_decimal128.low = decimal.value.low();
}

inline void convert_to_libbson(b_bool const& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_BOOL;
    v->value.v_bool = val.value;
}

inline void convert_to_libbson(b_date const& date, bson_value_t* v) {
    v->value_type = BSON_TYPE_DATE_TIME;
    v->value.v_datetime = date.value.count();
}

inline void convert_to_libbson(b_null const&, bson_value_t* v) {
    v->value_type = BSON_TYPE_NULL;
}

inline void convert_to_libbson(b_regex const& regex, bson_value_t* v) {
    v->value_type = BSON_TYPE_REGEX;
    v->value.v_regex.options = make_copy_for_libbson(regex.options);
    v->value.v_regex.regex = make_copy_for_libbson(regex.regex);
}

inline void convert_to_libbson(b_dbpointer const& db, bson_value_t* v) {
    v->value_type = BSON_TYPE_DBPOINTER;

    v->value.v_dbpointer.collection = make_copy_for_libbson(db.collection, &(v->value.v_dbpointer.collection_len));

    std::memcpy((v->value.v_dbpointer.oid.bytes), db.value.bytes(), db.value.k_oid_length);
}

inline void convert_to_libbson(b_code const& code, bson_value_t* v) {
    v->value_type = BSON_TYPE_CODE;
    v->value.v_code.code = make_copy_for_libbson(code.code, &(v->value.v_code.code_len));
}

inline void convert_to_libbson(b_symbol const& symbol, bson_value_t* v) {
    v->value_type = BSON_TYPE_SYMBOL;
    v->value.v_symbol.symbol = make_copy_for_libbson(symbol.symbol, &(v->value.v_symbol.len));
}

inline void convert_to_libbson(b_codewscope const& code, bson_value_t* v) {
    v->value_type = BSON_TYPE_CODEWSCOPE;

    // Copy the code
    v->value.v_codewscope.code = make_copy_for_libbson(code.code, &(v->value.v_codewscope.code_len));

    // Copy the scope
    if (code.scope.length() == 0) {
        v->value.v_codewscope.scope_data = nullptr;
        v->value.v_codewscope.scope_len = 0;
    } else {
        v->value.v_codewscope.scope_data = static_cast<std::uint8_t*>(bson_malloc0(code.scope.length()));
        v->value.v_codewscope.scope_len = static_cast<std::uint32_t>(code.scope.length());
        std::memcpy(v->value.v_codewscope.scope_data, code.scope.data(), code.scope.length());
    }
}

inline void convert_to_libbson(b_timestamp const& t, bson_value_t* v) {
    v->value_type = BSON_TYPE_TIMESTAMP;
    v->value.v_timestamp.timestamp = t.timestamp;
    v->value.v_timestamp.increment = t.increment;
}

inline void convert_to_libbson(b_minkey const&, bson_value_t* v) {
    v->value_type = BSON_TYPE_MINKEY;
}

inline void convert_to_libbson(b_maxkey const&, bson_value_t* v) {
    v->value_type = BSON_TYPE_MAXKEY;
}

inline void convert_to_libbson(b_document const& doc, bson_value_t* v) {
    v->value_type = BSON_TYPE_DOCUMENT;

    v->value.v_doc.data_len = static_cast<std::uint32_t>(doc.value.length());
    if (0 == (v->value.v_doc.data_len)) {
        v->value.v_doc.data = nullptr;
    } else {
        v->value.v_doc.data = static_cast<std::uint8_t*>(bson_malloc0(v->value.v_doc.data_len));
        std::memcpy(v->value.v_doc.data, doc.value.data(), v->value.v_doc.data_len);
    }
}

inline void convert_to_libbson(b_array const& arr, bson_value_t* v) {
    v->value_type = BSON_TYPE_ARRAY;
    // The bson_value_t struct does not have a separate union
    // member for arrays. They are handled the same as the document
    // BSON type.
    v->value.v_doc.data_len = static_cast<std::uint32_t>(arr.value.length());
    if (v->value.v_doc.data_len == 0) {
        v->value.v_doc.data = nullptr;
    } else {
        v->value.v_doc.data = static_cast<std::uint8_t*>(bson_malloc0(arr.value.length()));
        std::memcpy(v->value.v_doc.data, arr.value.data(), arr.value.length());
    }
}

//
// Helper to convert without caller being aware of the underlying bson type.
//
inline void convert_to_libbson(bson_value_t* v, bson_value::view const& bson_view) {
    switch (bson_view.type()) {
#define BSONCXX_ENUM(name, val)              \
    case bsoncxx::v_noabi::type::k_##name: { \
        auto value = bson_view.get_##name(); \
        convert_to_libbson(value, v);        \
        break;                               \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        default:
            BSONCXX_PRIVATE_UNREACHABLE;
    }
}

inline void convert_from_libbson(bson_value_t const* v, b_binary* out) {
    bson_subtype_t subtype = v->value.v_binary.subtype;
    std::uint32_t len = v->value.v_binary.data_len;
    std::uint8_t const* binary = v->value.v_binary.data;

    *out = {static_cast<binary_sub_type>(subtype), len, binary};
}

inline void convert_from_libbson(bson_value_t const* v, b_string* out) {
    uint32_t len = v->value.v_utf8.len;
    char const* val = v->value.v_utf8.str;

    *out = b_string{stdx::string_view{val, len}};
}

inline void convert_from_libbson(bson_value_t const* v, b_double* out) {
    *out = b_double{v->value.v_double};
}

inline void convert_from_libbson(bson_value_t const* v, b_int32* out) {
    *out = b_int32{v->value.v_int32};
}

inline void convert_from_libbson(bson_value_t const* v, b_int64* out) {
    *out = b_int64{v->value.v_int64};
}

inline void convert_from_libbson(bson_value_t const*, b_undefined* out) {
    *out = b_undefined{};
}

inline void convert_from_libbson(bson_value_t const* v, b_oid* out) {
    bson_oid_t const* boid = &(v->value.v_oid);
    oid val_oid(reinterpret_cast<char const*>(boid->bytes), sizeof(boid->bytes));
    *out = b_oid{std::move(val_oid)};
}

inline void convert_from_libbson(bson_value_t const* v, b_decimal128* out) {
    bson_decimal128_t d128 = v->value.v_decimal128;
    *out = b_decimal128{decimal128{d128.high, d128.low}};
}

inline void convert_from_libbson(bson_value_t const* v, b_bool* out) {
    *out = b_bool{v->value.v_bool};
}

inline void convert_from_libbson(bson_value_t const* v, b_date* out) {
    *out = b_date{std::chrono::milliseconds{v->value.v_datetime}};
}

inline void convert_from_libbson(bson_value_t const*, b_null* out) {
    *out = b_null{};
}

inline void convert_from_libbson(bson_value_t const* v, b_regex* out) {
    char const* options = v->value.v_regex.options;
    char const* regex = v->value.v_regex.regex;
    *out = b_regex{regex, options ? options : stdx::string_view{}};
}

inline void convert_from_libbson(bson_value_t const* v, b_dbpointer* out) {
    uint32_t collection_len = v->value.v_dbpointer.collection_len;
    char const* collection = v->value.v_dbpointer.collection;
    bson_oid_t const* boid = &(v->value.v_dbpointer.oid);

    oid oid{reinterpret_cast<char const*>(boid->bytes), sizeof(boid->bytes)};

    *out = b_dbpointer{stdx::string_view{collection, collection_len}, std::move(oid)};
}

inline void convert_from_libbson(bson_value_t const* v, b_code* out) {
    uint32_t len = v->value.v_code.code_len;
    char const* code = v->value.v_code.code;

    *out = b_code{stdx::string_view{code, len}};
}

inline void convert_from_libbson(bson_value_t const* v, b_symbol* out) {
    uint32_t len = v->value.v_symbol.len;
    char const* symbol = v->value.v_symbol.symbol;

    *out = b_symbol{stdx::string_view{symbol, len}};
}

inline void convert_from_libbson(bson_value_t const* v, b_codewscope* out) {
    uint32_t code_len = v->value.v_codewscope.code_len;
    uint8_t const* scope_ptr = v->value.v_codewscope.scope_data;
    uint32_t scope_len = v->value.v_codewscope.scope_len;
    char const* code = v->value.v_codewscope.code;
    document::view view(scope_ptr, scope_len);

    *out = b_codewscope{stdx::string_view{code, code_len}, view};
}

inline void convert_from_libbson(bson_value_t const* v, b_timestamp* out) {
    uint32_t timestamp = v->value.v_timestamp.timestamp;
    uint32_t increment = v->value.v_timestamp.increment;
    *out = {increment, timestamp};
}

inline void convert_from_libbson(bson_value_t const*, b_minkey* out) {
    *out = b_minkey{};
}

inline void convert_from_libbson(bson_value_t const*, b_maxkey* out) {
    *out = b_maxkey{};
}

inline void convert_from_libbson(bson_value_t const* v, b_document* out) {
    std::uint8_t const* buf = v->value.v_doc.data;
    std::uint32_t len = v->value.v_doc.data_len;

    *out = b_document{document::view{buf, len}};
}

inline void convert_from_libbson(bson_value_t const* v, b_array* out) {
    // The bson_value_t struct does not have a separate union
    // member for arrays. They are handled the same as the document
    // BSON type.
    std::uint8_t const* buf = v->value.v_doc.data;
    std::uint32_t len = v->value.v_doc.data_len;

    *out = b_array{array::view{buf, len}};
}

} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
