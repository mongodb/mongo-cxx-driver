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

#include <string>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// Converts a BSON document to a JSON string.
///
/// @param view
///   A valid BSON document.
///
/// @returns A JSON string.
///
BSONCXX_API std::string BSONCXX_CALL to_json(document::view view);

///
/// Converts an element (key-value pair) to a JSON key-value pair.
/// This is not actually a valid JSON document, but it can be useful for debugging.
///
/// @param element
///   A BSON element.
///
/// @returns A JSON key-value pair.
///
BSONCXX_API std::string BSONCXX_CALL to_json(document::element element);

///
/// Converts a BSON value to its JSON string representation.
/// This is not actually a valid JSON document, but it can be useful for debugging.
///
/// @param element
///   A BSON element.
///
/// @returns A JSON value.
///
BSONCXX_API std::string BSONCXX_CALL to_json(types::value value);

///
/// Constructs a new document::value from the provided JSON text
///
/// @param 'json'
///  A string_view into a JSON document
///
/// @returns A document::value if conversion worked.
///
/// @throws A bsoncxx::exception containing error details, if the
/// conversion failed.
///
BSONCXX_API document::value BSONCXX_CALL from_json(stdx::string_view json);

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
