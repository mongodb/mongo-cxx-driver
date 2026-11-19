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

#include <bsoncxx/v1/array/value-fwd.hpp> // IWYU pragma: keep
#include <bsoncxx/v1/array/view-fwd.hpp>
#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/element/view-fwd.hpp> // IWYU pragma: keep

#include <bsoncxx/v1/config/config.hpp> // IWYU pragma: keep
#include <bsoncxx/v1/decimal128.hpp>    // IWYU pragma: keep
#include <bsoncxx/v1/detail/bit.hpp>
#include <bsoncxx/v1/detail/compare.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/exception.hpp>
#include <bsoncxx/v1/oid.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/id.hpp>
#include <bsoncxx/v1/types/value.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <string>
#include <system_error>
#include <type_traits>

#include <bsoncxx/document/value-fwd.hpp>
#include <bsoncxx/document/view-fwd.hpp>
#include <bsoncxx/oid-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <bsoncxx/private/export.hh>

#include <catch2/catch_tostring.hpp>

CATCH_REGISTER_ENUM(
    bsoncxx::v1::oid::errc,
    bsoncxx::v1::oid::errc::zero,
    bsoncxx::v1::oid::errc::null_bytes_ptr,
    bsoncxx::v1::oid::errc::invalid_length,
    bsoncxx::v1::oid::errc::empty_string,
    bsoncxx::v1::oid::errc::invalid_string)

CATCH_REGISTER_ENUM(
    bsoncxx::v1::decimal128::errc,
    bsoncxx::v1::decimal128::errc::zero,
    bsoncxx::v1::decimal128::errc::empty_string,
    bsoncxx::v1::decimal128::errc::invalid_string_length,
    bsoncxx::v1::decimal128::errc::invalid_string_data)

CATCH_REGISTER_ENUM(
    bsoncxx::v1::types::view::errc,
    bsoncxx::v1::types::view::errc::zero,
    bsoncxx::v1::types::view::errc::type_mismatch)

CATCH_REGISTER_ENUM(
    bsoncxx::v1::types::value::errc,
    bsoncxx::v1::types::value::errc::zero,
    bsoncxx::v1::types::value::errc::invalid_length_u32)

CATCH_REGISTER_ENUM(
    bsoncxx::v1::source_errc,
    bsoncxx::v1::source_errc::zero,
    bsoncxx::v1::source_errc::bsoncxx,
    bsoncxx::v1::source_errc::bson)

CATCH_REGISTER_ENUM(
    bsoncxx::v1::type_errc,
    bsoncxx::v1::type_errc::zero,
    bsoncxx::v1::type_errc::invalid_argument,
    bsoncxx::v1::type_errc::runtime_error)

CATCH_REGISTER_ENUM(
    bsoncxx::detail::endian,
    bsoncxx::detail::endian::little,
    bsoncxx::detail::endian::big,
    bsoncxx::detail::endian::native)

namespace bsoncxx {
namespace test {

// Equivalent to `Catch::Detail::stringify`.
template <typename Param>
std::string stringify(Param const& param) {
    using T = typename std::remove_cv<typename std::remove_reference<Param>::type>::type;
    return Catch::StringMaker<T>::convert(param);
}

} // namespace test
} // namespace bsoncxx

namespace Catch {

template <>
struct StringMaker<bsoncxx::oid> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::oid const& value);
};

template <>
struct StringMaker<bsoncxx::document::view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::document::view const& value);
};

template <>
struct StringMaker<bsoncxx::document::view_or_value> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::document::view_or_value const& value);
};

template <>
struct StringMaker<bsoncxx::document::value> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::document::value const& value);
};

template <>
struct StringMaker<bsoncxx::types::bson_value::view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::types::bson_value::view const& value);
};

template <>
struct StringMaker<bsoncxx::types::bson_value::value> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::types::bson_value::value const& value);
};

template <>
struct StringMaker<bsoncxx::types::bson_value::view_or_value> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::types::bson_value::view_or_value const& value);
};

