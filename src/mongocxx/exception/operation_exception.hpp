// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/stdx.hpp>

#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/string/to_string.hpp>

#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx { inline namespace v_noabi { namespace bson_util {

inline void set_bson_error_message(bson_error_t* error, const char* msg) {
    bson_strncpy(error->message,
                 msg,
                 std::min(strlen(msg) + 1, static_cast<size_t>(BSON_ERROR_BUFFER_SIZE)));
}

inline void make_unknown_bson_error(bson_error_t* error) {
    // CDRIVER-3524 Zero these out since we don't have them.
    error->code = 0;
    error->domain = 0;
    set_bson_error_message(error, "unknown error");
}

}}} // namespace mongocxx::inline v_noabi::bson_util

namespace mongocxx { inline namespace v_noabi {

enum struct server_error_code : std::int32_t
{
 INITIAL_ENTRY__ = 0,
 //////////////////////
 operation_error,
 //////////////////////
 MAX_ENTRY__
};

//
// An error_category for codes returned by the server.
//
struct server_error_category final : std::error_category {
    const char* name() const noexcept override { return "mongodb"; }

    std::string message(int) const noexcept override { return "server error"; }
};

inline const std::error_category& server_error_category() {
    static const class server_error_category category {};
    return category;
}

inline std::error_code make_error_code(mongocxx::server_error_code error) {
    return { static_cast<int>(error), server_error_category() };
}

inline std::error_code make_error_code(const ::bson_error_t& error) { 
    // note that error.domain is ignored:
    return std::error_code { static_cast<int>(error.code), server_error_category() };
}

inline std::error_code make_error_code(int code, int) {
    // Domain is ignored. We simply issue the code.
    return {code, server_error_category()};
}

inline void make_generic_bson_error(bson_error_t* error) {
    // CDRIVER-3524 Zero these out since we don't have them.
    error->code = 0;
    error->domain = 0;
    mongocxx::bson_util::set_bson_error_message(error, "unknown error");
}

}} // namespace mongocxx::inline v_noabi


namespace mongocxx { inline namespace v_noabi {
///
/// Class representing an exception received from a MongoDB server.  It includes the server-provided
/// error code, if one was available.
///
/// @see mongocxx::exception
///
class operation_exception : public mongocxx::exception {
   public:
    using mongocxx::exception::exception;

    operation_exception(const ::bson_error_t& bson_error)
     : mongocxx::exception { mongocxx::make_error_code(bson_error) }
/* JFW: what would this even mean?
       _raw_server_error { mongocxx::make_error_code(bson_error.code) } 
*/
    {}

    operation_exception(const ::bson_error_t& error, bsoncxx::document::value&& raw_server_error)
     : mongocxx::exception(mongocxx::make_error_code(error)), 
       _raw_server_error { std::move(raw_server_error) }
    {}

    ///
    /// Constructs a new operation exception.
    ///
    /// @param ec
    ///   The error code associated with this exception.
    /// @param raw_server_error
    ///   The optional raw bson error document to be associated with this exception.
    /// @param what_arg
    ///   A message to be returned by `what`.
    ///
    operation_exception(std::error_code ec,
                        bsoncxx::document::value&& raw_server_error,
                        std::string what_arg)
    : exception(ec, what_arg), _raw_server_error{std::move(raw_server_error)} 
    {}

    ///
    /// @{
    ///
    /// The optional raw bson error document from the server.
    ///
    /// @returns The raw server error, if it is available.
    ///
    const stdx::optional<bsoncxx::document::value>& raw_server_error() const
	{ return _raw_server_error; }
    ///
    /// @}
    ///

    ///
    /// Checks for the specified label in a operation exception.
    ///
    /// @param label
    ///   A string of the label to search for
    ///
    /// @returns True if the label is found in the operation exception, and false otherwise.
    ///
    bool has_error_label(stdx::string_view label) const {
	    if (!_raw_server_error) {
       		 return false;
    	}

    	libbson::scoped_bson_t error(_raw_server_error->view());
    	std::string label_str{label.data(), label.size()};
    	return libmongoc::error_has_label(error.bson(), label_str.c_str());
    }

   public:
    static void make_bson_error(bson_error_t* error, const mongocxx::operation_exception& e) {
    	// No way to get the domain back out of the exception, so zero out.
    	error->code = static_cast<uint32_t>(e.code().value());
    	error->domain = 0;
    	mongocxx::bson_util::set_bson_error_message(error, e.what());
    }

   private:
    stdx::optional<bsoncxx::document::value> _raw_server_error;
};

///
/// Class representing an exception during authentication.
///
/// @see mongocxx::operation_exception
///
class MONGOCXX_API authentication_exception : public operation_exception {
   public:
    using operation_exception::operation_exception;
};

///
/// Class representing an exception during a bulk write operation.
///
/// @see mongocxx::operation_exception
///
class MONGOCXX_API bulk_write_exception : public operation_exception {
   public:
    using operation_exception::operation_exception;
};

///
/// Class representing an exception during a query operation.
///
/// @see mongocxx::operation_exception
///
class MONGOCXX_API query_exception : public operation_exception {
   public:
    using operation_exception::operation_exception;
};

///
/// Class representing an exception during a write operation.
///
/// @see mongocxx::operation_exception
///
class MONGOCXX_API write_exception : public operation_exception {
   public:
    using operation_exception::operation_exception;
};

}}  // namespace mongocxx::inline v_noabi

#include <mongocxx/config/postlude.hpp>

