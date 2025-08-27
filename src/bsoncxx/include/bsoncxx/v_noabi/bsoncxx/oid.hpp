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

#include <bsoncxx/oid-fwd.hpp>

//

#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/oid.hpp>

#include <array>
#include <ctime>
#include <string>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {

///
/// Represents a MongoDB BSON ObjectId.
///
/// @see
/// - [BSON Types (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/bson-types/)
///
class oid {
   private:
    v1::oid _oid;

   public:
    static constexpr BSONCXX_ABI_EXPORT std::size_t k_oid_length = v1::oid::k_oid_length;

    ///
    /// Constructs an oid and initializes it to a newly generated ObjectId.
    ///
    BSONCXX_ABI_EXPORT_CDECL() oid();

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ oid(v1::oid const& oid) noexcept : _oid{oid} {}

    ///
    /// Constructs an oid initializes it to the contents of the provided buffer.
    ///
    /// @param bytes
    ///   A pointer a buffer containing a valid ObjectId.
    /// @param len
    ///   The length of the buffer. Should be equal to oid::size().
    ///
    /// @throws bsoncxx::v_noabi::exception if the length is not equal to oid::size().
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() oid(char const* bytes, std::size_t len);

    ///
    /// Constructs an oid and initializes it from the provided hex string.
    ///
    /// @param str
    ///   A string of a hexadecimal representation of a valid ObjectId.
    ///
    /// @throws bsoncxx::v_noabi::exception if the string isn't an OID-sized hex
    /// string.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() oid(v1::stdx::string_view const& str);

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    explicit operator v1::oid() const noexcept {
        return _oid;
    }

    ///
    /// Converts this oid to a hexadecimal string.
    ///
    /// @return A hexadecimal string representation of this ObjectId.
    ///
    std::string to_string() const {
        return _oid.to_string();
    }

    ///
    /// Returns the number of bytes in this ObjectId.
    ///
    /// @return The length of this oid's buffer.
    ///
    static std::size_t size() {
        return k_oid_length;
    }

    ///
    /// @memberof bsoncxx::v_noabi::oid <!-- Fix leaky relates above. -->
    /// Extracts the timestamp portion of the underlying ObjectId.
    ///
    /// @return A std::time_t initialized to the timestamp.
    ///
    std::time_t get_time_t() const {
        return _oid.get_time_t();
    }

    ///
    /// An accessor for the internal data buffer in the oid.
    ///
    /// @return A pointer to the internal buffer holding the oid bytes.
    ///
    char const* bytes() const {
        return reinterpret_cast<char const*>(_oid.bytes());
    }

    ///
    /// @relates bsoncxx::v_noabi::oid
    ///
    /// Relational comparison operator.
    ///
    /// @{
    friend bool operator<(oid const& lhs, oid const& rhs) {
        return lhs._oid < rhs._oid;
    }

    friend bool operator>(oid const& lhs, oid const& rhs) {
        return lhs._oid > rhs._oid;
    }

    friend bool operator<=(oid const& lhs, oid const& rhs) {
        return lhs._oid <= rhs._oid;
    }

    friend bool operator>=(oid const& lhs, oid const& rhs) {
        return lhs._oid >= rhs._oid;
    }

    friend bool operator==(oid const& lhs, oid const& rhs) {
        return lhs._oid == rhs._oid;
    }

    friend bool operator!=(oid const& lhs, oid const& rhs) {
        return lhs._oid != rhs._oid;
    }
    /// @}
    ///
};

///
/// Convert to the @ref bsoncxx::v_noabi equivalent of `v`.
///
inline oid from_v1(v1::oid const& v) {
    return {v};
}

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::oid to_v1(v_noabi::oid const& v) {
    return v1::oid{v};
}

} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::oid.
///
/// @par Includes
/// - @ref bsoncxx/v1/oid.hpp
///
