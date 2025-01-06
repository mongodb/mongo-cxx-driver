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

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <bsoncxx/document/element-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>

#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

///
/// An owning variant type that represents any BSON type.
///
/// Owns its underlying buffer. When a bson_value::value goes out of scope, its underlying buffer is
/// freed.
///
/// For accessors into this type and to extract the various BSON types out,
/// please use bson_value::view.
///
/// @see
/// - @ref bsoncxx::v_noabi::types::bson_value::view
///
class value {
   public:
    ///
    /// Construct a bson_value::value from the provided BSON type.
    ///
    /// @{
    BSONCXX_ABI_EXPORT_CDECL() value(b_double v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_string v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_document v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_array v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_binary v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_undefined v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_oid v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_bool v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_date v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_null);
    BSONCXX_ABI_EXPORT_CDECL() value(b_regex v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_dbpointer v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_code v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_symbol v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_codewscope v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_int32 v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_timestamp v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_int64 v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_decimal128 v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_maxkey v);
    BSONCXX_ABI_EXPORT_CDECL() value(b_minkey v);
    /// @}
    ///

    ///
    /// Constructs a BSON UTF-8 string value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(char const* v);

    ///
    /// Constructs a BSON UTF-8 string value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(std::string v);

    ///
    /// Constructs a BSON UTF-8 string value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(stdx::string_view v);

    ///
    /// Constructs a BSON 32-bit signed integer value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(int32_t v);

    ///
    /// Constructs a BSON 64-bit signed integer value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(int64_t v);

    ///
    /// Constructs a BSON double value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(double v);

    ///
    /// Constructs a BSON boolean value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(bool v);

    ///
    /// Constructs a BSON ObjectId value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(oid v);

    ///
    /// Constructs a BSON Decimal128 value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(decimal128 v);

    ///
    /// Constructs a BSON date value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(std::chrono::milliseconds v);

    ///
    /// Constructs a BSON null value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(std::nullptr_t);

    ///
    /// Constructs a BSON document value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(bsoncxx::v_noabi::document::view v);

    ///
    /// Constructs a BSON array value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(bsoncxx::v_noabi::array::view v);

    ///
    /// Constructs a BSON binary data value.
    ///
    /// @param v
    ///     a stream of bytes
    /// @param sub_type
    ///     an optional binary sub type. Defaults to type::k_binary
    ///
    BSONCXX_ABI_EXPORT_CDECL()
    value(std::vector<unsigned char> v, binary_sub_type const sub_type = {});

    ///
    /// Constructs a BSON binary data value.
    ///
    /// @param data
    ///     pointer to a stream of bytes
    /// @param size
    ///     the size of the stream of bytes
    /// @param sub_type
    ///     an optional binary sub type. Defaults to type::k_binary
    ///
    BSONCXX_ABI_EXPORT_CDECL()
    value(uint8_t const* data, size_t size, binary_sub_type const sub_type = {});

    ///
    /// Constructs a BSON DBPointer value.
    ///
    /// @param collection
    ///     the collection name
    /// @param value
    ///     the object id
    ///
    /// @warning The DBPointer (aka DBRef) BSON type is deprecated. Usage is discouraged.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(stdx::string_view collection, oid value);

    ///
    /// Constructs a BSON JavaScript code with scope value.
    ///
    /// @param code
    ///     the JavaScript code
    /// @param scope
    ///     a bson document view holding the scope environment
    ///
    BSONCXX_ABI_EXPORT_CDECL()
    value(stdx::string_view code, bsoncxx::v_noabi::document::view_or_value scope);

    ///
    /// Constructs a BSON regex value with options.
    ///
    /// @param regex
    ///   The regex pattern
    /// @param options
    ///   The regex options
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(stdx::string_view regex, stdx::string_view options);

    ///
    /// Constructs one of the following BSON values (each specified by the parenthesized type):
    /// - BSON code value (type::k_code)
    /// - BSON regex value (type::k_regex)
    /// - BSON symbol value (type::k_symbol)
    ///
    /// @param id
    ///     the type of BSON value to construct.
    /// @param v
    ///     the symbol, JavaScript code, or regex pattern for the BSON symbol, code, or regex value
    ///     respectively.
    ///
    /// @throws bsoncxx::v_noabi::exception if the type's value is not k_code, k_regex, or k_symbol.
    ///
    /// @warning The Symbol BSON type is deprecated. Usage is discouraged.
    /// @warning The Undefined BSON type is deprecated. Usage is discouraged.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(type const id, stdx::string_view v);

