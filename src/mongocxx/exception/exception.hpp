// Copyright 2014-present MongoDB Inc.
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
#include <cstdint>
#include <system_error>

namespace mongocxx { inline namespace v_noabi {

/* This is what the user maintains for any general (i.e. non-customized) exceptions:
Note that the order of appearance determines the error code; you should add new errors to the
end.
*/
#define MONGOCXX_ERRORS \
 MONGOCXX_X(logic_error,		"logic_error")						\
 MONGOCXX_X(invalid_client_object, 	"invalid use of default constructed or moved-from mongocxx::client object") 	\
 MONGOCXX_X(invalid_collection_object,	"invalid use of default constructed or moved-from mongocxx::collection object") \
 MONGOCXX_X(invalid_database_object,	"invalid use of default constructed or moved-from mongocxx::database object") 	\
 MONGOCXX_X(invalid_parameter, "an invalid or out-of-bounds parameter was provided") \
 MONGOCXX_X(ssl_not_supported, "SSL support not available") \
 MONGOCXX_X(unknown_read_concern, "invalid attempt to set an unknown read concern level") \
 MONGOCXX_X(unknown_write_concern, "invalid attempt to set an unknown write concern level") \
 MONGOCXX_X(cannot_recreate_instance, "cannot create a mongocxx::instance object if one has already been created") \
 MONGOCXX_X(server_response_malformed, "the response from the server was malformed") \
 MONGOCXX_X(invalid_uri, "an invalid MongoDB URI was provided") \
 MONGOCXX_X(invalid_gridfs_bucket_object, "invalid use of default constructed or moved-from mongocxx::gridfs::bucket object") \
 MONGOCXX_X(invalid_gridfs_uploader_object, "invalid use of default constructed or moved-from mongocxx::gridfs::uploader object") \
 MONGOCXX_X(invalid_gridfs_downloader_object, "invalid use of default constructed or moved-from mongocxx::gridfs::downloader object") \
 MONGOCXX_X(gridfs_stream_not_open, "a mongocxx::gridfs::uploader object was not open for writing, or a mongocxx::gridfs::downloader object was not open for reading") \
 MONGOCXX_X(gridfs_upload_requires_too_many_chunks, "a mongocxx::gridfs::uploader object has exceeded the maximum number of allowable GridFS chunks when attempting to upload the requested file") \
 MONGOCXX_X(gridfs_file_not_found, "the requested GridFS file was not found") \
 MONGOCXX_X(gridfs_file_corrupted, "a GridFS file being operated on was discovered to be corrupted") \
 MONGOCXX_X(instance_destroyed, "the mongocxx instance has been destroyed") \
 MONGOCXX_X(cannot_create_session, "failed to create a client session") \
 MONGOCXX_X(invalid_session, "an invalid client session was provided") \
 MONGOCXX_X(invalid_transaction_options_object, "an invalid transactions options object was provided") \
 MONGOCXX_X(create_resource_fail, "could not create resource") \
////
// end x-macro table 

}} // namespace mongocxx::inline v_noabi

/******************************************************************* implementation details: */
namespace mongocxx { inline namespace v_noabi { 

// Generate the error_code table:
#define MONGOCXX_X(mongocxx_field_enum_label, mongocxx__) mongocxx_field_enum_label,
enum struct error_code : std::int32_t
{
 INITIAL_ENTRY__ = 0,
 //////////////////////
 MONGOCXX_ERRORS
 //////////////////////
 MAX_ENTRY__
};
#undef MONGOCXX_X

/* JFW: C++11 doesn't have "constinit inline"; may want to use a singleton function: */
#define MONGOCXX_X(mongocxx__, field_error_msg) field_error_msg,
static const char* const error_code_msgs[] =
{
 "__invalid_entry__",
 MONGOCXX_ERRORS
};
#undef MONGOCXX_X

struct error_category final : std::error_category
{
 const char *name() const noexcept override { return "mongocxx"; }

 std::string message(int ec) const noexcept override
  {
	if(static_cast<int>(mongocxx::error_code::INITIAL_ENTRY__) >= ec
	    || ec >= static_cast<int>(mongocxx::error_code::MAX_ENTRY__))
	 return "invalid value";
	
	return mongocxx::error_code_msgs[ec];
  }
};

inline const std::error_category& error_category() {
    static const struct error_category category {};
    return category;
}

// The names can be confusing, but this is "our" error_code to std::error_code:
inline std::error_code make_error_code(mongocxx::error_code error) {
    return {static_cast<int>(error), error_category()};
}

}} // namespace mongocxx::inline v_noabi

namespace std {
template <>
struct is_error_code_enum<mongocxx::error_code> : public true_type {};
}  // namespace std

/******************************************************************/

namespace mongocxx { inline namespace v_noabi { 

class exception : public std::system_error {
    using system_error::system_error;
};

/* JFW: generalized mongocxx exception with both an error code and a customizable
message-- for rubber-stamping out new error types: 
NOTE: There's deliberately no error-code-only ctor, as we want a type-to-code mapping
that's automatic: 
*/
namespace detail {

/* A base for most mongocxx exceptions, specifically those which map to a mongocxx::system_error (as
distinct from bsoncxx::system_error, for instance).

	Not meant to be instanced directly.
*/
template <std::int32_t err_code>
struct mongocxx_general_error : mongocxx::exception
{
 std::int32_t error_code = err_code; // JFW: TODO: system_error<> already contains an ec, make this go away :>

 mongocxx_general_error()
  : mongocxx::exception(static_cast<mongocxx::v_noabi::error_code>(err_code), "error")
 {}

 mongocxx_general_error(const std::string& what)
  : mongocxx::exception(static_cast<mongocxx::v_noabi::error_code>(err_code), what)
 {}

 mongocxx_general_error(const char *what)
  : mongocxx::exception(static_cast<mongocxx::v_noabi::error_code>(err_code), what)
 {}

 // JFW: TODO: we do NOT want to keep this around: it's to facilitate tracking down mongocxx::throw_exception<>: 
 mongocxx_general_error(const std::int32_t err_c, const char *what)
  : error_code { err_c },
    mongocxx::exception(static_cast<mongocxx::v_noabi::error_code>(err_c), what)
 {}

};

} // namespace detail
}} // namespace mongocxx::inline namespace v_noabi

/* This section is where "general" non-customized exceptions are "rubber-stamped" out. If you exception type doesn't need
any special behavior or rules, it should be put in the table above and then generated here: 

What we generate looks ABOUT like:
using logic_error = mongocxx::v_noabi::detail::mongocxx_general_error<(int32_t)mongocxx::v_noabi::error_code::logic_error>;

*/
namespace mongocxx { inline namespace v_noabi { 

#define MONGOCXX_X_REPASTE(field_name) field_name

#define MONGOCXX_X(mongocxx_field_enum_label, mongocxx__) using MONGOCXX_X_REPASTE(mongocxx_field_enum_label) = mongocxx::v_noabi::detail::mongocxx_general_error<static_cast<std::int32_t>( mongocxx::v_noabi::error_code::MONGOCXX_X_REPASTE(mongocxx_field_enum_label))>; 
MONGOCXX_ERRORS
#undef MONGOCXX_X
#undef MONGOCXX_X_REPASTE

#undef MONGOCXX_ERRORS

}} // namespace mongocxx::inline v_noabi

/* This is where customized types that don't easily fit the stamped-out pattern go:
Note that nothing prevents further generators from being added as-needed: */
namespace mongocxx { inline namespace v_noabi {

// Your special types go here!

}} // namespace mongocxx::inline v_noabi

/*********************************************************************/

