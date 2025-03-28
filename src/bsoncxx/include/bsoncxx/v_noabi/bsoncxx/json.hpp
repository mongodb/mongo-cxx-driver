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

#include <string>

#include <bsoncxx/json-fwd.hpp>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

///
/// An enumeration of the types of Extended JSON that the to_json function accepts
enum class ExtendedJsonMode : std::uint8_t {
    k_legacy,    ///< to produce Legacy Extended JSON
    k_canonical, ///< to produce Canonical Extended JSON
    k_relaxed,   ///< to produce Relaxed Extended JSON
};

///
/// Converts a BSON document to a JSON string, in extended format.
///
/// @param view
///   A valid BSON document or array.
/// @param mode
///   An optional JSON representation mode.
///
/// @throws bsoncxx::v_noabi::exception with error details if the conversion failed.
///
/// @returns An extended JSON string.
///
/// @{

BSONCXX_ABI_EXPORT_CDECL(std::string)
to_json(document::view view, ExtendedJsonMode mode = ExtendedJsonMode::k_legacy);

BSONCXX_ABI_EXPORT_CDECL(std::string)
to_json(array::view view, ExtendedJsonMode mode = ExtendedJsonMode::k_legacy);

/// @}
///

///
/// Constructs a new document::value from the provided JSON text.
///
/// @param json A string_view into a JSON document.
///
/// @returns A document::value if conversion worked.
///
/// @throws bsoncxx::v_noabi::exception with error details if the conversion failed.
///
BSONCXX_ABI_EXPORT_CDECL(document::value) from_json(stdx::string_view json);

///
/// Constructs a new document::value from the provided JSON text. This is the UDL version of
/// from_json().
///
/// @param json A string into a JSON document.
///
/// @param len The length of the JSON string. This is calculated automatically upon use of the UDL.
///
/// @returns A document::value if conversion worked.
///
/// @throws bsoncxx::v_noabi::exception with error details if the conversion failed.
///
BSONCXX_ABI_EXPORT_CDECL(document::value) operator"" _bson(char const* json, size_t len);

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {

using ::bsoncxx::v_noabi::from_json;
using ::bsoncxx::v_noabi::to_json;

using ::bsoncxx::v_noabi::operator"" _bson;

} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides utilities to convert between BSON and JSON representations.
///
