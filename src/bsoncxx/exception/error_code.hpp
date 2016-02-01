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

#pragma once

#include <cstdint>
#include <system_error>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// @todo document this enum - including the values
///
enum class error_code : std::int32_t {
    k_cannot_append_key_in_sub_array = 1,
    k_cannot_close_array_in_sub_document,
    k_cannot_close_document_in_sub_array,
    k_cannot_perform_array_operation_on_document,
    k_cannot_perform_document_operation_on_array,
#define BSONCXX_ENUM(name, value) k_need_element_type_k_##name,
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    k_need_key,
    k_no_array_to_close,
    k_no_document_to_close,
    k_unmatched_key_in_builder,
    k_unset_element,
    k_json_parse_failure,
};

///
/// Get the error_category for exceptions originating from the bsoncxx library.
///
/// @return The bsoncxx error_category
///
BSONCXX_API const std::error_category& BSONCXX_CALL error_category();

///
/// Translate a bsoncxx::error_code into a std::error_code.
///
/// @param an error from bsoncxx
/// @return an error_code
///
BSONCXX_INLINE std::error_code make_error_code(error_code error) {
    return {static_cast<int>(error), error_category()};
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

namespace std {
// Specialize is_error_code_enum so we get simpler std::error_code construction
template <>
struct is_error_code_enum<bsoncxx::error_code> : public true_type {};
}  // namespace std
