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

#include <mongocxx/gridfs/uploader.hpp>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <exception>
#include <iomanip>
#include <ios>
#include <limits>
#include <sstream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace {
std::size_t chunks_collection_documents_max_length(std::size_t chunk_size) {
    // 16 * 1000 * 1000 is used instead of 16 * 1024 * 1024 to ensure that the command document sent
    // to the server has space for the other fields.
    return 16 * 1000 * 1000 / chunk_size;
}
}  // namespace

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

uploader::uploader(bsoncxx::types::value id,
                   stdx::string_view filename,
                   collection files,
                   collection chunks,
                   std::int32_t chunk_size,
                   stdx::optional<bsoncxx::document::view_or_value> metadata)
    : _buffer(stdx::make_unique<std::uint8_t[]>(chunk_size)),
      _buffer_off(0),
      _chunks(chunks),
      _chunk_size(chunk_size),
      _chunks_written(0),
      _closed(false),
      _filename(filename),
      _files(files),
      _metadata(metadata ? stdx::make_optional<bsoncxx::document::value>(
                               bsoncxx::document::value{metadata->view()})
                         : stdx::nullopt),
      _result(id) {
    md5_init(&_md5);
}

uploader::uploader(uploader&&) noexcept = default;
uploader& uploader::operator=(uploader&&) = default;

void uploader::write(std::size_t length, const std::uint8_t* bytes) {
    if (_closed) {
        throw std::exception{};
    }

    while (length > 0) {
        std::size_t buffer_free_space = static_cast<std::size_t>(_chunk_size) - _buffer_off;

        if (buffer_free_space == 0) {
            finish_chunk();
        }

        std::size_t length_written = std::min(length, buffer_free_space);
        std::memcpy(&_buffer.get()[_buffer_off], bytes, length_written);
        bytes = &bytes[length_written];
        _buffer_off += length_written;
        length -= length_written;
    }
}

result::gridfs::upload uploader::close() {
    using bsoncxx::builder::basic::kvp;

    if (_closed) {
        throw std::exception{};
    }

    _closed = true;

    bsoncxx::builder::basic::document file;

    std::int64_t bytes_uploaded =
        static_cast<std::int64_t>(_chunks_written) * static_cast<std::int64_t>(_chunk_size);
    std::int64_t leftover = _buffer_off;

    finish_chunk();
    flush_chunks();

    file.append(kvp("_id", _result.id()));
    file.append(kvp("length", bytes_uploaded + leftover));
    file.append(kvp("chunkSize", _chunk_size));
    file.append(kvp("uploadDate", bsoncxx::types::b_date{std::chrono::system_clock::now()}));

    md5_byte_t md5_hash_array[16];
    md5_finish(&_md5, md5_hash_array);

    std::stringstream md5_hash;

    for (auto i = 0; i < 16; ++i) {
        md5_hash << std::setfill('0') << std::setw(2) << std::hex
                 << static_cast<int>(md5_hash_array[i]);
    }

    file.append(kvp("md5", md5_hash.str()));
    file.append(kvp("filename", _filename));

    if (_metadata) {
        file.append(kvp("metadata", *_metadata));
    }

    _files.insert_one(file.extract());

    return _result;
}

void uploader::abort() {
    if (_closed) {
        throw std::exception{};
    }

    _closed = true;

    bsoncxx::builder::basic::document filter;
    filter.append(bsoncxx::builder::basic::kvp("files_id", _result.id()));

    _chunks.delete_many(filter.extract());
}

std::int32_t uploader::chunk_size() const {
    return _chunk_size;
}

void uploader::finish_chunk() {
    using bsoncxx::builder::basic::kvp;

    if (!_buffer_off) {
        return;
    }

    bsoncxx::builder::basic::document chunk;

    std::size_t bytes_in_chunk = _buffer_off;

    chunk.append(kvp("files_id", _result.id()));
    chunk.append(kvp("n", _chunks_written));

    if (_chunks_written == std::numeric_limits<std::int32_t>::max()) {
        throw std::exception{};
    }

    ++_chunks_written;

    bsoncxx::types::b_binary data{bsoncxx::binary_sub_type::k_binary,
                                  static_cast<std::uint32_t>(bytes_in_chunk),
                                  _buffer.get()};

    md5_append(&_md5, _buffer.get(), bytes_in_chunk);

    chunk.append(kvp("data", data));
    _chunks_collection_documents.push_back(chunk.extract());

    // To reduce the number of calls to the server, chunks are sent in batches rather than each one
    // being sent immediately upon being written.
    if (_chunks_collection_documents.size() >=
        chunks_collection_documents_max_length(_chunk_size)) {
        flush_chunks();
    }

    _buffer_off = 0;
}

void uploader::flush_chunks() {
    if (_chunks_collection_documents.empty()) {
        return;
    }

    _chunks.insert_many(_chunks_collection_documents);
    _chunks_collection_documents.clear();
}

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
