// Copyright 2016-present MongoDB Inc.
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

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/collection.hpp>
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
// Converts a hexadecimal string to an string of bytes.
//
// This function assumes that `hex` has an even length and that all characters in the string are
// valid hexadecimal digits.
//
std::basic_string<std::uint8_t> convert_hex_string_to_bytes(bsoncxx::stdx::string_view hex);

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

///
/// Get replica set name, or empty string.
///
std::string replica_set_name(const client& client);

///
/// Determines if the server is a replica set member.
///
bool is_replica_set(const client& client);

///
/// Returns "standalone", "replicaset", or "sharded".
///
std::string get_topology(const client& client);

///
/// Parses a JSON file at a given path and return it as a BSON document value.
///
/// Returns none if the path is not found.
///
stdx::optional<bsoncxx::document::value> parse_test_file(std::string path);

//
// Determines whether or not the given client supports the collation feature, by running the
// "isMaster" command.
//
// Throws mongocxx::operation_exception if the operation fails, or the server reply is malformed.
//
bool supports_collation(const client& client);

using item_t = std::pair<stdx::optional<stdx::string_view>, bsoncxx::types::value>;
using xformer_t = std::function<stdx::optional<item_t>(item_t, bsoncxx::builder::basic::array*)>;

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
//   The bsoncxx::builder::basic::array* argument of fcn is used for storing non-owned values that
//   would no longer be alive when fcn returns. Any non-owned values returned from fcn should be
//   appended to the builder argument so that it remains alive for the duration of the
//   transform_document call.
//
// @return The new document that was built.
//
// @throws a logic_error{error_code::k_invalid_parameter} if fcn returns no key when a key is
// passed in.
//
bsoncxx::document::value transform_document(bsoncxx::document::view view, const xformer_t& fcn);

double as_double(bsoncxx::types::value value);

bool is_numeric(bsoncxx::types::value value);

enum class match_action { k_skip, k_proceed, k_not_equal };
using match_visitor =
    std::function<match_action(bsoncxx::stdx::string_view key,
                               bsoncxx::stdx::optional<bsoncxx::types::value> main,
                               bsoncxx::types::value pattern)>;

bool matches(bsoncxx::types::value main,
             bsoncxx::types::value pattern,
             bsoncxx::stdx::optional<match_visitor> visitor_fn = {});

bool matches(bsoncxx::document::view doc,
             bsoncxx::document::view pattern,
             bsoncxx::stdx::optional<match_visitor> visitor_fn = {});

std::string tolowercase(stdx::string_view view);

void check_outcome_collection(mongocxx::collection* coll, bsoncxx::document::view expected);

//
// Require a topology that supports sessions (a post-3.6 replica set or cluster of them).
//
// @param client
//   A connected client.
//
// @return Whether sessions are supported by the client's topology.
//
bool server_has_sessions(const client& conn);

}  // namespace test_util

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
