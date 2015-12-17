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

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

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
    k_unset_element
};

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
