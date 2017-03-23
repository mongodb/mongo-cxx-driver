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

#include <cstdlib>

#include <mongocxx/gridfs/downloader.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

namespace {
std::int64_t read_length_from_files_document(bsoncxx::document::view files_doc) {
    if (files_doc["length"].type() == bsoncxx::type::k_int64) {
        return files_doc["length"].get_int64().value;
    }

    return files_doc["length"].get_int32().value;
}

std::int32_t read_chunk_size_from_files_document(bsoncxx::document::view files_doc) {
    const std::int64_t k_max_document_size = 16 * 1024 * 1024;
    std::int64_t chunk_size;

    if (files_doc["chunkSize"].type() == bsoncxx::type::k_int64) {
        chunk_size = files_doc["chunkSize"].get_int64().value;
    } else {
        chunk_size = files_doc["chunkSize"].get_int32().value;
    }

    // Each chunk needs to be able to fit in a single document.
    if (chunk_size > k_max_document_size) {
        throw std::exception{};
    }

    return static_cast<std::int32_t>(chunk_size);
}
}  // namespace

class downloader::impl {
   public:
    impl(stdx::optional<cursor> chunks_param, bsoncxx::document::value files_doc_param)
        : files_doc{std::move(files_doc_param)},
          chunk_buffer_len{0},
          chunk_buffer_offset{0},
          chunk_buffer_ptr{nullptr},
          chunks{chunks_param ? std::move(chunks_param) : stdx::nullopt},
          chunks_curr{chunks ? stdx::make_optional<cursor::iterator>(chunks->begin())
                             : stdx::nullopt},
          chunks_end{chunks ? stdx::make_optional<cursor::iterator>(chunks->end()) : stdx::nullopt},
          chunks_seen{0},
          chunk_size{read_chunk_size_from_files_document(files_doc.view())},
          closed{false},
          file_chunk_count{0},
          file_len{read_length_from_files_document(files_doc.view())} {
        if (chunk_size) {
            std::lldiv_t result = std::lldiv(file_len, chunk_size);

            if (result.quot >= std::numeric_limits<std::int32_t>::max()) {
                throw std::exception{};
            }

            file_chunk_count = result.quot;

            if (result.rem) {
                ++file_chunk_count;
            }
        }
    }

    // The files document for the file being downloaded.
    bsoncxx::document::value files_doc;

    // The number of bytes in the current chunk.
    std::size_t chunk_buffer_len;

    // The offset from `chunk_buffer_ptr` to the next byte to be read.
    std::size_t chunk_buffer_offset;

    // A pointer to the current chunk being read.
    const uint8_t* chunk_buffer_ptr;

    // A cursor iterating over the chunks documents being read. In the case of a zero-length file,
    // this member does not have a value.
    stdx::optional<cursor> chunks;

    // An iterator to the current chunk document. In the case of a zero-length file, this member
    // does not have a value.
    stdx::optional<cursor::iterator> chunks_curr;

    // An iterator to the end of `chunks`. In the case of a zero-length file, this member does not
    // have a value.
    stdx::optional<cursor::iterator> chunks_end;

    // The number of chunks already downloaded from the server.
    std::int32_t chunks_seen;

    // The size of a chunk in bytes.
    std::int32_t chunk_size;

    // Whether or not the downloader has already been closed.
    bool closed;

    // The total number of chunks in the file.
    std::int32_t file_chunk_count;

    // The total length of the file in bytes.
    std::int64_t file_len;
};

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
