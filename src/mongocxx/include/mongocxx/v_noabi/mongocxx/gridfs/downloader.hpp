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

#include <mongocxx/gridfs/downloader-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/gridfs/downloader.hpp> // IWYU pragma: export

#include <cstddef>
#include <cstdint>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <mongocxx/gridfs/bucket-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>         // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/bson_value/view.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/cursor.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

///
/// Used to specify the offset from which to start reading the chunks of the file.
///
struct chunks_and_bytes_offset {
    std::int32_t chunks_offset = 0;
    std::int32_t bytes_offset = 0;
};

///
/// Used to download a GridFS file.
///
class downloader {
   private:
    v1::gridfs::downloader _downloader;

   public:
    ///
    /// Default constructs a downloader object. The downloader is equivalent to the state of a moved
    /// from downloader. The only valid actions to take with a default constructed downloader are to
    /// assign to it, or destroy it.
    ///
    downloader() noexcept {}

    ///
    /// Move constructs a downloader.
    ///
    downloader(downloader&& other) noexcept = default;

    ///
    /// Move assigns a downloader.
    ///
    downloader& operator=(downloader&& other) noexcept = default;

    downloader(downloader const& other) = delete;
    downloader& operator=(downloader const& other) = delete;

    ///
    /// Destroys a downloader.
    ///
    ~downloader() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ downloader(v1::gridfs::downloader downloader) : _downloader{std::move(downloader)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated objects.
    ///
    explicit operator v1::gridfs::downloader() && {
        return std::move(_downloader);
    }

    ///
    /// This class is not copyable.
    ///
    explicit operator v1::gridfs::downloader() const& = delete;

    ///
    /// Returns true if the downloader is valid, meaning it was not default constructed or moved
    /// from.
    ///
    explicit operator bool() const noexcept {
        return _downloader.operator bool();
    }

    ///
    /// Reads a specified number of bytes from the GridFS file being downloaded.
    ///
    /// @param buffer
    ///   A pointer to a buffer to store the bytes read from the file.
    ///
    /// @param length
    ///   The number of bytes to read from the file.
    ///
    /// @return
    ///   The number of bytes actually read. If zero, the downloader has reached the end of the
    ///   file.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the download stream was already closed.
    ///
    /// @throws mongocxx::v_noabi::gridfs_exception if the requested file has been corrupted.
    ///
    /// @throws mongocxx::v_noabi::query_exception
    ///   if an error occurs when reading chunk data from the database for the requested file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::size_t) read(std::uint8_t* buffer, std::size_t length);

    ///
    /// Closes the downloader stream.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the download stream was already closed.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) close();

    ///
    /// Gets the chunk size of the file being downloaded.
    ///
    /// @return
    ///   The chunk size in bytes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) chunk_size() const;

    ///
    /// Gets the length of the file being downloaded.
    ///
    /// @return
    ///   The length in bytes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) file_length() const;

    ///
    /// Gets the files collection document of the file being downloaded.
    ///
    /// @return
    ///    A view to the files collection document of the file being downloaded.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) files_document() const;

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
inline v_noabi::gridfs::downloader from_v1(v1::gridfs::downloader v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::gridfs::downloader to_v1(v_noabi::gridfs::downloader v) {
    return v1::gridfs::downloader{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides utilities to download GridFS files.
///
/// @par Includes
/// - @ref mongocxx/v1/gridfs/downloader.hpp
///
