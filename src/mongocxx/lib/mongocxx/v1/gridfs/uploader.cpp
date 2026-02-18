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

#include <mongocxx/v1/gridfs/uploader.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/client_session.hpp>
#include <mongocxx/v1/collection.hpp>
#include <mongocxx/v1/delete_many_result.hpp> // IWYU pragma: keep
#include <mongocxx/v1/insert_one_result.hpp>  // IWYU pragma: keep

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/gridfs/upload_result.hh>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

using code = v1::gridfs::uploader::errc;

class uploader::impl {
   public:
    v1::collection _files;                             // Collection to write the files document to.
    v1::collection _chunks;                            // Collection to write chunks to.
    v1::client_session const* _session_ptr;            // Optional session to use with collection commands.
    std::vector<bsoncxx::v1::document::value> _buffer; // Buffer of chunks to insert in bulk.
    bsoncxx::v1::types::value _id;                     // The ID of the file being written.
    std::string _filename;                             // The name of the file being written.
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _metadata; // Optional user-provided metadata.

    //  NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): fixed-size dynamic array: size tracked by `_chunk_size`.
    std::unique_ptr<std::uint8_t[]> _chunk_data;

    std::int32_t _chunk_size;        // Size (in bytes) of each chunk.
    std::int32_t _bytes_written = 0; // Number of bytes written to `_chunk_data`.
    std::int32_t _next_chunk_number = 0;
    bool _closed = false;

    impl(
        v1::collection files,
        v1::collection chunks,
        v1::client_session const* session_ptr,
        std::string filename,
        bsoncxx::v1::types::value id,
        std::int32_t chunk_size,
        bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> metadata)
        : _files{std::move(files)},
          _chunks{std::move(chunks)},
          _session_ptr{session_ptr},
          _id{std::move(id)},
          _filename{std::move(filename)},
          _metadata{std::move(metadata)},
          //  NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): fixed-size dynamic array: size tracked by `_chunk_size`.
          _chunk_data{bsoncxx::make_unique<std::uint8_t[]>(static_cast<std::size_t>(chunk_size))},
          _chunk_size{chunk_size} {}

