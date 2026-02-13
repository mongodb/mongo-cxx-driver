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

#include <mongocxx/v1/gridfs/downloader.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/cursor.hpp>

#include <mongocxx/v1/exception.hh>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <system_error>

#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

using code = v1::gridfs::downloader::errc;

namespace {

std::int32_t compute_total_chunk_count(std::int64_t file_len, std::int32_t chunk_size) {
    auto div = std::lldiv(file_len, chunk_size);

    if (div.rem) {
        ++div.quot;
        div.rem = 0;
    }

    if (div.quot > std::numeric_limits<std::int32_t>::max()) {
        std::string msg;

        msg += "file has ";
        msg += std::to_string(div.quot);
        msg += " chunks, which exceeds maximum of ";
        msg += std::to_string(std::numeric_limits<std::int32_t>::max());

        throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
    }

    return static_cast<std::int32_t>(div.quot);
}

} // namespace

class downloader::impl {
   public:
    // Downloader initial state.
    bsoncxx::v1::stdx::optional<v1::cursor> _chunks_cursor;
    bsoncxx::v1::document::value _files_doc; // The collection document describing the stored file.
    std::int64_t _file_length = {};          // Size (in bytes) of the stored file.
    std::int32_t _chunk_size = {};           // Size (in bytes) of each chunk.
    std::int32_t _total_chunk_count = {};    // Number of chunks representing the stored file.
    bool _closed = false;

    // Initial chunk and byte offsets.
    std::int32_t _initial_chunk_number = {}; // The chunk number from which to start reading.
    std::int32_t _initial_byte_offset = {};  // The byte offset within chunk data from which to start reading.

    // Chunk iteration state.
    v1::cursor::iterator _chunks_iter; // Iterator to the current chunk.
    v1::cursor::iterator _chunks_end;
    std::int32_t _next_chunk_number = {};     // Total chunks downloaded so far.
    std::uint8_t const* _chunk_data_ptr = {}; // Pointer to to the current chunk data.
    std::size_t _chunk_data_len = {};         // Length of the current chunk data.
    std::size_t _chunk_data_offset = {};      // Offset from `chunk_buffer_ptr` to the next byte to read.

    impl() = default;

    impl(
        v1::cursor chunks,
        bsoncxx::v1::document::value files_doc,
        std::int64_t file_length,
        std::int32_t chunk_size,
        std::int32_t initial_chunk_number,
        std::int32_t initial_byte_offset)
        : _chunks_cursor{std::move(chunks)},
          _files_doc{std::move(files_doc)},
          _file_length{file_length},
          _chunk_size{chunk_size},
          _total_chunk_count{_chunk_size > 0 ? compute_total_chunk_count(_file_length, _chunk_size) : 0},
          _initial_chunk_number{initial_chunk_number},
          _initial_byte_offset{initial_byte_offset},
          _chunks_iter{_chunks_cursor ? _chunks_cursor->begin() : v1::cursor::iterator{}} {}