    ///
    /// Constructs one of the following BSON values (each specified by the parenthesized type):
    /// - BSON maxkey value (type::k_maxkey)
    /// - BSON minkey value (type::k_minkey)
    /// - BSON undefined value (type::k_undefined)
    ///
    /// @param id
    ///     the type of BSON value to construct.
    ///
    /// @throws bsoncxx::v_noabi::exception if the type's value is not k_maxkey, k_minkey, or
    /// k_undefined.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(type const id);

    ///
    /// Constructs one of the following BSON values (each specified by the parenthesized type):
    /// - BSON decimal128 value (type::k_decimal128)
    /// - BSON timestamp value (type::k_timestamp)
    ///
    /// @param id
    ///     the type of the BSON value to construct.
    /// @param a
    ///     If a BSON decimal128 value is to be constructed, this is the high value.
    ///     If a BSON timestamp value is to be constructed, this is the increment.
    /// @param b
    ///     If a BSON decimal128 value is to be constructed, this is the low value.
    ///     If a BSON timestamp value is to be constructed, this is the timestamp.
    ///
    /// @throws bsoncxx::v_noabi::exception if the specified type is missing its required arguments.
    ///
    /// @warning
    ///   The BSON timestamp type is used internally by the MongoDB server - use by clients
    ///   is discouraged.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(type const id, uint64_t a, uint64_t b);

    BSONCXX_ABI_EXPORT_CDECL() ~value();

    BSONCXX_ABI_EXPORT_CDECL() value(value const&);
    BSONCXX_ABI_EXPORT_CDECL(value&) operator=(value const&);

    BSONCXX_ABI_EXPORT_CDECL() value(value&&) noexcept;
    BSONCXX_ABI_EXPORT_CDECL(value&) operator=(value&&) noexcept;

    ///
    /// Create an owning copy of a bson_value::view.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() value(bson_value::view const&);

    ///
    /// Get a view over the bson_value owned by this object.
    ///
    BSONCXX_ABI_EXPORT_CDECL(bson_value::view) view() const noexcept;

    ///
    /// Conversion operator that provides a bson_value::view given a bson_value::value.
    ///
    BSONCXX_ABI_EXPORT_CDECL() operator bson_value::view() const noexcept;

   private:
    friend ::bsoncxx::v_noabi::document::element;

    value(std::uint8_t const* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen);

    // Makes a copy of 'internal_value' and owns the copy.
    // Export is required by mongocxx via make_owning_bson.
    BSONCXX_ABI_EXPORT_CDECL() value(void* internal_value);

    friend value make_owning_bson(void* internal_value);

    class impl;
    std::unique_ptr<impl> _impl;
};

///
/// Compares values for (in)-equality.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::types::bson_value::value
inline bool operator==(value const& lhs, value const& rhs) {
    return (lhs.view() == rhs.view());
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::value
inline bool operator!=(value const& lhs, value const& rhs) {
    return !(lhs == rhs);
}

/// @}
///

///
/// Compares a value with a view for (in)equality.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::types::bson_value::value
inline bool operator==(value const& lhs, view const& rhs) {
    return (lhs.view() == rhs);
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::value
inline bool operator==(view const& lhs, value const& rhs) {
    return (rhs == lhs);
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::value
inline bool operator!=(value const& lhs, view const& rhs) {
    return !(lhs == rhs);
}

/// @relatesalso bsoncxx::v_noabi::types::bson_value::value
inline bool operator!=(view const& lhs, value const& rhs) {
    return !(lhs == rhs);
}

/// @}
///

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace types {
namespace bson_value {

using ::bsoncxx::v_noabi::types::bson_value::operator==;
using ::bsoncxx::v_noabi::types::bson_value::operator!=;

} // namespace bson_value
} // namespace types
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::types::bson_value::value.
///
