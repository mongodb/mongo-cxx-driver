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

#include <bsoncxx/v1/decimal128-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>
#include <system_error>

namespace bsoncxx {
namespace v1 {
namespace error {
namespace category {

///
/// The error category for @ref bsoncxx::v1::error::decimal128.
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) decimal128();

} // namespace category
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace bsoncxx {
namespace v1 {
namespace error {

///
/// Errors codes which may be returned by @ref bsoncxx::v1::decimal128.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class decimal128 {
    zero,                  ///< Zero.
    empty_string,          ///< String must not be empty.
    invalid_string_length, ///< Length of string is too long (exceeds `INT_MAX`).
    invalid_string_data,   ///< String is not a valid Decimal128 representation.
};

///
/// Support implicit conversion to `std::error_code`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_code make_error_code(decimal128 v) {
    return {static_cast<int>(v), v1::error::category::decimal128()};
}

} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace std {

template <>
struct is_error_code_enum<bsoncxx::v1::error::decimal128> : true_type {};

} // namespace std

namespace bsoncxx {
namespace v1 {

///
/// A BSON Decimal128.
///
/// @attention This feature is experimental! It is not ready for use!
///
class decimal128 {
   private:
    std::uint64_t _high = 0u;
    std::uint64_t _low = 0u;

   public:
    ///
    /// Zero-initialize the byte representation.
    ///
    /// @note The result is equivalent to `"0E-6176"`, not `"0"`.
    ///
    decimal128() = default;

    ///
    /// Initialize with the given `high` and `low` byte representations.
    ///
    decimal128(std::uint64_t high, std::uint64_t low) : _high{high}, _low{low} {}

    ///
    /// Initialize with the given Decimal128 string representation.
    ///
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::error::decimal128::empty_string if `str` is null.
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::error::decimal128::invalid_string_length if the length of
    /// `str` exceeds `INT_MAX`.
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::error::decimal128::invalid_string_data if `str` is not a
    /// valid Decimal128 string representation.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() decimal128(v1::stdx::string_view str);

    ///
    /// Return the string representation.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::string) to_string() const;

    ///
    /// Return the high-order bytes.
    ///
    std::uint64_t high() const {
        return _high;
    }

    ///
    /// Return the low-order bytes.
    ///
    std::uint64_t low() const {
        return _low;
    }

    ///
    /// Compare equal when the byte representations compare equal.
    ///
    /// @{
    friend bool operator==(decimal128 const& lhs, decimal128 const& rhs) {
        return lhs._high == rhs._high && lhs._low == rhs._low;
    }

    friend bool operator!=(decimal128 const& lhs, decimal128 const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::decimal128.
///