    static impl const& with(uploader const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(uploader const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(uploader& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(uploader* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

uploader::~uploader() {
    delete impl::with(_impl);
}

uploader::uploader(uploader&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

uploader& uploader::operator=(uploader&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

uploader::uploader() : _impl{nullptr} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

uploader::operator bool() const {
    return _impl != nullptr;
}

bool uploader::is_open() const {
    return !impl::with(this)->_closed;
}

namespace {

void append_bson_value(char const* name, bsoncxx::v1::types::value const& value, scoped_bson& doc) {
    scoped_bson v;
    if (!BSON_APPEND_VALUE(v.out_ptr(), name, &bsoncxx::v1::types::value::internal::get_bson_value(value))) {
        throw std::logic_error{"mongocxx::v1::gridfs::append_bson_value: BSON_APPEND_VALUE failed"};
    }
    doc += v;
}

} // namespace

v1::gridfs::upload_result uploader::close() {
    auto& impl = *impl::with(this);

    // Nothing to do: already closed.
    if (impl._closed) {
        return v1::gridfs::upload_result::internal::make(impl._id);
    }

    impl._closed = true;

    // Save info before flushing the final chunk, which may contain fewer bytes than the chunk size.
    auto const bytes_written =
        static_cast<std::int64_t>(impl._next_chunk_number) * static_cast<std::int64_t>(impl._chunk_size);
    auto const remainder = static_cast<std::int64_t>(impl._bytes_written);
    auto const total_bytes_written = bytes_written + remainder;

    this->save_chunk();
    this->flush();

    scoped_bson files_doc;

    append_bson_value("_id", impl._id, files_doc);
    files_doc += scoped_bson{BCON_NEW("length", BCON_INT64(total_bytes_written))};
    files_doc += scoped_bson{BCON_NEW("chunkSize", BCON_INT32(impl._chunk_size))};
    files_doc += scoped_bson{BCON_NEW(
        "uploadDate",
        BCON_DATE_TIME(std::int64_t{bsoncxx::v1::types::b_date{std::chrono::system_clock::now()}.value.count()}))};
    files_doc += scoped_bson{BCON_NEW("filename", BCON_UTF8(impl._filename.c_str()))};

    if (auto const& opt = impl._metadata) {
        files_doc += scoped_bson{BCON_NEW("metadata", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    auto& files = impl._files;

    if (impl._session_ptr) {
        (void)files.insert_one(*impl._session_ptr, files_doc.view());
    } else {
        (void)files.insert_one(files_doc.view());
    }

    return v1::gridfs::upload_result::internal::make(impl._id);
}

void uploader::abort() {
    auto& impl = *impl::with(this);

    if (impl._closed) {
        throw v1::exception::internal::make(code::is_closed);
    }

    impl._closed = true;

    scoped_bson filter;
    append_bson_value("files_id", impl._id, filter);

    auto& chunks = impl._chunks;

    if (auto const& session = impl._session_ptr) {
        (void)chunks.delete_many(*session, filter.view());
    } else {
        (void)chunks.delete_many(filter.view());
    }
}

std::int32_t uploader::chunk_size() const {
    return impl::with(this)->_chunk_size;
}

void uploader::write(std::uint8_t const* data, std::size_t length) {
    auto& impl = *impl::with(this);

    if (impl._closed) {
        throw v1::exception::internal::make(code::is_closed);
    }

    // Nothing to write.
    if (length == 0u) {
        return;
    }

    auto const chunk_data = impl._chunk_data.get();
    auto const chunk_size = impl._chunk_size;

    // While there are still bytes to write...
    while (length > 0u) {
        auto const byte_offset = impl._bytes_written;
        auto const bytes_remaining = chunk_size - byte_offset;

        // When no more bytes are available in the current chunk, save the current chunk and prepare the next.
        if (bytes_remaining <= 0) {
            this->save_chunk();
        }

        // Write the next set of available bytes.
        auto const bytes_written = std::min(length, static_cast<std::size_t>(bytes_remaining));
        std::memcpy(chunk_data + byte_offset, data, bytes_written);

        // Shift all iterators and increment counters for the next iteration.
        data += bytes_written;
        impl._bytes_written += static_cast<std::int32_t>(bytes_written);
        length -= bytes_written;
    }
}

void uploader::flush() {
    auto& impl = *impl::with(this);

    if (impl._buffer.empty()) {
        return; // Nothing to do.
    }

    auto& chunks = impl._chunks;
    auto& buffer = impl._buffer;

    if (auto const session_ptr = impl._session_ptr) {
        chunks.insert_many(*session_ptr, buffer);
    } else {
        chunks.insert_many(buffer);
    }

    buffer.clear();
}

std::error_category const& uploader::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::gridfs::uploader";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::is_closed:
                    return "the GridFS file upload stream is not open";
                case code::too_many_chunks:
                    return "the total number of chunks must be less than INT32_MAX";
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
                    case code::too_many_chunks:
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
                    case code::too_many_chunks:
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

uploader::uploader(void* impl) : _impl{impl} {}

namespace {

std::size_t saved_chunks_limit(std::int32_t chunk_size) {
    // 16 * 1000 * 1000 (16 MB) for approximate consistency with the 16 MiB BSON document limit, but slightly less for
    // historical reasons (OP_MSG body size limit).
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    return 16u * 1000u * 1000u / static_cast<std::size_t>(chunk_size);
}

} // namespace

void uploader::save_chunk() {
    auto& impl = *impl::with(this);

    auto const bytes_written = impl._bytes_written;

    // No bytes were written to the current chunk: nothing to save.
    if (bytes_written == 0) {
        return;
    }

    auto const next_chunk_number = impl._next_chunk_number;

    // Already stored INT32_MAX chunks: next chunk number would overflow.
    if (next_chunk_number == std::numeric_limits<std::int32_t>::max()) {
        throw v1::exception::internal::make(code::too_many_chunks);
    }

    scoped_bson chunk_doc;

    append_bson_value("files_id", impl._id, chunk_doc);
    chunk_doc += scoped_bson{BCON_NEW("n", BCON_INT32(next_chunk_number))};
    chunk_doc += scoped_bson{BCON_NEW(
        "data", BCON_BIN(BSON_SUBTYPE_BINARY, impl._chunk_data.get(), static_cast<std::uint32_t>(bytes_written)))};

    auto& buffer = impl._buffer;

    // Save chunk into an internal chunk buffer...
    buffer.push_back(std::move(chunk_doc).value());
    ++impl._next_chunk_number;
    impl._bytes_written = 0;

    // ... and flush (insert) the chunks in bulk to reduce the number of commands.
    if (buffer.size() >= saved_chunks_limit(impl._chunk_size)) {
        this->flush();
    }
}

uploader uploader::internal::make(
    v1::collection files,
    v1::collection chunks,
    v1::client_session const* session_ptr,
    std::string filename,
    bsoncxx::v1::types::value id,
    std::int32_t chunk_size,
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata) {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> metadata_owner;

    if (metadata) {
        metadata_owner.emplace(*metadata);
    }

    return {new impl{
        std::move(files),
        std::move(chunks),
        session_ptr,
        std::move(filename),
        std::move(id),
        chunk_size,
        std::move(metadata_owner)}};
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
