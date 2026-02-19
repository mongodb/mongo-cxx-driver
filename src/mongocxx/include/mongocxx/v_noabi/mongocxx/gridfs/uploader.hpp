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

#include <mongocxx/gridfs/uploader-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/gridfs/uploader.hpp> // IWYU pragma: export

#include <cstddef>
#include <cstdint>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <mongocxx/gridfs/bucket-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/value.hpp>        // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/stdx/optional.hpp>         // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/stdx/string_view.hpp>      // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/bson_value/view.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/view_or_value.hpp>         // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/client_session.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/collection.hpp>     // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/result/gridfs/upload.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

///
/// Used to upload a GridFS file.
///
class uploader {
   private:
    v1::gridfs::uploader _uploader;

   public:
    ///
    /// Default constructs an uploader object. The uploader is equivalent to the state of a moved
    /// from uploader. The only valid actions to take with a default constructed uploader are to
    /// assign to it, or destroy it.
    ///
    uploader() noexcept {}

    ///
    /// Move constructs an uploader.
    ///
    uploader(uploader&&) noexcept = default;

    ///
    /// Move assigns an uploader.
    ///
    uploader& operator=(uploader&&) noexcept = default;

    uploader(uploader const& other) = delete;
    uploader& operator=(uploader const& other) = delete;

    ///
    /// Destroys an uploader.
    ///
    ~uploader() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ uploader(v1::gridfs::uploader uploader) : _uploader{std::move(uploader)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated objects.
    ///
    explicit operator v1::gridfs::uploader() && {
        return std::move(_uploader);
    }

    ///
    /// This class is not copyable.
    ///
    explicit operator v1::gridfs::uploader() const& = delete;

    ///
    /// Returns true if the uploader is valid, meaning it was not default constructed or moved from.
    ///
    explicit operator bool() const noexcept {
        return _uploader.operator bool();
    }

    ///
    /// Writes a specified number of bytes to a GridFS file.
    ///
    /// @param bytes
    ///   A pointer to the bytes to write.
    ///
    /// @param length
    ///   The number of bytes to write.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the upload stream was already closed.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception
    ///   if an error occurs when writing chunk data to the database.
    ///
    /// @throws mongocxx::v_noabi::gridfs_exception
    ///   if the uploader requires more than 2^31-1 chunks to store the file at the requested chunk
    ///   size.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) write(std::uint8_t const* bytes, std::size_t length);

    ///
    /// Closes the uploader stream.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the upload stream was already closed.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception
    ///   if an error occurs when writing chunk data or file metadata to the database.
    ///
    /// @throws mongocxx::v_noabi::gridfs_exception
    ///   if the uploader requires more than 2^31-1 chunks to store the file at the requested chunk
    ///   size.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(result::gridfs::upload) close();

    ///
    /// Aborts uploading the file.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the upload stream was already closed.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception
    ///   if an error occurs when removing chunk data from the database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) abort();

    ///
    /// Gets the chunk size of the file being uploaded.
    ///
    /// @return
    ///   The chunk size in bytes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) chunk_size() const;

    class internal;
};

} // namespace gridfs
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::gridfs::uploader from_v1(v1::gridfs::uploader v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::gridfs::uploader to_v1(v_noabi::gridfs::uploader v) {
    return v1::gridfs::uploader{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides utilities to upload GridFS files.
///
/// @par Includes
/// - @ref mongocxx/v1/gridfs/uploader.hpp
///
