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
#include <cstring>
#include <exception>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/gridfs/private/downloader.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

downloader::downloader(stdx::optional<cursor> chunks, bsoncxx::document::value files_doc)
    : _impl{stdx::make_unique<impl>(std::move(chunks), std::move(files_doc))} {}

downloader::downloader() noexcept = default;
downloader::downloader(downloader&&) noexcept = default;
downloader& downloader::operator=(downloader&&) noexcept = default;
downloader::~downloader() = default;

downloader::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

std::size_t downloader::read(std::size_t length_requested, std::uint8_t* buffer) {
    if (_get_impl().closed) {
        throw std::exception{};
    }

    if (_get_impl().file_len == 0) {
        return 0;
    }

    std::size_t bytes_read = 0;

    while (length_requested > 0 &&
           (_get_impl().chunks_seen != _get_impl().file_chunk_count ||
            _get_impl().chunk_buffer_offset < _get_impl().chunk_buffer_len)) {
        if (_get_impl().chunk_buffer_offset == _get_impl().chunk_buffer_len) {
            fetch_chunk();
        }

        std::size_t length = std::min(
            length_requested, _get_impl().chunk_buffer_len - _get_impl().chunk_buffer_offset);
        std::memcpy(buffer, &_get_impl().chunk_buffer_ptr[_get_impl().chunk_buffer_offset], length);
        buffer = &buffer[length];
        _get_impl().chunk_buffer_offset += length;
        bytes_read += length;
        length_requested -= length;
    }

    return bytes_read;
}

void downloader::close() {
    if (_get_impl().closed) {
        throw new std::exception{};
    }

    _get_impl().chunks = {};
    _get_impl().closed = true;
}

std::int32_t downloader::chunk_size() const {
    return _get_impl().chunk_size;
}

bsoncxx::document::view downloader::files_document() const {
    return _get_impl().files_doc.view();
}

void downloader::fetch_chunk() {
    if (_get_impl().chunks_curr == _get_impl().chunks_end) {
        throw std::exception{};
    }

    if (_get_impl().chunks_seen) {
        ++(*_get_impl().chunks_curr);
    }

    bsoncxx::document::view chunk_doc = **_get_impl().chunks_curr;

    if (chunk_doc["n"].get_int32().value != _get_impl().chunks_seen) {
        throw std::exception{};
    }

    if (_get_impl().chunks_seen == std::numeric_limits<std::int32_t>::max()) {
        throw std::exception{};
    }

    ++_get_impl().chunks_seen;

    auto binary_data = chunk_doc["data"].get_binary();

    if (_get_impl().chunks_seen != _get_impl().file_chunk_count) {
        if (binary_data.size != static_cast<std::uint32_t>(_get_impl().chunk_size)) {
            throw std::exception{};
        }
    } else {
        auto expected_size =
            _get_impl().file_len % static_cast<std::int64_t>(_get_impl().chunk_size);

        if (expected_size == 0) {
            expected_size = static_cast<std::int64_t>(_get_impl().chunk_size);
        }

        if (binary_data.size != static_cast<std::uint32_t>(expected_size)) {
            throw std::exception{};
        }
    }

    _get_impl().chunk_buffer_ptr = binary_data.bytes;
    _get_impl().chunk_buffer_len = binary_data.size;
    _get_impl().chunk_buffer_offset = 0;
}

const downloader::impl& downloader::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_gridfs_downloader_object};
    }
    return *_impl;
}

downloader::impl& downloader::_get_impl() {
    auto cthis = const_cast<const downloader*>(this);
    return const_cast<downloader::impl&>(cthis->_get_impl());
}

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
