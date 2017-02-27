// Copyright 2017 MongoDB Inc.
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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

///
/// Class used to download a GridFS file.
///
class MONGOCXX_API downloader {
   public:
    ///
    /// Move constructs a downloader.
    ///
    downloader(downloader&&) noexcept;

    ///
    /// Move assigns a downloader.
    ///
    /// TODO: CXX-1235 Add noexcept specifier.
    ///
    downloader& operator=(downloader&&);

    downloader(const downloader&) = delete;

    downloader& operator=(const downloader&) = delete;

    ///
    /// Reads a specified number of bytes from the GridFS file being downloaded.
    ///
    /// @param length
    ///   The number of bytes to read from the file.
    ///
    /// @param buffer
    ///   A pointer to a buffer to store the bytes read from the file.
    ///
    /// @return
    ///   The number of bytes actually read. If zero, the downloader has reached the end of the
    ///   file.
    ///
    /// @throws an exception if an invalid chunk is downloaded, if the file ends unexpectedly, or if
    /// the stream has been closed.
    ///
    std::size_t read(std::size_t length, std::uint8_t* buffer);

    ///
    /// Closes the downloader stream.
    ///
    /// @throws if the downloader stream was already closed.
    ///
    void close();

    ///
    /// Gets the chunk size of the file being downloaded.
    ///
    /// @return
    ///   The chunk size in bytes.
    ///
    std::int32_t chunk_size() const;

    ///
    /// Gets the files collection document of the file being downloaded.
    ///
    /// @return
    ///    A view to the files collection document of the file being downloaded.
    ///
    bsoncxx::document::view files_document() const;

   private:
    friend class bucket;

    //
    // Constructs a new downloader stream.
    //
    // @param chunks
    //   The cursor to read the chunks of the file from. It must have a value if the length of the
    //   file is non-zero.
    //
    // @param files_doc
    //   The files collection document of the file being downloaded.
    //
    MONGOCXX_PRIVATE downloader(stdx::optional<cursor> chunks, bsoncxx::document::value files_doc);

    MONGOCXX_PRIVATE void fetch_chunk();

    // The files document for the file being downloaded.
    bsoncxx::document::value _files_doc;

    // The number of bytes in the current chunk.
    std::size_t _chunk_buffer_len;

    // The offset from _buffer_ptr to the next byte to be read.
    std::size_t _chunk_buffer_offset;

    // A pointer to the current chunk being read.
    const uint8_t* _chunk_buffer_ptr;

    // A cursor iterating over the chunks documents being read. In the case of a zero-length file,
    // this member does not have a value.
    stdx::optional<cursor> _chunks;

    // An iterator to the current chunk document. In the case of a zero-length file, this member
    // does not have a value.
    stdx::optional<cursor::iterator> _chunks_curr;

    // An iterator to the end of _chunks. In the case of a zero-length file, this member does not
    // have a value.
    stdx::optional<cursor::iterator> _chunks_end;

    // The number of chunks already downloaded from the server.
    std::int32_t _chunks_seen;

    // The size of a chunk in bytes.
    std::int32_t _chunk_size;

    // Whether or not the downloader has already been closed.
    bool _closed;

    // The total number of chunks in the file.
    std::int32_t _file_chunk_count;

    // The total length of the file in bytes.
    std::int64_t _file_len;
};

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