template <>
struct StringMaker<bsoncxx::detail::strong_ordering> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::detail::strong_ordering value);
};

template <>
struct StringMaker<std::error_category> {
    static std::string BSONCXX_ABI_CDECL convert(std::error_category const& value);
};

template <>
struct StringMaker<std::error_condition> {
    static std::string BSONCXX_ABI_CDECL convert(std::error_condition const& value);
};

template <>
struct StringMaker<bsoncxx::v1::oid> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::oid const& value);
};

template <>
struct StringMaker<bsoncxx::v1::decimal128> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::decimal128 const& value);
};

template <>
struct StringMaker<bsoncxx::v1::array::view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::array::view const& value);
};

template <>
struct StringMaker<bsoncxx::v1::array::value> : StringMaker<bsoncxx::v1::array::view> {};

template <>
struct StringMaker<bsoncxx::v1::document::view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::document::view const& value);
};

template <>
struct StringMaker<bsoncxx::v1::document::view::const_iterator> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::document::view::const_iterator const& value);
};

template <>
struct StringMaker<bsoncxx::v1::document::value> : StringMaker<bsoncxx::v1::document::view> {};

template <>
struct StringMaker<bsoncxx::v1::element::view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::element::view const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::id> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::id const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::binary_subtype> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::binary_subtype const& value);
};

#if defined(BSONCXX_POLY_USE_IMPLS)
template <>
struct StringMaker<bsoncxx::v1::stdx::string_view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::stdx::string_view const& value);
};
#endif

template <typename T>
struct StringMaker<bsoncxx::v1::stdx::optional<T>> {
    static std::string convert(bsoncxx::v1::stdx::optional<T> const& value) {
        if (value) {
            return bsoncxx::test::stringify(value.value());
        }

        return "nullopt";
    }
};

template <>
struct StringMaker<bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::nullopt_t>> {
    static std::string convert(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::nullopt_t> const&) {
        return "nullopt";
    }
};

template <>
struct StringMaker<bsoncxx::v1::types::view> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::view const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::value> : StringMaker<bsoncxx::v1::types::view> {};

template <>
struct StringMaker<bsoncxx::v1::types::b_double> : StringMaker<double> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_string> : StringMaker<bsoncxx::v1::stdx::string_view> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_document> : StringMaker<bsoncxx::v1::document::view> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_array> : StringMaker<bsoncxx::v1::array::view> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_oid> : StringMaker<bsoncxx::v1::oid> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_bool> : StringMaker<bool> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_code> : StringMaker<bsoncxx::v1::stdx::string_view> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_symbol> : StringMaker<bsoncxx::v1::stdx::string_view> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_int32> : StringMaker<std::int32_t> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_int64> : StringMaker<std::int64_t> {};
template <>
struct StringMaker<bsoncxx::v1::types::b_decimal128> : StringMaker<bsoncxx::v1::decimal128> {};

template <>
struct StringMaker<bsoncxx::v1::types::b_binary> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::b_binary const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::b_undefined> {
    static std::string convert(bsoncxx::v1::types::b_undefined const&) {
        return "undefined";
    }
};

template <>
struct StringMaker<bsoncxx::v1::types::b_date> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::b_date const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::b_null> {
    static std::string convert(bsoncxx::v1::types::b_null const&) {
        return "null";
    }
};

template <>
struct StringMaker<bsoncxx::v1::types::b_regex> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::b_regex const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::b_dbpointer> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::b_dbpointer const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::b_codewscope> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::b_codewscope const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::b_timestamp> {
    static std::string BSONCXX_ABI_CDECL convert(bsoncxx::v1::types::b_timestamp const& value);
};

template <>
struct StringMaker<bsoncxx::v1::types::b_maxkey> {
    static std::string convert(bsoncxx::v1::types::b_maxkey const&) {
        return "maxkey";
    }
};

template <>
struct StringMaker<bsoncxx::v1::types::b_minkey> {
    static std::string convert(bsoncxx::v1::types::b_minkey const&) {
        return "minkey";
    }
};

} // namespace Catch