    static impl const& with(downloader const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(downloader const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(downloader& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(downloader* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

downloader::~downloader() {
    delete impl::with(_impl);
}

downloader::downloader(downloader&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

downloader& downloader::operator=(downloader&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

downloader::downloader() : _impl{nullptr} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

downloader::operator bool() const {
    return _impl != nullptr;
}

bool downloader::is_open() const {
    return !impl::with(this)->_closed;
}

void downloader::close() {
    impl::with(this)->_closed = true;
}

std::int32_t downloader::chunk_size() const {
    return impl::with(this)->_chunk_size;
}

std::int64_t downloader::file_length() const {
    return impl::with(this)->_file_length;
}

bsoncxx::v1::document::view downloader::files_document() const {
    return impl::with(this)->_files_doc;
}

std::size_t downloader::read(std::uint8_t* data, std::size_t length) {
    auto& impl = *downloader::impl::with(this);

    if (impl._closed) {
        throw v1::exception::internal::make(code::is_closed);
    }

    // Nothing to read.
    if (impl._file_length == 0) {
        return 0u;
    }

    auto const total_chunk_count = impl._total_chunk_count;

    std::size_t actual_bytes_read = 0u;

    // While there are still bytes to read...
    while (length > 0u) {
        // ... and there are still bytes left to be read.
        if (impl._next_chunk_number >= total_chunk_count && impl._chunk_data_offset >= impl._chunk_data_len) {
            break; // All available chunks and available bytes in current chunk have been read.
        }

        // When no more bytes remain in the current chunk, download the next chunk.
        if (impl._chunk_data_offset >= impl._chunk_data_len) {
            this->download_next_chunk();
        }

        auto const chunk_data_ptr = impl._chunk_data_ptr;
        auto const chunk_data_len = impl._chunk_data_len;
        auto const chunk_data_offset = impl._chunk_data_offset;

        // Read the next set of available bytes.
        auto const available_bytes = std::min(length, chunk_data_len - chunk_data_offset);
        std::memcpy(data, chunk_data_ptr + chunk_data_offset, available_bytes);

        // Shift all iterators and increment counters for the next iteration.
        data += available_bytes;
        impl._chunk_data_offset = chunk_data_offset + available_bytes;
        actual_bytes_read += available_bytes;
        length -= available_bytes;
    }

    return actual_bytes_read;
}

std::error_category const& downloader::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::gridfs::downloader";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::is_closed:
                    return "the GridFS file download stream is not open";
                case code::corrupt_data:
                    return "the GridFS file is in an invalid or inconsistent state";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::is_closed:
                    case code::corrupt_data:
                        return source == condition::mongocxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::is_closed:
                    case code::corrupt_data:
                        return type == condition::runtime_error;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

downloader::downloader(void* impl) : _impl{impl} {}

void downloader::download_next_chunk() {
    auto& impl = *impl::with(this);

    auto& chunks_iter = impl._chunks_iter;
    auto const& chunks_end = impl._chunks_end;

    auto const total_chunk_count = compute_total_chunk_count(impl._file_length, impl._chunk_size);
    auto const next_chunk_number = impl._next_chunk_number;

    if (chunks_iter == chunks_end) {
        std::string msg;

        msg += "expected file to have ";
        msg += std::to_string(total_chunk_count);
        msg += " chunk(s), but query to chunks collection only returned ";
        msg += std::to_string(next_chunk_number);
        msg += " chunk(s)";

        throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
    }

    if (next_chunk_number > 0) {
        ++chunks_iter; // Download the next chunk.
    } else {
        // Cursor already obtained the first chunk.
    }

    auto const current_chunk_n = next_chunk_number > 0 ? next_chunk_number : impl._initial_chunk_number;
    auto const current_chunk_doc = *chunks_iter;

    // Validate chunk offset.
    {
        auto const n = current_chunk_doc["n"];

        if (!n || n.type_id() != bsoncxx::v1::types::id::k_int32 || n.get_int32().value != current_chunk_n) {
            std::string msg;

            msg += "chunk #";
            msg += std::to_string(current_chunk_n);
            msg += ": expected to find field \"n\" with k_int32 type";

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        }

        if (current_chunk_n == std::numeric_limits<std::int32_t>::max()) {
            throw v1::exception::internal::make(code::corrupt_data, "file has too many chunks");
        }
    }

    auto const data = current_chunk_doc["data"];

    // Validate chunk data.
    {
        if (!data || data.type_id() != bsoncxx::v1::types::id::k_binary) {
            std::string msg;

            msg += "chunk #";
            msg += std::to_string(current_chunk_n);
            msg += ": expected to find field \"data\" with k_binary type";

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        }
    }

    auto const binary_data = data.get_binary();

    auto const file_length = impl._file_length;
    auto const chunk_size = impl._chunk_size;

    // Validate: more chunks available.
    if (current_chunk_n < total_chunk_count - 1) {
        if (binary_data.size != static_cast<std::uint32_t>(chunk_size)) {
            std::string msg;

            msg += "chunk #";
            msg += std::to_string(current_chunk_n);
            msg += ": expected size of chunk to be ";
            msg += std::to_string(chunk_size);
            msg += " bytes, but actual size of chunk is ";
            msg += std::to_string(binary_data.size);
            msg += " bytes";

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        }
    }

    // Validate: last available chunk.
    else {
        auto const remainder = file_length % chunk_size;
        auto const expected_size = remainder > 0 ? remainder : chunk_size;

        if (binary_data.size != static_cast<std::uint32_t>(expected_size)) {
            std::string msg;

            msg += "chunk #";
            msg += std::to_string(current_chunk_n);
            msg += ": expected size of chunk to be ";
            msg += std::to_string(expected_size);
            msg += " bytes, but actual size of chunk is ";
            msg += std::to_string(binary_data.size);
            msg += " bytes";

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        }
    }

    impl._chunk_data_ptr = binary_data.bytes;
    impl._chunk_data_len = binary_data.size;

    if (next_chunk_number == 0) {
        auto const initial_byte_offset = impl._initial_byte_offset;

        if (initial_byte_offset < 0) {
            throw v1::exception::internal::make(code::corrupt_data, "expected bytes offset to be in bounds of size_t");
        }

        impl._chunk_data_offset = static_cast<std::size_t>(initial_byte_offset);
        impl._next_chunk_number = current_chunk_n + 1;
    } else {
        impl._chunk_data_offset = 0;
        impl._next_chunk_number = next_chunk_number + 1;
    }
}

downloader downloader::internal::make() {
    return {new impl{}};
}

downloader downloader::internal::make(
    v1::cursor cursor,
    bsoncxx::v1::document::value files_doc,
    std::int64_t file_length,
    std::int32_t chunk_size,
    std::int32_t initial_chunk_number,
    std::int32_t initial_byte_offset) {
    return {new impl{
        std::move(cursor), std::move(files_doc), file_length, chunk_size, initial_chunk_number, initial_byte_offset}};
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
