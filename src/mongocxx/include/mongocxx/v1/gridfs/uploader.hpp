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

#include <mongocxx/v1/gridfs/uploader-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/gridfs/upload_result-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstddef>
#include <cstdint>
#include <system_error>
#include <type_traits>

namespace mongocxx {
namespace v1 {
namespace gridfs {

///
/// Support for uploading files to a GridFS bucket.
///
/// @see
/// - [GridFS (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class uploader {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// Calls @ref close(). Any exceptions are caught and ignored.
    ///
    /// @note When a server-side error is encountered, already-uploaded chunks are "orphaned" and no cleanup attempt is
    /// made.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~uploader();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() uploader(uploader&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(uploader&) operator=(uploader&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    uploader(uploader const& other) = delete;

    ///
    /// This class is not copyable.
    ///
    uploader& operator=(uploader const& other) = delete;

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() uploader();

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state and `this->is_open() == true`.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Return true when the underlying GridFS file stream is open for writing.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) is_open() const;

    ///
    /// Complete the upload of this GridFS file.
    ///
    /// @note When a server-side error is encountered, already-uploaded chunks are "orphaned" and no cleanup attempt is
    /// made.
    ///
    /// @returns The files collection document for the successfully uploaded GridFS file.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::upload_result) close();

    ///
    /// Abort uploading this GridFS file.
    ///
    /// @note When a server-side error is encountered, already-uploaded chunks are "orphaned" and no cleanup attempt is
    /// made.
    ///
    /// @par Postconditions:
    /// - All uploaded chunks for this GridFS file are deleted, unless a server-side error is encountered.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::uploader::errc::is_closed if the
    /// underlying GridFS download stream was already closed.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) abort();

    ///
    /// Return the chunk size (in bytes) of the associated GridFS file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) chunk_size() const;

    ///
    /// Write `length` bytes of the associated GridFS file.
    ///
    /// An internal buffer is used to reduce the total number of collection write operations. Use @ref flush() to
    /// manually flush the internal buffer.
    ///
    /// @note When a server-side error is encountered, already-uploaded chunks are "orphaned" and no cleanup attempt is
    /// made.
    ///
    /// @par Preconditions:
    /// - `data` is not null.
    /// - The size of the storage region pointed to by `data` must be greater than or equal to `length`.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::uploader::errc::is_closed if the
    /// underlying GridFS download stream was already closed.
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::uploader::errc::too_many_chunks if the
    /// total number of chunks would be greater than or equal to INT32_MAX.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) write(std::uint8_t const* data, std::size_t length);

    ///
    /// Flush the internal buffer.
    ///
    /// @note When a server-side error is encountered, already-uploaded chunks are "orphaned" and no cleanup attempt is
    /// made.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::uploader::errc::is_closed if the
    /// underlying GridFS download stream was already closed.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) flush();

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::gridfs::uploader.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,            ///< Zero.
        is_closed,       ///< The GridFS file upload stream is not open.
        too_many_chunks, ///< The total number of chunks must be less than INT32_MAX.
    };

    ///
    /// The error category for @ref mongocxx::v1::gridfs::uploader::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }

    class internal;

   private:
    /* explicit(false) */ uploader(void* impl);

    void save_chunk();
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::gridfs::uploader::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::gridfs::uploader.
///
