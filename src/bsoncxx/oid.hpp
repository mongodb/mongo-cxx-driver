// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// Represents a MongoDB ObjectId. As this BSON type is used within the MongoDB server
/// as a primary key for each document, it is useful for representing a 'pointer'
/// to another document.
///
/// @note we use 'oid' to refer to this concrete class. We use 'ObjectId' to refer
/// to the BSON type.
///
/// @see https://docs.mongodb.com/master/reference/object-id/
///
class BSONCXX_API oid {
   public:
    ///
    /// Constructs an oid and initializes it to a newly generated ObjectId.
    ///
    oid();

    struct init_tag_t {};

    // TODO(MSVC): Ideally this would be constexpr, but VS2015U1 can't
    // handle it.
    //
    // See https://connect.microsoft.com/VisualStudio/feedback/details/2092790
    //
    BSONCXX_DEPRECATED static const init_tag_t init_tag;

    ///
    /// Constructs an oid and initializes it to a newly generated ObjectId.
    ///
    /// @deprecated
    ///   This constructor for bsoncxx::oid is still supported but deprecated.
    ///
    /// @param tag
    ///   A bsoncxx::oid::init_tag used to dispatch this overload.
    ///
    BSONCXX_DEPRECATED explicit oid(init_tag_t tag);

    struct init_tag_t_deprecated {};
    static const init_tag_t_deprecated init_tag_deprecated;
    explicit oid(init_tag_t_deprecated);

    ///
    /// Constructs an oid initializes it to the contents of the provided buffer.
    ///
    /// @param bytes
    ///   A pointer a buffer containing a valid ObjectId.
    /// @param len
    ///   The length of the buffer. Should be 12.
    ///
    /// @throws bsoncxx::exception if the length is not 12.
    ///
    explicit oid(const char* bytes, std::size_t len);

    ///
    /// Constructs an oid and initializes it from the provided hex string.
    ///
    /// @param str
    ///   A string of a hexadecimal representation of a valid ObjectId.
    ///
    /// @throws bsoncxx::exception if the string isn't an OID-sized hex
    /// string.
    ///
    explicit oid(const bsoncxx::stdx::string_view& str);

    ///
    /// Converts this oid to a hexadecimal string.
    ///
    /// @return A hexadecimal string representation of this ObjectId.
    ///
    std::string to_string() const;

    ///
    /// @{
    ///
    /// Relational operators for OIDs
    ///
    /// @relates oid
    ///
    friend BSONCXX_API bool BSONCXX_CALL operator<(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool BSONCXX_CALL operator>(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool BSONCXX_CALL operator<=(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool BSONCXX_CALL operator>=(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool BSONCXX_CALL operator==(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool BSONCXX_CALL operator!=(const oid& lhs, const oid& rhs);
    ///
    /// @}
    ///

    ///
    /// Conversion operator that indicates that the oid is initialized.
    ///
    /// @deprecated
    ///   Uninitialized oids can no longer be created so this function will always return True.
    ///
    /// @return True
    ///
    BSONCXX_DEPRECATED explicit operator bool() const;
    bool operator_bool_deprecated() const;

    ///
    /// Extracts the timestamp portion of the underlying ObjectId.
    ///
    /// @return A std::time_t initialized to the timestamp.
    ///
    std::time_t get_time_t() const;

    ///
    /// An accessor for the internal data buffer in the oid.
    ///
    /// @return A pointer to the internal buffer holding the oid bytes.
    ///
    const char* bytes() const;

   private:
    friend BSONCXX_PRIVATE int oid_compare(const oid& lhs, const oid& rhs);

    std::array<char, 12> _bytes;
};

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
