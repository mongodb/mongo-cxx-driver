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

#include <cstdint>
#include <system_error>

#include <bsoncxx/exception/error_code-fwd.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

///
/// Enum representing the various error types that can occur while operating on BSON values.
///
enum class error_code : std::int32_t {
    /// A new key was appended while building a subarray.
    k_cannot_append_key_in_sub_array = 1,

    /// A subarray was closed while building a subdocument.
    k_cannot_close_array_in_sub_document,

    /// A subdocument was closed while building a subarray.
    k_cannot_close_document_in_sub_array,

    /// An array operation was performed while building a document.
    k_cannot_perform_array_operation_on_document,

    /// A document operation was performed while building an array.
    k_cannot_perform_document_operation_on_array,

    /// The underlying value does not have the requested type.
    /// @{
    k_need_element_type_k_double,
    k_need_element_type_k_string,
    k_need_element_type_k_document,
    k_need_element_type_k_array,
    k_need_element_type_k_binary,
    k_need_element_type_k_undefined,
    k_need_element_type_k_oid,
    k_need_element_type_k_bool,
    k_need_element_type_k_date,
    k_need_element_type_k_null,
    k_need_element_type_k_regex,
    k_need_element_type_k_dbpointer,
    k_need_element_type_k_code,
    k_need_element_type_k_symbol,
    k_need_element_type_k_codewscope,
    k_need_element_type_k_int32,
    k_need_element_type_k_timestamp,
    k_need_element_type_k_int64,
    k_need_element_type_k_decimal128,
    k_need_element_type_k_maxkey,
    k_need_element_type_k_minkey,
    /// @}

    /// No key was provided when one was needed.
    k_need_key,

    /// An array was closed while no array was open.
    k_no_array_to_close,

    /// A document was closed while no document was open.
    k_no_document_to_close,

    /// Attempted to view or extract a document when a key was still awaiting a matching value.
    k_unmatched_key_in_builder,

    /// An empty element was accessed.
    k_unset_element,

    /// A JSON document failed to parse.
    k_json_parse_failure,

    /// An Object ID string failed to parse.
    k_invalid_oid,

    /// This type is unused and deprecated.
    k_failed_converting_bson_to_json,

    /// A Decimal128 string failed to parse.
    k_invalid_decimal128,

    /// BSON data could not be processed, but no specific reason was available.
    k_internal_error,

    /// Failed to begin appending an array to a BSON document or array.
    k_cannot_begin_appending_array,

    /// Failed to begin appending a BSON document to a BSON document or array.
    k_cannot_begin_appending_document,

    /// Failed to complete appending an array to a BSON document or array.
    k_cannot_end_appending_array,

    /// Failed to complete appending a BSON document to a BSON document or array.
    k_cannot_end_appending_document,

    /// Invalid binary subtype.
    k_invalid_binary_subtype,

    /// Invalid type.
    k_invalid_bson_type_id,

    /// Failed to append a value of the given type.
    /// @{
    k_cannot_append_double,
    k_cannot_append_string,
    k_cannot_append_document,
    k_cannot_append_array,
    k_cannot_append_binary,
    k_cannot_append_undefined,
    k_cannot_append_oid,
    k_cannot_append_bool,
    k_cannot_append_date,
    k_cannot_append_null,
    k_cannot_append_regex,
    k_cannot_append_dbpointer,
    k_cannot_append_code,
    k_cannot_append_symbol,
    k_cannot_append_codewscope,
    k_cannot_append_int32,
    k_cannot_append_timestamp,
    k_cannot_append_int64,
    k_cannot_append_decimal128,
    k_cannot_append_maxkey,
    k_cannot_append_minkey,
    /// @}

    // Add new constant string message to error_code.cpp as well!
};

///
/// Get the error_category for exceptions originating from the bsoncxx library.
///
/// @return The bsoncxx error_category
///
BSONCXX_ABI_EXPORT_CDECL(const std::error_category&) error_category();

///
/// Translate a bsoncxx::v_noabi::error_code into a std::error_code.
///
/// @param error An error from bsoncxx
/// @return An error_code
///
inline std::error_code make_error_code(error_code error) {
    return {static_cast<int>(error), error_category()};
}

}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {

using ::bsoncxx::v_noabi::error_category;
using ::bsoncxx::v_noabi::make_error_code;

}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

namespace std {

///
/// Indicates @ref bsoncxx::v_noabi::error_code is eligible for `std::error_code` implicit
/// conversions.
///
template <>
struct is_error_code_enum<bsoncxx::v_noabi::error_code> : public true_type {};

}  // namespace std

///
/// @file
/// Provides @ref bsoncxx::v_noabi::error_code.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {

/// @ref bsoncxx::v_noabi::error_category()
const std::error_category& error_category();

/// @ref bsoncxx::v_noabi::make_error_code(v_noabi::error_code error)
std::error_code make_error_code(v_noabi::error_code error);

}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
