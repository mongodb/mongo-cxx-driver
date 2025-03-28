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

#include <string>

#include <bsoncxx/exception/error_code.hpp>

namespace bsoncxx {
namespace v_noabi {

namespace {

class error_category_impl final : public std::error_category {
   public:
    char const* name() const noexcept override {
        return "bsoncxx";
    }

    std::string message(int condition) const noexcept override {
        switch (static_cast<error_code>(condition)) {
            case error_code::k_cannot_append_key_in_sub_array:
                return "tried to append new key while in subarray";
            case error_code::k_cannot_close_array_in_sub_document:
                return "tried to close array while in subdocument";
            case error_code::k_cannot_close_document_in_sub_array:
                return "tried to close document while in a subarray";
            case error_code::k_cannot_perform_array_operation_on_document:
                return "tried to operate on document, but this is an array";
            case error_code::k_cannot_perform_document_operation_on_array:
                return "tried to operate on array, but this is a document";
#define BSONCXX_ENUM(name, value)                  \
    case error_code::k_need_element_type_k_##name: \
        return {"expected element type k_" #name};
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
            case error_code::k_need_key:
                return "expected a key but found none";
            case error_code::k_no_array_to_close:
                return "tried to close array, but no array was opened";
            case error_code::k_no_document_to_close:
                return "tried to close document, but no document was opened";
            case error_code::k_unmatched_key_in_builder:
                return "can't convert builder to a valid view: unmatched key";
            case error_code::k_unset_element:
                return "unset document::element";
            case error_code::k_invalid_oid:
                return "could not parse Object ID string";
            case error_code::k_failed_converting_bson_to_json:
                return "could not convert document to JSON";
            case error_code::k_json_parse_failure:
                return "could not parse JSON document";
            case error_code::k_invalid_decimal128:
                return "could not parse Decimal128 string";
            case error_code::k_internal_error:
                return "could not process bson data";
            case error_code::k_cannot_begin_appending_array:
                return "unable to begin appending an array";
            case error_code::k_cannot_begin_appending_document:
                return "unable to begin appending a document";
            case error_code::k_cannot_end_appending_array:
                return "tried to complete appending an array, but overflowed";
            case error_code::k_cannot_end_appending_document:
                return "tried to complete appending an document, but overflowed";
            case error_code::k_invalid_binary_subtype:
                return "invalid BSON binary subtype";
            case error_code::k_invalid_bson_type_id:
                return "invalid BSON type identifier";
#define BSONCXX_ENUM(name, value)            \
    case error_code::k_cannot_append_##name: \
        return {"unable to append " #name};
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
            case error_code::k_invalid_vector:
                return "invalid BSON vector";
            case error_code::k_vector_too_large:
                return "BSON vector too large";
            case error_code::k_vector_out_of_range:
                return "BSON vector access out of range";
            default:
                return "unknown bsoncxx error code";
        }
    }
};

} // namespace

std::error_category const& error_category() {
    static error_category_impl const instance{};
    return instance;
}

} // namespace v_noabi
} // namespace bsoncxx
