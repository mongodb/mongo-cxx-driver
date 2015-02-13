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

#include <bsoncxx/config/prelude.hpp>

#include <iostream>
#include <ctime>
#include <string>

#include <bsoncxx/stdx/string_view.hpp>

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
/// @see http://docs.mongodb.org/manual/reference/object-id/
///
class BSONCXX_API oid {
   public:
    struct init_tag_t {};
    static constexpr init_tag_t init_tag{};

    ///
    /// Constructs an uninitialized oid.
    ///
    oid();

    ///
    /// Constructs an oid and initializes it to a newly generated ObjectId.
    ///
    /// @param tag
    ///   A bsoncxx::oid::init_tag used to dispatch this overload.
    ///
    explicit oid(init_tag_t tag);

    ///
    /// Constructs an oid initializes it to the contents of the provided buffer.
    ///
    /// @param bytes
    ///   A pointer a buffer containing a valid ObjectId.
    /// @param len
    ///   The length of the buffer. Should be 12.
    ///
    explicit oid(const char* bytes, std::size_t len);

    ///
    /// Constructs and oid an initializes it from the provided hex string.
    ///
    /// @param str
    ///   A string_view of a hexadecimal representation of a valid ObjectId.
    ///
    explicit oid(stdx::string_view str);

    ///
    /// Converts this oid to a hexadecimal string.
    ///
    /// @return A hexadecimal string representation of this ObjectId.
    ///
    std::string to_string() const;

    friend BSONCXX_API bool operator<(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool operator>(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool operator<=(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool operator>=(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool operator==(const oid& lhs, const oid& rhs);
    friend BSONCXX_API bool operator!=(const oid& lhs, const oid& rhs);

    explicit operator bool() const;

    ///
    /// Extracts the timestamp portion of the underlying ObjectId.
    ///
    /// @return A std::time_t initialized to the timestamp.
    ///
    std::time_t get_time_t() const;

    friend BSONCXX_API std::ostream& operator<<(std::ostream& out, const oid& rhs);

    const char* bytes() const;

   private:
    friend BSONCXX_API int oid_compare(const oid& lhs, const oid& rhs);

    bool _is_valid;
    char _bytes[12];
};

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
