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

#include <bsoncxx/decimal128-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/decimal128.hpp>         // IWYU pragma: export
#include <bsoncxx/v1/detail/type_traits.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <cstdint>
#include <string>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

///
/// Represents a MongoDB BSON Decimal128.
///
/// This type implements the Decimal Arithmetic Encodings (IEEE 754-2008) specification, _with certain
/// exceptions_ around value integrity and the coefficient encoding. When a value cannot be represented exactly, the
/// value will be rejected.
///
/// @see
/// - [BSON Types (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/bson-types/)
/// - [BSON Decimal128 (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/bson-decimal128/decimal128/)
///
class decimal128 {
   private:
    v1::decimal128 _d128;

   public:
    ///
    /// Constructs a BSON Decimal128 value representing zero.
    ///
    decimal128() = default;

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ decimal128(v1::decimal128 const& d128) noexcept : _d128{d128} {}

    ///
    /// Constructs a BSON Decimal128 from high and low 64-bit big-endian parts.
    ///
    /// @param high
    ///     The high 64-bits.
    /// @param low
    ///     The low 64-bits.
    ///
    decimal128(std::uint64_t high, std::uint64_t low) : _d128{high, low} {}

    ///
    /// Constructs a BSON Decimal128 from a string.
    ///
    /// @param str
    ///     A string representation of a decimal number.
    ///
    /// @throws bsoncxx::v_noabi::exception if the string isn't a valid BSON Decimal128
    /// representation.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() decimal128(v1::stdx::string_view str);

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    explicit operator v1::decimal128() const noexcept {
        return _d128;
    }

    ///
    /// Converts this decimal128 value to a string representation.
    ///
    /// @return A string representation of a IEEE 754-2008 decimal number.
    ///
    std::string to_string() const {
        return _d128.to_string();
    }

    ///
    /// Accessor for high 64 bits.
    ///
    std::uint64_t high() const {
        return _d128.high();
    }

    ///
    /// Accessor for low 64 bits.
    ///
    std::uint64_t low() const {
        return _d128.low();
    }

    ///
    /// @relates bsoncxx::v_noabi::decimal128
    ///
    /// Equality comparison operator.
    ///
    friend bool operator==(decimal128 const& lhs, decimal128 const& rhs) {
        return lhs._d128 == rhs._d128;
    }

    ///
    /// @relates bsoncxx::v_noabi::decimal128
    ///
    /// Equality comparison operator.
    ///
    friend bool operator!=(decimal128 const& lhs, decimal128 const& rhs) {
        return lhs._d128 != rhs._d128;
    }
};

///
/// Convert to the @ref bsoncxx::v_noabi equivalent of `v`.
///
inline v_noabi::decimal128 from_v1(v1::decimal128 const& v) {
    return {v};
}

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::decimal128 to_v1(v_noabi::decimal128 const& v) {
    return v1::decimal128{v};
}

} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::decimal128.
///
/// @par Includes
/// - @ref bsoncxx/v1/decimal128.hpp
///
