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

// This is what the user maintains:
#define MY_ERRORS                                                                       \
 X(my_bad,                      "comment about what I did")                             \
 X(my_also_bad,                 "a comment about what else happened")                   \
 X(merely_unfortunate,          "all of them victims... all of them martyrs")           \
 X(invalid_parameter,           "that parameter may not walk again")
// end x-macro table 

/******************************************************************* implementation details: */
// Generate the error_code table:
#define X(my_field_enum_label, my__) my_field_enum_label,
enum struct my_error_codes : std::int32_t
{
 INITIAL_ENTRY__ = 0,
 //////////////////////
 MY_ERRORS
 //////////////////////
 MAX_ENTRY__
};
#undef X

/* JFW: C++11 doesn't have "constinit inline"; may want to use a singleton function: */
#define X(my__, my_field_error_msg) my_field_error_msg,
static const char* const my_error_code_msgs[] =
{
 "__invalid_entry__",
 MY_ERRORS
};
#undef X

/**** END implementation details, back to our regularly scheduled program! */

struct my_error_category final : std::error_category
{
 const char *name() const noexcept override { return "my_company"; }

 std::string message(int code) const noexcept override;
};

inline std::string my_error_category::message(int ec) const noexcept
{
 if(static_cast<int>(my_error_codes::INITIAL_ENTRY__) >= ec
    || ec >= static_cast<int>(my_error_codes::MAX_ENTRY__))
  return "invalid value";

 return my_error_code_msgs[ec];
}

const std::error_category& error_category();

inline std::error_code make_error_code(my_error_codes error) {
    return {static_cast<int>(error), error_category()};
}

inline const std::error_category& error_category() {
    static const struct my_error_category my_category {};
    return my_category;
}

namespace std {
template <>
struct is_error_code_enum<my_error_codes> : public true_type {};
}  // namespace std

/******************************************************************/

namespace mongocxx { inline namespace v_noabi {

class exception : public std::system_error {
    using system_error::system_error;
};

}} // namespace mongocxx::inline namespace v_noabi

template <std::int32_t errc>
struct my_logic_error : mongocxx::exception
{
 my_logic_error(const std::string& what)
  : mongocxx::exception(static_cast<my_error_codes>(errc), what)
 {}

 my_logic_error(const char *what)
  : mongocxx::exception(static_cast<my_error_codes>(errc), what)
 {}
};

#define X_REPASTE(field_name) field_name
#define X(my_field_enum_label, my__) using exception_ ## my_field_enum_label = my_logic_error<static_cast<std::int32_t>( my_error_codes::X_REPASTE(my_field_enum_label))>; 
MY_ERRORS
#undef X
#undef X_REPASTE

#undef MY_ERRORS

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
