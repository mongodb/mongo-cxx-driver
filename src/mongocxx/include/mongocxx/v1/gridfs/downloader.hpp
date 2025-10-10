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

#include <mongocxx/v1/gridfs/downloader-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <system_error>
#include <type_traits>

namespace mongocxx {
namespace v1 {
namespace gridfs {

///
/// Support for downloading files from a GridFS bucket.
///
/// @see
/// - [GridFS (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class downloader {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~downloader();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() downloader(downloader&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(downloader&) operator=(downloader&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    downloader(downloader const& other) = delete;

    ///
    /// This class is not copyable.
    ///
    downloader& operator=(downloader const& other) = delete;

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() downloader();

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state and `this->is_open() == true`.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Return true when the underlying GridFS file stream is open for reading.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) is_open() const;

    ///
    /// Close the underlying GridFS download stream.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) close();

    ///
    /// Return the chunk size (in bytes) of the associated GridFS file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) chunk_size() const;

    ///
    /// Return the length of the associated GridFS file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) file_length() const;

    ///
    /// Return the files collection document for the associated GridFS file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) files_document() const;

    ///
    /// Read up to `length` bytes of the associated GridFS file.
    ///
    /// @par Preconditions:
    /// - `data` is not null.
    /// - The size of the storage region pointed to by `data` must be greater than or equal to `length`.
    ///
    /// @return The actual number of bytes read. `0` indicates the downloader has reached the end of the file.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::downloader::errc::is_closed if the
    /// underlying GridFS download stream was already closed.
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::downloader::errc::corrupt_data if the
    /// GridFS file data is invalid or inconsistent.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::size_t) read(std::uint8_t* data, std::size_t length);

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::gridfs::downloader.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,         ///< Zero.
        is_closed,    ///< The GridFS file download stream is not open.
        corrupt_data, ///< The GridFS file is in an invalid or inconsistent state.
    };

    ///
    /// The error category for @ref mongocxx::v1::gridfs::downloader::errc.
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
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::gridfs::downloader::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::gridfs::downloader.
///
