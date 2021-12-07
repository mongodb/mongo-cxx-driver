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

// This is what the user maintains:
#define MONGOCXX_ERRORS                                                                       	\
 MONGOCXX_X(logic_error,		"logic_error")						\
 MONGOCXX_X(invalid_parameter,          "that parameter may not walk again")
// end x-macro table 

}} // namespace mongocxx::inline v_noabi

/******************************************************************* implementation details: */
namespace mongocxx { inline namespace v_noabi { 

// Generate the error_code table:
#define MONGOCXX_X(mongocxx_field_enum_label, mongocxx__) mongocxx_field_enum_label,
enum struct Zerror_code : std::int32_t
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
static const char* const Zerror_code_msgs[] =
{
 "__invalid_entry__",
 MONGOCXX_ERRORS
};
#undef MONGOCXX_X

}} // namespace mongocxx::inline v_noabi

/**** END implementation details, back to our regularly scheduled program! */

struct my_error_category final : std::error_category
{
 const char *name() const noexcept override { return "mongocxx"; }

 std::string message(int code) const noexcept override;
};

inline std::string my_error_category::message(int ec) const noexcept
{
 if(static_cast<int>(mongocxx::v_noabi::Zerror_code::INITIAL_ENTRY__) >= ec
    || ec >= static_cast<int>(mongocxx::v_noabi::Zerror_code::MAX_ENTRY__))
  return "invalid value";

 return mongocxx::Zerror_code_msgs[ec];
}

const std::error_category& error_category();

inline std::error_code make_error_code(mongocxx::Zerror_code error) {
    return {static_cast<int>(error), error_category()};
}

inline const std::error_category& error_category() {
    static const struct my_error_category my_category {};
    return my_category;
}

namespace std {
template <>
struct is_error_code_enum<mongocxx::Zerror_code> : public true_type {};
}  // namespace std

/******************************************************************/

namespace mongocxx { inline namespace v_noabi {

class exception : public std::system_error {
    using system_error::system_error;
};

/* JFW: generalized mongocxx exception with both an error code and a customizable
message-- for rubber-stamping out new error types: */

namespace detail {

template <std::int32_t errc>
struct general_error : mongocxx::exception
{
 general_error(const std::string& what)
  : mongocxx::exception(static_cast<mongocxx::Zerror_code>(errc), what)
 {}

 general_error(const char *what)
  : mongocxx::exception(static_cast<mongocxx::Zerror_code>(errc), what)
 {}

/* JFW: note: we should entirely deprecate these-- in the meantime, they're useful for tracking what
we have during development: */
 general_error()
  : mongocxx::exception(static_cast<mongocxx::Zerror_code>(errc), "error")
 {}

 general_error(const mongocxx::Zerror_code errc_, const char *what) : mongocxx::exception(static_cast<mongocxx::Zerror_code>(errc_), what)
 {}

};

} // namespace detail

}} // namespace mongocxx::inline namespace v_noabi

namespace mongocxx { namespace v_noabi { 

//JFW: using logic_error = mongocxx::v_noabi::detail::general_error<(int32_t)mongocxx::v_noabi::Zerror_code::logic_error>;

#define MONGOCXX_X_REPASTE(field_name) field_name

#define MONGOCXX_X(mongocxx_field_enum_label, my__) using MONGOCXX_X_REPASTE(mongocxx_field_enum_label) = mongocxx::v_noabi::detail::general_error<static_cast<std::int32_t>( mongocxx::v_noabi::Zerror_code::MONGOCXX_X_REPASTE(mongocxx_field_enum_label))>; 
MONGOCXX_ERRORS
#undef MONGOCXX_X
#undef MONGOCXX_X_REPASTE

#undef MONGOCXX_ERRORS

}} // JFW

/*********************************************************************/

/* JFW:
#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// A class to be used as the base class for all mongocxx exceptions.
///
class MONGOCXX_API exception : public std::system_error {
    using system_error::system_error;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
*/
