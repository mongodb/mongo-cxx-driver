// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <string>
#include <system_error>

#include <bson.h>

#include <bsoncxx/private/error_category.hpp>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/private/libbson_error.hpp>

namespace {

using namespace bsoncxx;

///
/// An error category for errors originating in bsoncxx.
///
class bsoncxx_error_category_impl final : public std::error_category {
   public:
    const char* name() const noexcept override {
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
        }
    }
};

///
/// An error category for libbson errors in the BSON_ERROR_JSON domain.
///

class libbson_json_error final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libbson json error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case BSON_JSON_ERROR_READ_CORRUPT_JS:
                return "attempted to parse corrupt javascript";
            case BSON_JSON_ERROR_READ_INVALID_PARAM:
                return "invalid parameter passed to json parser";
            case BSON_JSON_ERROR_READ_CB_FAILURE:
                return "reader callback failed";
            default:
                return "unknown libbson json error";
        }
    }
};

///
/// An error category for libbson errors in the BSON_ERROR_READER domain.
///
class libbson_reader_error final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libbson reader error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case BSON_ERROR_READER_BADFD:
                return "bad file descriptor";
            default:
                return "unknown libbson reader error";
        }
    }
};

///
/// An error category for errors from an unknown libbson domain.
///
class libbson_unknown_error final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libbson unknown error";
    }

    std::string message(int code) const noexcept override {
        return "unknown libbson error in unknown domain";
    }
};

}  // namespace

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

const std::error_category& bsoncxx_error_category() {
    static const bsoncxx_error_category_impl instance{};
    return instance;
}

const std::error_category& libbson_error_category(int domain) {
    switch (domain) {
        case BSON_ERROR_JSON:
            static const libbson_json_error json_category{};
            return json_category;
        case BSON_ERROR_READER:
            static const libbson_reader_error reader_category{};
            return reader_category;
        default:
            static const libbson_unknown_error unknown_category{};
            return unknown_category;
    }
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
