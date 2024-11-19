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

#include <memory>
#include <vector>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/private/b64_ntop.hh>
#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
namespace v_noabi {

namespace {

void bson_free_deleter(std::uint8_t* ptr) {
    bson_free(ptr);
}

std::string to_json_helper(document::view view, char* (*converter)(const bson_t*, std::size_t*)) {
    bson_t bson;

    if (!bson_init_static(&bson, view.data(), view.length())) {
        throw exception(error_code::k_failed_converting_bson_to_json);
    }

    size_t size;
    auto result = converter(&bson, &size);

    if (!result) {
        throw exception(error_code::k_failed_converting_bson_to_json);
    }

    const auto deleter = [](char* result) { bson_free(result); };
    const std::unique_ptr<char[], decltype(deleter)> cleanup(result, deleter);

    return {result, size};
}

}  // namespace

std::string to_json(document::view view, ExtendedJsonMode mode) {
    switch (mode) {
        case ExtendedJsonMode::k_legacy:
            return to_json_helper(view, bson_as_legacy_extended_json);

        case ExtendedJsonMode::k_relaxed:
            return to_json_helper(view, bson_as_relaxed_extended_json);

        case ExtendedJsonMode::k_canonical:
            return to_json_helper(view, bson_as_canonical_extended_json);
    }

    BSONCXX_UNREACHABLE;
}

std::string to_json(array::view view, ExtendedJsonMode mode) {
    switch (mode) {
        case ExtendedJsonMode::k_legacy:
            return to_json_helper(view, bson_array_as_legacy_extended_json);

        case ExtendedJsonMode::k_relaxed:
            return to_json_helper(view, bson_array_as_relaxed_extended_json);

        case ExtendedJsonMode::k_canonical:
            return to_json_helper(view, bson_array_as_canonical_extended_json);
    }

    BSONCXX_UNREACHABLE;
}

document::value from_json(stdx::string_view json) {
    bson_error_t error;
    bson_t* result = bson_new_from_json(reinterpret_cast<const uint8_t*>(json.data()),
                                        static_cast<std::int32_t>(json.size()),
                                        &error);

    if (!result)
        throw exception(error_code::k_json_parse_failure, error.message);

    std::uint32_t length;
    std::uint8_t* buf = bson_destroy_with_steal(result, true, &length);

    return document::value{buf, length, bson_free_deleter};
}

document::value operator"" _bson(const char* str, size_t len) {
    return from_json(stdx::string_view{str, len});
}

}  // namespace v_noabi
}  // namespace bsoncxx
