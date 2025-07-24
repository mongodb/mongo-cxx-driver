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

#include <bsoncxx/v1/oid-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <system_error>
namespace bsoncxx {
namespace v1 {

///
/// A BSON ObjectID.
///
/// @attention This feature is experimental! It is not ready for use!
///
class oid {
   public:
    ///
    /// The number of bytes required to represent an ObjectID.
    ///
    static constexpr BSONCXX_ABI_EXPORT std::size_t k_oid_length = 12;

   private:
    std::array<std::uint8_t, k_oid_length> _bytes;

   public:
    ///
    /// Initialize with a unique ObjectID.
    ///
    /// The ObjectID is generated using the bson library's
    /// [`bson_oid_init`](https://mongoc.org/libbson/current/bson_oid_init.html) function with the default
    /// [`bson_context_t`](https://mongoc.org/libbson/current/bson_context_t.html).
    ///
    /// @throws bsoncxx::v1::exception (on Windows only) with a `std::system_category()` error code
    /// (as returned by
    /// [`WSAGetLastError()`](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsagetlasterror))
    /// on failure to initialize the Winsock library with
    /// [`WSAStartup()`](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup).
    ///
    BSONCXX_ABI_EXPORT_CDECL() oid();

    ///
    /// Initialize with the given ObjectID byte representation.
    ///
    /// @param bytes A pointer to the ObjectID byte representation.
    /// @param len The length of the array pointed to by `bytes`.
    ///
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::oid::errc::null_bytes_ptr if `bytes` is null.
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::oid::errc::invalid_length if `len` is not equal to @ref
    /// k_oid_length.
    ///
    BSONCXX_ABI_EXPORT_CDECL() oid(std::uint8_t const* bytes, std::size_t len);

    ///
    /// Initialize with the given ObjectID hexadecimal string representation.
    ///
    /// @param str A valid ObjectID represented.
    ///
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::oid::errc::empty_string if `str` is empty.
    /// @throws bsoncxx::v1::exception with @ref bsoncxx::v1::oid::errc::invalid_string if `str` is invalid.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() oid(v1::stdx::string_view str);

    ///
    /// Return @ref k_oid_length.
    ///
    static std::size_t size() {
        return k_oid_length;
    }

    ///
    /// Return a pointer to the byte representation.
    ///
    std::uint8_t const* bytes() const {
        return _bytes.data();
    }

    ///
    /// Return the hexadecimal representation.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::string) to_string() const;

    ///
    /// Return the timestamp component.
    ///
    BSONCXX_ABI_EXPORT_CDECL(std::time_t) get_time_t() const;

    ///
    /// Equivalent to [`bson_oid_compare`](https://mongoc.org/libbson/current/bson_oid_compare.html).
    ///
    BSONCXX_ABI_EXPORT_CDECL(int) compare(oid const& other) const;

    ///
    /// Equivalent to `lhs.compare(rhs) == 0`.
    ///
    friend bool operator==(oid const& lhs, oid const& rhs) {
        return lhs.compare(rhs) == 0;
    }

    ///
    /// Equivalent to `lhs.compare(rhs) != 0`.
    ///
    friend bool operator!=(oid const& lhs, oid const& rhs) {
        return lhs.compare(rhs) != 0;
    }

    ///
    /// Equivalent to `lhs.compare(rhs) < 0`.
    ///
    friend bool operator<(oid const& lhs, oid const& rhs) {
        return lhs.compare(rhs) < 0;
    }

    ///
    /// Equivalent to `lhs.compare(rhs) <= 0`.
    ///
    friend bool operator<=(oid const& lhs, oid const& rhs) {
        return lhs.compare(rhs) <= 0;
    }

    ///
    /// Equivalent to `lhs.compare(rhs) > 0`.
    ///
    friend bool operator>(oid const& lhs, oid const& rhs) {
        return lhs.compare(rhs) > 0;
    }

    ///
    /// Equivalent to `lhs.compare(rhs) >= 0`.
    ///
    friend bool operator>=(oid const& lhs, oid const& rhs) {
        return lhs.compare(rhs) >= 0;
    }

    ///
    /// Errors codes may be returned by @ref bsoncxx::v1::oid.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,           ///< Zero.
        null_bytes_ptr, ///< Bytes pointer must not be null.
        invalid_length, ///< Byte length must equal @ref bsoncxx::v1::oid::k_oid_length.
        empty_string,   ///< String must not be empty.
        invalid_string, ///< String is not a valid ObjectID representation.
    };

    ///
    /// The error category for @ref bsoncxx::v1::oid::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }
};

} // namespace v1
} // namespace bsoncxx

namespace std {

template <>
struct is_error_code_enum<bsoncxx::v1::oid::errc> : true_type {};

} // namespace std

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::oid.
///
