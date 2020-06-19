// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <cstdlib>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {

BSONCXX_INLINE char* make_copy_for_libbson(stdx::string_view s, uint32_t* len_out = nullptr) {
    // Append a nul byte to the end of the string
    char* copy = (char*)bson_malloc0(s.length() + 1);
    std::memcpy(copy, s.data(), s.length());
    copy[s.length()] = '\0';

    if (len_out) {
        *len_out = (uint32_t)(s.length());
    }

    return copy;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_binary& binary, bson_value_t* v) {
    v->value_type = BSON_TYPE_BINARY;

    v->value.v_binary.subtype = static_cast<bson_subtype_t>(binary.sub_type);
    v->value.v_binary.data_len = binary.size;
    v->value.v_binary.data = (uint8_t*)bson_malloc0(binary.size);
    std::memcpy(v->value.v_binary.data, binary.bytes, binary.size);
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_utf8& utf8, bson_value_t* v) {
    v->value_type = BSON_TYPE_UTF8;
    v->value.v_utf8.str = make_copy_for_libbson(utf8.value, &(v->value.v_utf8.len));
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_double& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_DOUBLE;
    v->value.v_double = val.value;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_int32& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_INT32;
    v->value.v_int32 = val.value;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_int64& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_INT64;
    v->value.v_int64 = val.value;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_undefined&, bson_value_t* v) {
    v->value_type = BSON_TYPE_UNDEFINED;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_oid& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_OID;
    std::memcpy(&v->value.v_oid.bytes, val.value.bytes(), val.value.k_oid_length);
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_decimal128& decimal,
                                       bson_value_t* v) {
    v->value_type = BSON_TYPE_DECIMAL128;

    v->value.v_decimal128.high = decimal.value.high();
    v->value.v_decimal128.low = decimal.value.low();
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_bool& val, bson_value_t* v) {
    v->value_type = BSON_TYPE_BOOL;
    v->value.v_bool = val.value;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_date& date, bson_value_t* v) {
    v->value_type = BSON_TYPE_DATE_TIME;
    v->value.v_datetime = date.value.count();
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_null&, bson_value_t* v) {
    v->value_type = BSON_TYPE_NULL;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_regex& regex, bson_value_t* v) {
    v->value_type = BSON_TYPE_REGEX;
    v->value.v_regex.options = make_copy_for_libbson(regex.options);
    v->value.v_regex.regex = make_copy_for_libbson(regex.regex);
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_dbpointer& db, bson_value_t* v) {
    v->value_type = BSON_TYPE_DBPOINTER;

    v->value.v_dbpointer.collection =
        make_copy_for_libbson(db.collection, &(v->value.v_dbpointer.collection_len));

    std::memcpy((v->value.v_dbpointer.oid.bytes), db.value.bytes(), db.value.k_oid_length);
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_code& code, bson_value_t* v) {
    v->value_type = BSON_TYPE_CODE;
    v->value.v_code.code = make_copy_for_libbson(code.code, &(v->value.v_code.code_len));
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_symbol& symbol, bson_value_t* v) {
    v->value_type = BSON_TYPE_SYMBOL;
    v->value.v_symbol.symbol = make_copy_for_libbson(symbol.symbol, &(v->value.v_symbol.len));
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_codewscope& code, bson_value_t* v) {
    v->value_type = BSON_TYPE_CODEWSCOPE;

    // Copy the code
    v->value.v_codewscope.code =
        make_copy_for_libbson(code.code, &(v->value.v_codewscope.code_len));

    // Copy the scope
    if (code.scope.length() == 0) {
        v->value.v_codewscope.scope_data = nullptr;
        v->value.v_codewscope.scope_len = 0;
    } else {
        v->value.v_codewscope.scope_data = (uint8_t*)bson_malloc0(code.scope.length());
        v->value.v_codewscope.scope_len = (uint32_t)code.scope.length();
        std::memcpy(v->value.v_codewscope.scope_data, code.scope.data(), code.scope.length());
    }
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_timestamp& t, bson_value_t* v) {
    v->value_type = BSON_TYPE_TIMESTAMP;
    v->value.v_timestamp.timestamp = t.timestamp;
    v->value.v_timestamp.increment = t.increment;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_minkey&, bson_value_t* v) {
    v->value_type = BSON_TYPE_MINKEY;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_maxkey&, bson_value_t* v) {
    v->value_type = BSON_TYPE_MAXKEY;
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_document& doc, bson_value_t* v) {
    v->value_type = BSON_TYPE_DOCUMENT;

    v->value.v_doc.data_len = (uint32_t)doc.value.length();
    if (0 == (v->value.v_doc.data_len)) {
        v->value.v_doc.data = nullptr;
    } else {
        v->value.v_doc.data = (uint8_t*)bson_malloc0(v->value.v_doc.data_len);
        std::memcpy(v->value.v_doc.data, doc.value.data(), v->value.v_doc.data_len);
    }
}

