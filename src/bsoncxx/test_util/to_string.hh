// Copyright 2022 MongoDB Inc.
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

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

inline std::string to_string(types::bson_value::view_or_value val) {
    switch (val.view().type()) {
        case bsoncxx::type::k_string:
            return string::to_string(val.view().get_string().value);
        case bsoncxx::type::k_int32:
            return std::to_string(val.view().get_int32().value);
        case bsoncxx::type::k_int64:
            return std::to_string(val.view().get_int64().value);
        case bsoncxx::type::k_document:
            return to_json(val.view().get_document().value);
        case bsoncxx::type::k_array:
            return to_json(val.view().get_array().value);
        case bsoncxx::type::k_oid:
            return val.view().get_oid().value.to_string();
        case bsoncxx::type::k_binary: {
            const auto& binary = val.view().get_binary();
            std::stringstream ss;
            ss << std::hex;
            for (auto&& byte :
                 std::vector<unsigned int>(binary.bytes, binary.bytes + binary.size)) {
                ss << std::setw(2) << std::setfill('0') << byte;
            }
            return ss.str();
        }
        case bsoncxx::type::k_bool:
            return val.view().get_bool().value ? "true" : "false";
        case bsoncxx::type::k_code:
            return string::to_string(val.view().get_code().code);
        case bsoncxx::type::k_codewscope:
            return "code={" + string::to_string(val.view().get_codewscope().code) + "}, scope={" +
                   to_json(val.view().get_codewscope().scope) + "}";
        case bsoncxx::type::k_date:
            return std::to_string(val.view().get_date().value.count());
        case bsoncxx::type::k_double:
            return std::to_string(val.view().get_double());
        case bsoncxx::type::k_null:
            return "null";
        case bsoncxx::type::k_undefined:
            return "undefined";
        case bsoncxx::type::k_timestamp:
            return "timestamp={" + std::to_string(val.view().get_timestamp().timestamp) +
                   "}, increment={" + std::to_string(val.view().get_timestamp().increment) + "}";
        case bsoncxx::type::k_regex:
            return "regex={" + string::to_string(val.view().get_regex().regex) + "}, options={" +
                   string::to_string(val.view().get_regex().options) + "}";
        case bsoncxx::type::k_minkey:
            return "minkey";
        case bsoncxx::type::k_maxkey:
            return "maxkey";
        case bsoncxx::type::k_decimal128:
            return val.view().get_decimal128().value.to_string();
        case bsoncxx::type::k_symbol:
            return string::to_string(val.view().get_symbol().symbol);
        case bsoncxx::type::k_dbpointer:
            return val.view().get_dbpointer().value.to_string();
        default:
            return "?";  // Match bsoncxx::to_string(bsoncxx::type) behavior.
    }
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/private/postlude.hh>
