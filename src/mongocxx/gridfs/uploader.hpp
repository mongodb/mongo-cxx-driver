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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/result/gridfs/upload.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/third_party/md5.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

///
/// Class used to upload a GridFS file.
///
class MONGOCXX_API uploader {
   public:
    ///
    /// Move constructs an uploader.
    ///
    uploader(uploader&&) noexcept;

    ///
    /// Move assigns an uploader.
    ///
    /// TODO: CXX-1235 Add noexcept specifier.
    ///
    uploader& operator=(uploader&&);

    uploader(const uploader&) = delete;

    uploader& operator=(const uploader&) = delete;

    ///
    /// Writes a specified number of bytes to a GridFS file.
    ///
    /// @param length
    ///   The number of bytes to write.
    ///
    /// @param bytes
    ///   A pointer to the bytes to write.
    ///
    /// @throws an exception if the upload stream was already closed or `bytes` is null or if an
    /// error occurs when writing data to the database.
    ///
    void write(std::size_t length, const std::uint8_t* bytes);

    ///
    /// Closes the uploader stream.
    ///
    /// @throws if the uploader stream was already closed or if an error occurs when writing data to
    /// the database.
    ///
    result::gridfs::upload close();

    ///
    /// Aborts uploading the file.
    ///
    /// @throws if the uploader is unable to be aborted.
    ///
    void abort();

    ///
    /// Gets the chunk size of the file being uploaded.
    ///
    /// @return
    ///   The chunk size in bytes.
    ///
    std::int32_t chunk_size() const;

   private:
    friend class bucket;

    //
    // Constructs a new uploader stream.
    //
    // @param id
    //   The id of the GridFS file being uploaded.
    //
    // @param files
    //   The files collection of the bucket receiving the file.
    //
    // @param chunks
    //   The chunks collection of the bucket receiving the file.
    //
    // @param chunk_size
    //   The size in bytes of the chunks being uploaded.
    //
    // @param metadata
    //   Optional metadata field of the files collection document.
    //
    MONGOCXX_PRIVATE uploader(bsoncxx::types::value id,
                              stdx::string_view filename,
                              collection files,
                              collection chunks,
                              std::int32_t chunk_size,
                              stdx::optional<bsoncxx::document::view_or_value> metadata = {});

    MONGOCXX_PRIVATE void finish_chunk();
    MONGOCXX_PRIVATE void flush_chunks();

    // Bytes that have been written for the current chunk.
    std::unique_ptr<std::uint8_t[]> _buffer;

    // The offset from _buffer to the next byte to be written.
    std::size_t _buffer_off;

    // The collection to which the chunks will be written.
    collection _chunks;

    // Chunks that have been fully written but not yet uploaded to the server.
    std::vector<bsoncxx::document::value> _chunks_collection_documents;

    // The size of a chunk in bytes.
    std::int32_t _chunk_size;

    // The number of chunks fully written so far.
    std::int32_t _chunks_written;

    // Whether or not the uploader has already been closed.
    bool _closed;

    // The name of the file to be written.
    std::string _filename;

    // The collection to which the files document will be written.
    collection _files;

    // Keeps track of the md5 hash of the file.
    md5_state_t _md5;

    // User-specified metadata for the file.
    stdx::optional<bsoncxx::document::value> _metadata;

    // Contains the id of the file being written.
    result::gridfs::upload _result;
};

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
