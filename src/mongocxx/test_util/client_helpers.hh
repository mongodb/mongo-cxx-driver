// Copyright 2016 MongoDB Inc.
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
#include <functional>
#include <string>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;

namespace test_util {

//
// Compares to version number strings (i.e. positive integers separated by periods). Comparisons are
// done to the lesser precision of the two versions. For example, 3.2 is considered equal to 3.2.11,
// whereas 3.2.0 is considered less than 3.2.11.
//
// Returns a positive integer if version1 is greater than version2, a negative integer if version1
// is less than version2, and 0 if version1 is equal to version2.
//
// Throws a std::invalid_argument exception if either version string is not a valid version number.
//
std::int32_t compare_versions(std::string version1, std::string version2);

//
// Determines the max wire version associated with the given client, by running the "isMaster"
// command.
//
// Throws mongocxx::operation_exception if the operation fails, or the server reply is malformed.
//
std::int32_t get_max_wire_version(const client& client);

///
/// Determines the server version number by running "serverStatus".
///
std::string get_server_version(const client& client);

//
// Determines whether or not the given client supports the collation feature, by running the
// "isMaster" command.
//
// Throws mongocxx::operation_exception if the operation fails, or the server reply is malformed.
//
bool supports_collation(const client& client);

// item_t -- document items have keys, array items don't
using item_t = std::pair<stdx::optional<stdx::string_view>, bsoncxx::types::value>;

// xformer_t -- if return value disengaged, item should be omitted from
// container, otherwise, transformed item should be used in place of the
// original.  To use as-is, return the argument (i.e. like an identity
// function).
using xformer_t = std::function<stdx::optional<item_t>(item_t)>;

//
// Transforms a document and returns a copy of it.
//
// @param view
//   The document to transform.
//
// @param fcn
//   The function to apply to each element of the document.
//
//   For document elements, the element's key and value will be passed into the function. If
//   nothing is returned, the element will be removed from the document. Otherwise, the key and
//   value pair returned will be added to the document. To leave the element as-is, simply return
//   the same key and value passed into the function. If no key is returned, an exception will be
//   thrown.
//
//   For array elements, only the value will be passed in (since there is no key). If nothing is
//   returned, the element will be removed from the array. Otherwise, the value returned will be
//   added to the array. To leave the element as-is, simply return the value passed into the
//   function.
//
// @return The new document that was built.
//
// @throws if fcn returns no key when a key is passed in.
bsoncxx::document::value transform_document(bsoncxx::document::view view, const xformer_t& fcn);
}  // namespace test_util
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
