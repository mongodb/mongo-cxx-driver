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
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/options/client.hpp>

#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/catch.hh>

namespace mongocxx {
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
// Returns 'true' if the server version for the default client is at least 'version',
// returns 'false' otherwise.
//
bool newer_than(std::string version);

//
// Converts a hexadecimal string to an string of bytes.
//
// This function assumes that `hex` has an even length and that all characters in the string are
// valid hexadecimal digits.
//
std::basic_string<std::uint8_t> convert_hex_string_to_bytes(bsoncxx::stdx::string_view hex);

//
// Adds server API options to passed-in options if MONGODB_API_VERSION
// environment variable is set.
//
// @param opts
//   The options to add server API options to.
//
// @return The new options with appended server API options.
//
options::client add_test_server_api(options::client opts = {});

//
// Determines the max wire version associated with the default client, by running the "hello"
// command.
//
// Throws mongocxx::operation_exception if the operation fails, or the server reply is malformed.
//
std::int32_t get_max_wire_version();

///
/// Determines the server version number by running "serverStatus" with the default client.
///
std::string get_server_version();

///
/// Determines the setting of all server parameters by running "getParameter, *" with the default client.
///
bsoncxx::document::value get_server_params();

///
/// Returns the replica set name or an empty string using the default client.
///
std::string replica_set_name();

///
/// Determines if the server is a replica set member using the default client.
///
bool is_replica_set();

///
/// Determines if the server is a sharded cluster member using the default client.
///
bool is_sharded_cluster();

///
/// Returns "standalone", "replicaset", or "sharded" using the default client.
///
std::string get_topology();

///
/// Returns the "host" field of the config.shards collection using the default client.
///
std::string get_hosts();

///
/// Parses a JSON file at a given path and return it as a BSON document value.
///
/// Returns none if the path is not found.
///
bsoncxx::stdx::optional<bsoncxx::document::value> parse_test_file(std::string path);

using item_t = std::pair<bsoncxx::stdx::optional<bsoncxx::stdx::string_view>, bsoncxx::types::bson_value::view>;
using xformer_t = std::function<bsoncxx::stdx::optional<item_t>(item_t, bsoncxx::builder::basic::array*)>;

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
//   value pair returned will be added to the document. To leave the element as-is, simply
//   return the same key and value passed into the function. If no key is returned, an exception
//   will be thrown.
//
//   For array elements, only the value will be passed in (since there is no key). If nothing is
//   returned, the element will be removed from the array. Otherwise, the value returned will be
//   added to the array. To leave the element as-is, simply return the value passed into the
//   function.
//
//   The bsoncxx::builder::basic::array* argument of fcn is used for storing non-owned values
//   that would no longer be alive when fcn returns. Any non-owned values returned from fcn
//   should be appended to the builder argument so that it remains alive for the duration of the
//   transform_document call.
//
// @return The new document that was built.
//
// @throws a logic_error{error_code::k_invalid_parameter} if fcn returns no key when a key is
// passed in.
//
bsoncxx::document::value transform_document(bsoncxx::document::view view, xformer_t const& fcn);

double as_double(bsoncxx::types::bson_value::view value);

bool is_numeric(bsoncxx::types::bson_value::view value);

enum class match_action { k_skip, k_proceed, k_not_equal };
using match_visitor = std::function<match_action(
    bsoncxx::stdx::string_view key,
    bsoncxx::stdx::optional<bsoncxx::types::bson_value::view> main,
    bsoncxx::types::bson_value::view pattern)>;

bool matches(
    bsoncxx::types::bson_value::view main,
    bsoncxx::types::bson_value::view pattern,
    match_visitor visitor_fn = {});

bool matches(bsoncxx::document::view doc, bsoncxx::document::view pattern, match_visitor visitor_fn = {});

#define REQUIRE_BSON_MATCHES(_main, _pattern)                                                      \
    do {                                                                                           \
        if (!test_util::matches((_main), (_pattern))) {                                            \
            FAIL("Mismatch, expected " << to_json((_pattern)) << ", actual " << to_json((_main))); \
        }                                                                                          \
    } while (0)

#define REQUIRE_BSON_MATCHES_V(_main, _pattern, _visit)                                            \
    do {                                                                                           \
        if (!test_util::matches((_main), (_pattern), (_visit))) {                                  \
            FAIL("Mismatch, expected " << to_json((_pattern)) << ", actual " << to_json((_main))); \
        }                                                                                          \
    } while (0)

std::string tolowercase(bsoncxx::stdx::string_view view);

void check_outcome_collection(mongocxx::collection* coll, bsoncxx::document::view expected);

template <typename Container>
auto size(Container c) -> decltype(std::distance(std::begin(c), std::end(c))) {
    return std::distance(std::begin(c), std::end(c));
}

//
// Require a topology that supports sessions (a post-3.6 replica set or cluster of them).
//
// @return Whether sessions are supported by the default client's topology.
//
bool server_has_sessions_impl();

#define SERVER_HAS_SESSIONS_OR_SKIP()                       \
    if (!mongocxx::test_util::server_has_sessions_impl()) { \
        SKIP("server does not support session");            \
    } else                                                  \
        ((void)0)

#if defined(MONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION)
enum struct cseeos_result {
    enable,
    skip,
    fail,
};

cseeos_result client_side_encryption_enabled_or_skip_impl();

#define CLIENT_SIDE_ENCRYPTION_ENABLED_OR_SKIP()                                      \
    if (1) {                                                                          \
        switch (mongocxx::test_util::client_side_encryption_enabled_or_skip_impl()) { \
            case mongocxx::test_util::cseeos_result::enable:                          \
                break;                                                                \
            case mongocxx::test_util::cseeos_result::skip:                            \
                SKIP("CSE environemnt variables not set");                            \
                break;                                                                \
            case mongocxx::test_util::cseeos_result::fail:                            \
                FAIL("One or more CSE environment variable is missing");              \
                break;                                                                \
        }                                                                             \
    } else                                                                            \
        ((void)0)
#else
#define CLIENT_SIDE_ENCRYPTION_ENABLED_OR_SKIP() SKIP("linked libmongoc does not support client side encryption")
#endif // defined(MONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION)

std::string getenv_or_fail(const std::string env_name);

} // namespace test_util
} // namespace mongocxx
