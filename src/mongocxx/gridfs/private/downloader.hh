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

#include <mongocxx/exception/gridfs_exception.hpp>
#include <mongocxx/gridfs/downloader.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

namespace {
std::int64_t read_length_from_files_document(bsoncxx::document::view files_doc) {
    auto length_ele = files_doc["length"];
    std::int64_t length;
    if (length_ele && length_ele.type() == bsoncxx::type::k_int64) {
        length = length_ele.get_int64().value;
    } else if (length_ele && length_ele.type() == bsoncxx::type::k_int32) {
        length = length_ele.get_int32().value;
    } else {
        throw gridfs_exception{error_code::k_gridfs_file_corrupted,
                               "expected files document to contain field \"length\" with type "
                               "k_int32 or k_int64"};
    }

    if (length < 0) {
        std::ostringstream err;
        err << "files document contains unexpected negative value for \"length\": " << length;
        throw gridfs_exception{error_code::k_gridfs_file_corrupted, err.str()};
    }

    return length;
}

std::int32_t read_chunk_size_from_files_document(bsoncxx::document::view files_doc) {
    const std::int64_t k_max_document_size = 16 * 1024 * 1024;
    std::int64_t chunk_size;

    auto chunk_size_ele = files_doc["chunkSize"];

    if (chunk_size_ele && chunk_size_ele.type() == bsoncxx::type::k_int64) {
        chunk_size = chunk_size_ele.get_int64().value;
    } else if (chunk_size_ele && chunk_size_ele.type() == bsoncxx::type::k_int32) {
        chunk_size = chunk_size_ele.get_int32().value;
    } else {
        throw gridfs_exception{error_code::k_gridfs_file_corrupted,
                               "expected files document to contain field \"chunkSize\" with type "
                               "k_int32 or k_int64"};
    }

    // Each chunk needs to be able to fit in a single document.
    if (chunk_size > k_max_document_size) {
        std::ostringstream err;
        err << "files document contains unexpected chunk size of " << chunk_size
            << ", which exceeds maximum chunk size of " << k_max_document_size;
        throw gridfs_exception{error_code::k_gridfs_file_corrupted, err.str()};
    } else if (chunk_size <= 0) {
        std::ostringstream err;
        err << "files document contains unexpected chunk size: " << chunk_size
            << "; value must be positive";
        throw gridfs_exception{error_code::k_gridfs_file_corrupted, err.str()};
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
            std::lldiv_t num_chunks_div = std::lldiv(file_len, chunk_size);
            if (num_chunks_div.rem) {
                ++num_chunks_div.quot;
                num_chunks_div.rem = 0;
            }

            if (num_chunks_div.quot > std::numeric_limits<std::int32_t>::max()) {
                std::ostringstream err;
                err << "file has " << num_chunks_div.quot << " chunks, which exceeds maximum of "
                    << std::numeric_limits<std::int32_t>::max();
                throw gridfs_exception{error_code::k_gridfs_file_corrupted, err.str()};
            }

            file_chunk_count = static_cast<std::int32_t>(num_chunks_div.quot);
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
