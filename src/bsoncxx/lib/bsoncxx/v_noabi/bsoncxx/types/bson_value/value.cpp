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

#include <bsoncxx/types/bson_value/value.hpp>

//

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

static_assert(is_explicitly_convertible<value&&, v1::types::value>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<value const&, v1::types::value>::value, "v_noabi -> v1 must be explicit");
static_assert(is_implicitly_convertible<v1::types::value&&, value>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::types::value const&, value>::value, "v1 -> v_noabi must be implicit");

value::value(v_noabi::type const id, v1::stdx::string_view v) {
    switch (id) {
        case v_noabi::type::k_regex:
            _value = v1::types::value{v1::types::b_regex{v}};
            break;
        case v_noabi::type::k_code:
            _value = v1::types::value{v1::types::b_code{v}};
            break;
        case v_noabi::type::k_symbol:
            _value = v1::types::value{v1::types::b_symbol{v}};
            break;

        case v_noabi::type::k_double:
        case v_noabi::type::k_string:
        case v_noabi::type::k_document:
        case v_noabi::type::k_array:
        case v_noabi::type::k_binary:
        case v_noabi::type::k_undefined:
        case v_noabi::type::k_oid:
        case v_noabi::type::k_bool:
        case v_noabi::type::k_date:
        case v_noabi::type::k_null:
        case v_noabi::type::k_dbpointer:
        case v_noabi::type::k_codewscope:
        case v_noabi::type::k_int32:
        case v_noabi::type::k_timestamp:
        case v_noabi::type::k_int64:
        case v_noabi::type::k_decimal128:
        case v_noabi::type::k_maxkey:
        case v_noabi::type::k_minkey:
        default:
            throw v_noabi::exception(v_noabi::error_code::k_invalid_bson_type_id);
    }
}

value::value(v_noabi::type const id) {
    switch (id) {
        case v_noabi::type::k_minkey:
            _value = v1::types::value{v1::types::b_minkey{}};
            break;
        case v_noabi::type::k_maxkey:
            _value = v1::types::value{v1::types::b_maxkey{}};
            break;
        case v_noabi::type::k_undefined:
            _value = v1::types::value{v1::types::b_undefined{}};
            break;

        case v_noabi::type::k_double:
        case v_noabi::type::k_string:
        case v_noabi::type::k_document:
        case v_noabi::type::k_array:
        case v_noabi::type::k_binary:
        case v_noabi::type::k_oid:
        case v_noabi::type::k_bool:
        case v_noabi::type::k_date:
        case v_noabi::type::k_null:
        case v_noabi::type::k_regex:
        case v_noabi::type::k_dbpointer:
        case v_noabi::type::k_code:
        case v_noabi::type::k_symbol:
        case v_noabi::type::k_codewscope:
        case v_noabi::type::k_int32:
        case v_noabi::type::k_timestamp:
        case v_noabi::type::k_int64:
        case v_noabi::type::k_decimal128:
        default:
            throw v_noabi::exception(v_noabi::error_code::k_invalid_bson_type_id);
    }
}
value::value(v_noabi::type id, std::uint64_t a, std::uint64_t b) {
    switch (id) {
        case v_noabi::type::k_decimal128:
            _value = v1::types::value{v1::types::b_decimal128{{a, b}}};
            break;
        case v_noabi::type::k_timestamp:
            _value =
                v1::types::value{v1::types::b_timestamp{static_cast<std::uint32_t>(a), static_cast<std::uint32_t>(b)}};
            break;

        case v_noabi::type::k_double:
        case v_noabi::type::k_string:
        case v_noabi::type::k_document:
        case v_noabi::type::k_array:
        case v_noabi::type::k_binary:
        case v_noabi::type::k_undefined:
        case v_noabi::type::k_oid:
        case v_noabi::type::k_bool:
        case v_noabi::type::k_date:
        case v_noabi::type::k_null:
        case v_noabi::type::k_regex:
        case v_noabi::type::k_dbpointer:
        case v_noabi::type::k_code:
        case v_noabi::type::k_symbol:
        case v_noabi::type::k_codewscope:
        case v_noabi::type::k_int32:
        case v_noabi::type::k_int64:
        case v_noabi::type::k_maxkey:
        case v_noabi::type::k_minkey:
        default:
            throw v_noabi::exception(error_code::k_invalid_bson_type_id);
    }
}

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
