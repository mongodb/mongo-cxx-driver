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

#include <mongocxx/gridfs/downloader.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <exception>

#include <bsoncxx/types.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/config/private/prelude.hh>

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

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

downloader::downloader(stdx::optional<cursor> chunks, bsoncxx::document::value files_doc)
    : _files_doc(files_doc),
      _chunk_buffer_len(0),
      _chunk_buffer_offset(0),
      _chunks(chunks ? std::move(chunks) : stdx::nullopt),
      _chunks_curr(_chunks ? stdx::make_optional<cursor::iterator>(_chunks->begin())
                           : stdx::nullopt),
      _chunks_end(_chunks ? stdx::make_optional<cursor::iterator>(_chunks->end()) : stdx::nullopt),
      _chunks_seen(0),
      _chunk_size(read_chunk_size_from_files_document(files_doc.view())),
      _closed(false),
      _file_chunk_count(0),
      _file_len(read_length_from_files_document(files_doc.view())) {
    if (_chunk_size) {
        std::lldiv_t result = std::lldiv(_file_len, _chunk_size);

        if (result.quot >= std::numeric_limits<std::int32_t>::max()) {
            throw std::exception{};
        }

        _file_chunk_count = result.quot;

        if (result.rem) {
            ++_file_chunk_count;
        }
    }
}

downloader::downloader(downloader&&) noexcept = default;
downloader& downloader::operator=(downloader&&) = default;

std::size_t downloader::read(std::size_t length_requested, std::uint8_t* buffer) {
    if (_closed) {
        throw std::exception{};
    }

    if (_file_len == 0) {
        return 0;
    }

    std::size_t bytes_read = 0;

    while (length_requested > 0 &&
           (_chunks_seen != _file_chunk_count || _chunk_buffer_offset < _chunk_buffer_len)) {
        if (_chunk_buffer_offset == _chunk_buffer_len) {
            fetch_chunk();
        }

        std::size_t length = std::min(length_requested, _chunk_buffer_len - _chunk_buffer_offset);
        std::memcpy(buffer, &_chunk_buffer_ptr[_chunk_buffer_offset], length);
        buffer = &buffer[length];
        _chunk_buffer_offset += length;
        bytes_read += length;
        length_requested -= length;
    }

    return bytes_read;
}

void downloader::close() {
    if (_closed) {
        throw new std::exception{};
    }

    _chunks = {};
    _closed = true;
}

std::int32_t downloader::chunk_size() const {
    return _chunk_size;
}

bsoncxx::document::view downloader::files_document() const {
    return _files_doc.view();
}

void downloader::fetch_chunk() {
    if (_chunks_curr == _chunks_end) {
        throw std::exception{};
    }

    if (_chunks_seen) {
        ++(*_chunks_curr);
    }

    bsoncxx::document::view chunk_doc = **_chunks_curr;

    if (chunk_doc["n"].get_int32().value != _chunks_seen) {
        throw std::exception{};
    }

    if (_chunks_seen == std::numeric_limits<std::int32_t>::max()) {
        throw std::exception{};
    }

    ++_chunks_seen;

    auto binary_data = chunk_doc["data"].get_binary();

    if (_chunks_seen != _file_chunk_count) {
        if (binary_data.size != static_cast<std::uint32_t>(_chunk_size)) {
            throw std::exception{};
        }
    } else {
        auto expected_size = _file_len % static_cast<std::int64_t>(_chunk_size);

        if (expected_size == 0) {
            expected_size = static_cast<std::int64_t>(_chunk_size);
        }

        if (binary_data.size != static_cast<std::uint32_t>(expected_size)) {
            throw std::exception{};
        }
    }

    _chunk_buffer_ptr = binary_data.bytes;
    _chunk_buffer_len = binary_data.size;
    _chunk_buffer_offset = 0;
}

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
