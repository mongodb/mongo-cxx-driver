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

#include <array>
#include <ctime>
#include <string>

#include <bsoncxx/oid-fwd.hpp>

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
   public:
    static constexpr std::size_t k_oid_length = 12;

    ///
    /// Constructs an oid and initializes it to a newly generated ObjectId.
    ///
    BSONCXX_ABI_EXPORT_CDECL() oid();

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
    explicit BSONCXX_ABI_EXPORT_CDECL() oid(stdx::string_view const& str);

    ///
    /// Converts this oid to a hexadecimal string.
    ///
    /// @return A hexadecimal string representation of this ObjectId.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::string) to_string() const;

    ///
    /// Returns the number of bytes in this ObjectId.
    ///
    /// @return The length of this oid's buffer.
    ///
    static std::size_t size() {
        return k_oid_length;
    }

    ///
    /// @relates bsoncxx::v_noabi::oid
    ///
    /// Relational operators for OIDs.
    ///
    /// @{
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator<(oid const& lhs, oid const& rhs);
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator>(oid const& lhs, oid const& rhs);
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator<=(oid const& lhs, oid const& rhs);
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator>=(oid const& lhs, oid const& rhs);
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator==(oid const& lhs, oid const& rhs);
    friend BSONCXX_ABI_EXPORT_CDECL(bool) operator!=(oid const& lhs, oid const& rhs);
    /// @}
    ///

    ///
    /// @memberof bsoncxx::v_noabi::oid <!-- Fix leaky relates above. -->
    /// Extracts the timestamp portion of the underlying ObjectId.
    ///
    /// @return A std::time_t initialized to the timestamp.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::time_t) get_time_t() const;

    ///
    /// An accessor for the internal data buffer in the oid.
    ///
    /// @return A pointer to the internal buffer holding the oid bytes.
    ///
    BSONCXX_ABI_EXPORT_CDECL(char const*) bytes() const;

   private:
    friend int oid_compare(oid const& lhs, oid const& rhs);

    std::array<char, k_oid_length> _bytes;
};

} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::oid.
///
