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

#include <bsoncxx/types/bson_value/value-fwd.hpp>

//

#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/document/element-fwd.hpp>

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
/// @deprecated Use @ref bsoncxx::v_noabi::types::value instead (renamed).
///
/// @see
/// - @ref bsoncxx::v_noabi::types::bson_value::view
///
class value {
   private:
    v1::types::value _value;

    template <typename T>
    using is_value = detail::is_alike<T, value>;

    template <typename T>
    using from_v1_expr = decltype(from_v1(std::declval<T>()));

    template <typename T>
    struct has_from_v1 : detail::conjunction<detail::negation<is_value<T>>, detail::is_detected<from_v1_expr, T>> {};

   public:
    /// @copydoc v1::types::value::value()
    value() = default;

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ value(v1::types::value v) : _value{std::move(v)} {}

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /// @par Constraints:
    /// - `from_v1(v)` is found via ADL.
    ///
    template <typename T, detail::enable_if_t<has_from_v1<T>::value>* = nullptr>
    /* explicit(false) */ value(T v) : value{from_v1(v)} {}

#pragma push_macro("X")
#undef X
#define X(_name, _unused) \
    /* explicit(false) */ value(v_noabi::types::b_##_name v) : _value{to_v1(v)} {}

    ///
    /// Construct a bson_value::value from the provided BSON type.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")

    ///
    /// Constructs a BSON UTF-8 string value.
    ///
    /* explicit(false) */ value(char const* v) : _value{v} {}

    ///
    /// Constructs a BSON UTF-8 string value.
    ///
    /* explicit(false) */ value(std::string v) : _value{std::move(v)} {}

    ///
    /// Constructs a BSON UTF-8 string value.
    ///
    /* explicit(false) */ value(v1::stdx::string_view v) : _value{v} {}

    ///
    /// Constructs a BSON 32-bit signed integer value.
    ///
    /* explicit(false) */ value(std::int32_t v) : _value{v} {}

    ///
    /// Constructs a BSON 64-bit signed integer value.
    ///
    /* explicit(false) */ value(std::int64_t v) : _value{v} {}

    ///
    /// Constructs a BSON double value.
    ///
    /* explicit(false) */ value(double v) : _value{v} {}

    ///
    /// Constructs a BSON boolean value.
    ///
    /* explicit(false) */ value(bool v) : _value{v} {}

    ///
    /// Constructs a BSON ObjectId value.
    ///
    /* explicit(false) */ value(v_noabi::oid v) : _value{to_v1(v)} {}

    ///
    /// Constructs a BSON Decimal128 value.
    ///
    /* explicit(false) */ value(v_noabi::decimal128 v) : _value{to_v1(v)} {}

    ///
    /// Constructs a BSON date value.
    ///
    /* explicit(false) */ value(std::chrono::milliseconds v) : _value{v} {}

    ///
    /// Constructs a BSON null value.
    ///
    /* explicit(false) */ value(std::nullptr_t) : _value{nullptr} {}

    ///
    /// Constructs a BSON document value.
    ///
    /* explicit(false) */ value(v_noabi::document::view v) : _value{to_v1(v)} {}

    ///
    /// Constructs a BSON array value.
    ///
    /* explicit(false) */ value(v_noabi::array::view v) : _value{to_v1(v)} {}

    ///
    /// Constructs a BSON binary data value.
    ///
    /// @param v
    ///     a stream of bytes
    /// @param sub_type
    ///     an optional binary sub type. Defaults to type::k_binary
    ///
    /* explicit(false) */
    value(std::vector<unsigned char> v, v_noabi::binary_sub_type const sub_type = {})
        : _value{reinterpret_cast<std::uint8_t const*>(v.data()), v.size(), to_v1(sub_type)} {}

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
    /* explicit(false) */
    value(std::uint8_t const* data, std::size_t size, v_noabi::binary_sub_type const sub_type = {})
        : _value{data, size, to_v1(sub_type)} {}

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
    /* explicit(false) */ value(v1::stdx::string_view collection, v_noabi::oid value)
        : _value{collection, to_v1(value)} {}

    ///
    /// Constructs a BSON JavaScript code with scope value.
    ///
    /// @param code
    ///     the JavaScript code
    /// @param scope
    ///     a bson document view holding the scope environment
    ///
    /* explicit(false) */ value(v1::stdx::string_view code, v_noabi::document::view_or_value scope)
        : _value{code, to_v1(scope.view())} {}

    ///
    /// Constructs a BSON regex value with options.
    ///
    /// @param regex
    ///   The regex pattern
    /// @param options
    ///   The regex options
    ///
    /* explicit(false) */ value(v1::stdx::string_view regex, v1::stdx::string_view options)
        : _value{v1::types::b_regex{regex, options}} {}

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
    /* explicit(false) */ BSONCXX_ABI_EXPORT_CDECL() value(v_noabi::type const id, v1::stdx::string_view v);

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
    /* explicit(false) */ BSONCXX_ABI_EXPORT_CDECL() value(type const id);

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
    BSONCXX_ABI_EXPORT_CDECL() value(v_noabi::type const id, std::uint64_t a, std::uint64_t b);

    ///
    /// Create an owning copy of a bson_value::view.
    ///
    explicit value(v_noabi::types::bson_value::view const& v) : _value{to_v1(v)} {}

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    explicit operator v1::types::value() && {
        return std::move(_value);
    }

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    explicit operator v1::types::value() const& {
        return _value;
    }

    ///
    /// Returns the type of the underlying BSON value stored in this object.
    ///
    /// @deprecated Use @ref type_id() const instead (renamed).
    ///
    v_noabi::type type() const {
        return this->view().type();
    }

    ///
    /// Equivalent to @ref type() const.
    ///
    /// To support incremental migration to @ref bsoncxx::v1::types::view.
    ///
    v_noabi::type type_id() const {
        return this->view().type_id();
    }

    ///
    /// Get a view over the bson_value owned by this object.
    ///
    v_noabi::types::bson_value::view view() const noexcept {
        return _value.view();
    }

    ///
    /// Conversion operator that provides a bson_value::view given a bson_value::value.
    ///
    /* explicit(false) */ operator v_noabi::types::bson_value::view() const noexcept {
        return _value.view();
    }

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                   \
    v_noabi::types::b_##_name const& get_##_name() const { \
        return this->view().get_##_name();                 \
    }

    ///
    /// Return the underlying BSON type value.
    ///
    /// @warning
    ///   Calling the wrong get_<type> method will cause an exception to be thrown.
    ///
    /// @{
    BSONCXX_V1_TYPES_XMACRO(X)
    /// @}
    ///
#pragma pop_macro("X")
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
namespace v_noabi {

///
/// Convert to the @ref bsoncxx::v_noabi equivalent of `v`.
///
inline v_noabi::types::bson_value::value from_v1(v1::types::value v) {
    return {std::move(v)};
}

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::types::value to_v1(v_noabi::types::bson_value::value v) {
    return v1::types::value{std::move(v)};
}

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace types {
namespace bson_value {

using v_noabi::types::bson_value::operator==;
using v_noabi::types::bson_value::operator!=;

} // namespace bson_value
} // namespace types
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::types::bson_value::value.
///
/// @deprecated Use @ref bsoncxx/types/value.hpp instead (renamed).
///
/// @par Includes
/// - @ref bsoncxx/v1/types/value.hpp
///