BSONCXX_INLINE void convert_to_libbson(const bsoncxx::types::b_array& arr, bson_value_t* v) {
    v->value_type = BSON_TYPE_ARRAY;
    // The bson_value_t struct does not have a separate union
    // member for arrays. They are handled the same as the document
    // BSON type.
    v->value.v_doc.data_len = (uint32_t)arr.value.length();
    if (v->value.v_doc.data_len == 0) {
        v->value.v_doc.data = nullptr;
    } else {
        v->value.v_doc.data = (uint8_t*)bson_malloc0(arr.value.length());
        std::memcpy(v->value.v_doc.data, arr.value.data(), arr.value.length());
    }
}

//
// Helper to convert without caller being aware of the underlying bson type.
//
BSONCXX_INLINE void convert_to_libbson(bson_value_t* v, const class bson_value::view& bson_view) {
    switch (bson_view.type()) {
#define BSONCXX_ENUM(name, val)              \
    case bsoncxx::type::k_##name: {          \
        auto value = bson_view.get_##name(); \
        convert_to_libbson(value, v);        \
        break;                               \
    }
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        default:
            BSONCXX_UNREACHABLE;
    }
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_binary* out) {
    bson_subtype_t subtype = v->value.v_binary.subtype;
    std::uint32_t len = v->value.v_binary.data_len;
    const std::uint8_t* binary = v->value.v_binary.data;

    *out = {static_cast<binary_sub_type>(subtype), len, binary};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_utf8* out) {
    uint32_t len = v->value.v_utf8.len;
    const char* val = v->value.v_utf8.str;

    *out = bsoncxx::types::b_utf8{stdx::string_view{val, len}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_double* out) {
    *out = bsoncxx::types::b_double{v->value.v_double};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_int32* out) {
    *out = bsoncxx::types::b_int32{v->value.v_int32};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_int64* out) {
    *out = bsoncxx::types::b_int64{v->value.v_int64};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t*, bsoncxx::types::b_undefined* out) {
    *out = bsoncxx::types::b_undefined{};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_oid* out) {
    const bson_oid_t* boid = &(v->value.v_oid);
    oid val_oid(reinterpret_cast<const char*>(boid->bytes), sizeof(boid->bytes));
    *out = bsoncxx::types::b_oid{std::move(val_oid)};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_decimal128* out) {
    bson_decimal128_t d128 = v->value.v_decimal128;
    *out = bsoncxx::types::b_decimal128{decimal128{d128.high, d128.low}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_bool* out) {
    *out = bsoncxx::types::b_bool{v->value.v_bool};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_date* out) {
    *out = bsoncxx::types::b_date{std::chrono::milliseconds{v->value.v_datetime}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t*, bsoncxx::types::b_null* out) {
    *out = bsoncxx::types::b_null{};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_regex* out) {
    const char* options = v->value.v_regex.options;
    const char* regex = v->value.v_regex.regex;
    *out = bsoncxx::types::b_regex{stdx::string_view{regex}, stdx::string_view{options}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_dbpointer* out) {
    uint32_t collection_len = v->value.v_dbpointer.collection_len;
    const char* collection = v->value.v_dbpointer.collection;
    const bson_oid_t* boid = &(v->value.v_dbpointer.oid);

    oid oid{reinterpret_cast<const char*>(boid->bytes), sizeof(boid->bytes)};

    *out =
        bsoncxx::types::b_dbpointer{stdx::string_view{collection, collection_len}, std::move(oid)};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_code* out) {
    uint32_t len = v->value.v_code.code_len;
    const char* code = v->value.v_code.code;

    *out = bsoncxx::types::b_code{stdx::string_view{code, len}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_symbol* out) {
    uint32_t len = v->value.v_symbol.len;
    const char* symbol = v->value.v_symbol.symbol;

    *out = bsoncxx::types::b_symbol{stdx::string_view{symbol, len}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_codewscope* out) {
    uint32_t code_len = v->value.v_codewscope.code_len;
    const uint8_t* scope_ptr = v->value.v_codewscope.scope_data;
    uint32_t scope_len = v->value.v_codewscope.scope_len;
    const char* code = v->value.v_codewscope.code;
    document::view view(scope_ptr, scope_len);

    *out = bsoncxx::types::b_codewscope{stdx::string_view{code, code_len}, view};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_timestamp* out) {
    uint32_t timestamp = v->value.v_timestamp.timestamp;
    uint32_t increment = v->value.v_timestamp.increment;
    *out = {increment, timestamp};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t*, bsoncxx::types::b_minkey* out) {
    *out = bsoncxx::types::b_minkey{};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t*, bsoncxx::types::b_maxkey* out) {
    *out = bsoncxx::types::b_maxkey{};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_document* out) {
    const std::uint8_t* buf = v->value.v_doc.data;
    std::uint32_t len = v->value.v_doc.data_len;

    *out = bsoncxx::types::b_document{document::view{buf, len}};
}

BSONCXX_INLINE void convert_from_libbson(bson_value_t* v, bsoncxx::types::b_array* out) {
    // The bson_value_t struct does not have a separate union
    // member for arrays. They are handled the same as the document
    // BSON type.
    const std::uint8_t* buf = v->value.v_doc.data;
    std::uint32_t len = v->value.v_doc.data_len;

    *out = bsoncxx::types::b_array{array::view{buf, len}};
}

}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
