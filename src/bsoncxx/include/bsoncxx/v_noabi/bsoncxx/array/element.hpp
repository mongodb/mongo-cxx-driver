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

#include <cstddef>
#include <cstdint>

#include <bsoncxx/array/element-fwd.hpp>
#include <bsoncxx/array/view-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>

#include <bsoncxx/document/element.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace array {

///
/// A variant view type that accesses values in serialized BSON arrays.
///
/// Element functions as a variant type, where the kind of the element can be
/// interrogated by calling type() and a specific value can be extracted through
/// get_X() accessors.
///
class element : private document::element {
   public:
    BSONCXX_ABI_EXPORT_CDECL() element();

    using document::element::operator bool;

    using document::element::type;

    using document::element::get_array;
    using document::element::get_binary;
    using document::element::get_bool;
    using document::element::get_code;
    using document::element::get_codewscope;
    using document::element::get_date;
    using document::element::get_dbpointer;
    using document::element::get_decimal128;
    using document::element::get_document;
    using document::element::get_double;
    using document::element::get_int32;
    using document::element::get_int64;
    using document::element::get_maxkey;
    using document::element::get_minkey;
    using document::element::get_null;
    using document::element::get_oid;
    using document::element::get_regex;
    using document::element::get_string;
    using document::element::get_symbol;
    using document::element::get_timestamp;
    using document::element::get_undefined;

    using document::element::get_value;

    using document::element::operator[];

    using document::element::key;
    using document::element::keylen;
    using document::element::length;
    using document::element::offset;
    using document::element::raw;

   private:
    friend ::bsoncxx::v_noabi::array::view;

    explicit element(const std::uint8_t* raw,
                     std::uint32_t length,
                     std::uint32_t offset,
                     std::uint32_t keylen);

    explicit element(const stdx::string_view key);
};

///
/// Convenience methods to compare for equality against a bson_value.
///
/// Compares equal if this element contains a matching bson_value. Otherwise, compares unequal.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::array::element
BSONCXX_ABI_EXPORT_CDECL(bool) operator==(const element& elem, const types::bson_value::view& v);

/// @relatesalso bsoncxx::v_noabi::array::element
BSONCXX_ABI_EXPORT_CDECL(bool) operator==(const types::bson_value::view& v, const element& elem);

/// @relatesalso bsoncxx::v_noabi::array::element
BSONCXX_ABI_EXPORT_CDECL(bool) operator!=(const element& elem, const types::bson_value::view& v);

/// @relatesalso bsoncxx::v_noabi::array::element
BSONCXX_ABI_EXPORT_CDECL(bool) operator!=(const types::bson_value::view& v, const element& elem);

/// @}
///

}  // namespace array
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace array {

using ::bsoncxx::v_noabi::array::operator==;
using ::bsoncxx::v_noabi::array::operator!=;

}  // namespace array
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::array::element.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace array {

/// @ref bsoncxx::v_noabi::array::operator==(const v_noabi::array::element& elem, const v_noabi::types::bson_value::view& v)
bool operator==(const v_noabi::array::element& elem, const v_noabi::types::bson_value::view& v);

/// @ref bsoncxx::v_noabi::array::operator==(const v_noabi::types::bson_value::view& v, const v_noabi::array::element& elem)
bool operator==(const v_noabi::types::bson_value::view& v, const v_noabi::array::element& elem);

/// @ref bsoncxx::v_noabi::array::operator!=(const v_noabi::array::element& elem, const v_noabi::types::bson_value::view& v)
bool operator!=(const v_noabi::array::element& elem, const v_noabi::types::bson_value::view& v);

/// @ref bsoncxx::v_noabi::array::operator!=(const v_noabi::types::bson_value::view& v, const v_noabi::array::element& elem)
bool operator!=(const v_noabi::types::bson_value::view& v, const v_noabi::array::element& elem);

}  // namespace array
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
