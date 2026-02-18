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

#include <mongocxx/v1/gridfs/bucket.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/client_session.hpp>
#include <mongocxx/v1/collection.hpp>
#include <mongocxx/v1/cursor.hpp>
#include <mongocxx/v1/delete_many_result.hpp> // IWYU pragma: keep
#include <mongocxx/v1/delete_one_result.hpp>
#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/find_options.hpp>
#include <mongocxx/v1/gridfs/upload_options.hpp>
#include <mongocxx/v1/indexes.hpp>
#include <mongocxx/v1/read_concern.hpp>
#include <mongocxx/v1/read_preference.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/gridfs/downloader.hh>
#include <mongocxx/v1/gridfs/upload_result.hh>
#include <mongocxx/v1/gridfs/uploader.hh>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <system_error>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

using code = v1::gridfs::bucket::errc;

class bucket::impl {
   public:
    v1::collection _files;
    v1::collection _chunks;
    std::string _bucket_name;
    std::int32_t _default_chunk_size;
    bool _indexes_created = false;

    impl(v1::collection files, v1::collection chunks, std::string bucket_name, std::int32_t default_chunk_size)
        : _files{std::move(files)},
          _chunks{std::move(chunks)},
          _bucket_name{std::move(bucket_name)},
          _default_chunk_size{default_chunk_size} {}

    static impl const& with(bucket const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(bucket const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(bucket& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(bucket* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bucket::~bucket() {
    delete impl::with(_impl);
}

bucket::bucket(bucket&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bucket& bucket::operator=(bucket&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bucket::bucket(bucket const& other) : _impl{new impl{impl::with(other)}} {}

bucket& bucket::operator=(bucket const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bucket::bucket() : _impl{nullptr} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

bucket::operator bool() const {
    return _impl != nullptr;
}

v1::gridfs::uploader bucket::open_upload_stream(
    bsoncxx::v1::stdx::string_view filename,
    v1::gridfs::upload_options const& opts) {
    return this->open_upload_stream_with_id(bsoncxx::v1::types::value{bsoncxx::v1::oid{}}, filename, opts);
}

v1::gridfs::uploader bucket::open_upload_stream(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view filename,
    v1::gridfs::upload_options const& opts) {
    return this->open_upload_stream_with_id(session, bsoncxx::v1::types::value{bsoncxx::v1::oid{}}, filename, opts);
}

v1::gridfs::uploader bucket::open_upload_stream_with_id(
    bsoncxx::v1::types::view id,
    bsoncxx::v1::stdx::string_view filename,
    v1::gridfs::upload_options const& opts) {
    return internal::open_upload_stream_with_id_impl(
        *this, nullptr, id, filename, opts.chunk_size_bytes(), opts.metadata());
}

v1::gridfs::uploader bucket::open_upload_stream_with_id(
    v1::client_session const& session,
    bsoncxx::v1::types::view id,
    bsoncxx::v1::stdx::string_view filename,
    v1::gridfs::upload_options const& opts) {
    return internal::open_upload_stream_with_id_impl(
        *this, &session, id, filename, opts.chunk_size_bytes(), opts.metadata());
}

v1::gridfs::upload_result bucket::upload_from_stream(
    bsoncxx::v1::stdx::string_view filename,
    std::istream& input,
    v1::gridfs::upload_options const& opts) {
    bsoncxx::v1::types::value id{bsoncxx::v1::oid{}};

    this->upload_from_stream_with_id(id, filename, input, opts);

    return v1::gridfs::upload_result::internal::make(std::move(id));
}

v1::gridfs::upload_result bucket::upload_from_stream(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view filename,
    std::istream& input,
    v1::gridfs::upload_options const& opts) {
    bsoncxx::v1::types::value id{bsoncxx::v1::oid{}};

    this->upload_from_stream_with_id(session, id, filename, input, opts);

    return v1::gridfs::upload_result::internal::make(std::move(id));
}

void bucket::upload_from_stream_with_id(
    bsoncxx::v1::types::view id,
    bsoncxx::v1::stdx::string_view filename,
    std::istream& input,
    v1::gridfs::upload_options const& opts) {
    internal::upload_from_stream_with_id_impl(this->open_upload_stream_with_id(id, filename, opts), input);
}

void bucket::upload_from_stream_with_id(
    v1::client_session const& session,
    bsoncxx::v1::types::view id,
    bsoncxx::v1::stdx::string_view filename,
    std::istream& input,
    v1::gridfs::upload_options const& opts) {
    internal::upload_from_stream_with_id_impl(this->open_upload_stream_with_id(session, id, filename, opts), input);
}

namespace {

void append_bson_value(char const* name, bsoncxx::v1::types::view const& value, scoped_bson& doc) {
    scoped_bson v;
    if (!BSON_APPEND_VALUE(
            v.out_ptr(),
            name,
            &bsoncxx::v1::types::value::internal::get_bson_value(bsoncxx::v1::types::value{value}))) {
        throw std::logic_error{"mongocxx::v1::gridfs::append_bson_value: BSON_APPEND_VALUE failed"};
    }
    doc += v;
}

std::int64_t read_integral_field(char const* name, bsoncxx::v1::document::view doc) {
    auto const e = doc[name];

    if (!e || (e.type_id() != bsoncxx::v1::types::id::k_int64 && e.type_id() != bsoncxx::v1::types::id::k_int32)) {
        std::string msg;

        msg += "expected files document to contain field \"";
        msg += name;
        msg += "\" with type k_int32 or k_int64";

        if (e) {
            msg += " but got type ";
            msg += bsoncxx::v1::types::to_string(e.type_id());
        }

        throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
    }

    return e.type_id() == bsoncxx::v1::types::id::k_int64 ? e.get_int64().value : e.get_int32().value;
}

} // namespace

v1::gridfs::downloader bucket::open_download_stream(bsoncxx::v1::types::view id) {
    return internal::open_download_stream_impl(*this, nullptr, id);
}

v1::gridfs::downloader bucket::open_download_stream(v1::client_session const& session, bsoncxx::v1::types::view id) {
    return internal::open_download_stream_impl(*this, &session, id);
}

void bucket::download_to_stream(bsoncxx::v1::types::view id, std::ostream& output) {
    internal::download_to_stream_impl(this->open_download_stream(id), output);
}

void bucket::download_to_stream(v1::client_session const& session, bsoncxx::v1::types::view id, std::ostream& output) {
    internal::download_to_stream_impl(this->open_download_stream(session, id), output);
}

void bucket::download_to_stream(bsoncxx::v1::types::view id, std::ostream& output, std::size_t start, std::size_t end) {
    // Also validates `start` and `end` are representable as std::int64_t.
    auto downloader = internal::open_download_stream_impl(*this, nullptr, id, start, end);

    internal::download_to_stream_impl(
        std::move(downloader), output, static_cast<std::int64_t>(start), static_cast<std::int64_t>(end));
}

void bucket::download_to_stream(
    v1::client_session const& session,
    bsoncxx::v1::types::view id,
    std::ostream& output,
    std::size_t start,
    std::size_t end) {
    // Also validates `start` and `end` are representable as std::int64_t.
    auto downloader = internal::open_download_stream_impl(*this, &session, id, start, end);

    internal::download_to_stream_impl(
        std::move(downloader), output, static_cast<std::int64_t>(start), static_cast<std::int64_t>(end));
}

void bucket::delete_file(bsoncxx::v1::types::view id) {
    internal::delete_file_impl(*this, nullptr, id);
}

void bucket::delete_file(v1::client_session const& session, bsoncxx::v1::types::view id) {
    internal::delete_file_impl(*this, &session, id);
}

v1::cursor bucket::find(bsoncxx::v1::document::view filter, v1::find_options const& opts) {
    return impl::with(this)->_files.find(filter, opts);
}

v1::cursor
bucket::find(v1::client_session const& session, bsoncxx::v1::document::view filter, v1::find_options const& opts) {
    return impl::with(this)->_files.find(session, filter, opts);
}

bsoncxx::v1::stdx::string_view bucket::bucket_name() const {
    return impl::with(this)->_bucket_name;
}

std::error_category const& bucket::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::gridfs::bucket";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_bucket_name:
                    return "the \"bucketName\" field must not be empty";
                case code::invalid_chunk_size_bytes:
                    return "the \"chunkSizeBytes\" field must be a positive value";
                case code::not_found:
                    return "the requested GridFS file does not exist";
                case code::corrupt_data:
                    return "the GridFS file is in an invalid or inconsistent state";
                case code::invalid_byte_range:
                    return "[start, end) must be a valid range of byte indexes within the requested GridFS file";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_bucket_name:
                    case code::invalid_chunk_size_bytes:
                    case code::not_found:
                    case code::corrupt_data:
                    case code::invalid_byte_range:
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
                    case code::invalid_bucket_name:
                    case code::invalid_chunk_size_bytes:
                    case code::invalid_byte_range:
                        return type == condition::invalid_argument;

                    case code::not_found:
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

bucket::bucket(void* impl) : _impl{impl} {}

class bucket::options::impl {
   public:
    bsoncxx::v1::stdx::optional<std::string> _bucket_name;
    bsoncxx::v1::stdx::optional<std::int32_t> _chunk_size_bytes;
    bsoncxx::v1::stdx::optional<v1::read_concern> _read_concern;
    bsoncxx::v1::stdx::optional<v1::read_preference> _read_preference;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;

    static impl const& with(options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bucket::options::~options() {
    delete impl::with(_impl);
}

bucket::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bucket::options& bucket::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bucket::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

bucket::options& bucket::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bucket::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

bucket::options& bucket::options::bucket_name(std::string v) {
    impl::with(this)->_bucket_name = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> bucket::options::bucket_name() const {
    return impl::with(this)->_bucket_name;
}

bucket::options& bucket::options::chunk_size_bytes(std::int32_t v) {
    impl::with(this)->_chunk_size_bytes = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> bucket::options::chunk_size_bytes() const {
    return impl::with(this)->_chunk_size_bytes;
}

bucket::options& bucket::options::read_concern(v1::read_concern v) {
    impl::with(this)->_read_concern = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_concern> bucket::options::read_concern() const {
    return impl::with(this)->_read_concern;
}

bucket::options& bucket::options::read_preference(v1::read_preference v) {
    impl::with(this)->_read_preference = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> bucket::options::read_preference() const {
    return impl::with(this)->_read_preference;
}

bucket::options& bucket::options::write_concern(v1::write_concern v) {
    impl::with(this)->_write_concern = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> bucket::options::write_concern() const {
    return impl::with(this)->_write_concern;
}

bucket bucket::internal::make(
    v1::collection files,
    v1::collection chunks,
    std::string bucket_name,
    std::int32_t default_chunk_size) {
    return {new impl{std::move(files), std::move(chunks), std::move(bucket_name), default_chunk_size}};
}

std::int32_t bucket::internal::default_chunk_size(bucket const& self) {
    return impl::with(self)._default_chunk_size;
}

v1::collection const& bucket::internal::files(bucket const& self) {
    return impl::with(self)._files;
}

v1::collection& bucket::internal::files(bucket& self) {
    return impl::with(self)._files;
}

std::int32_t bucket::internal::compute_chunk_size(bucket const& self, bsoncxx::v1::stdx::optional<std::int32_t> opt) {
    if (!opt) {
        return impl::with(self)._default_chunk_size;
    }

    if (*opt <= 0) {
        throw v1::exception::internal::make(code::invalid_chunk_size_bytes);
    }

    return *opt;
}

void bucket::internal::create_indexes(bucket& self, v1::client_session const* session_ptr) {
    auto& impl = impl::with(self);

    // Nothing to do: already created.
    if (impl._indexes_created) {
        return;
    }

    auto& files = impl._files;
    auto& chunks = impl._chunks;

    {
        v1::find_options opts;

        opts.projection(scoped_bson{R"({"_id": 1})"}.value());
        opts.read_preference(v1::read_preference{});

        auto const res = session_ptr ? files.find_one(*session_ptr, {}, opts) : files.find_one({}, opts);

        // Do nothing if the files collection already contains documents.
        if (res.has_value()) {
            return;
        }
    }

    {
        scoped_bson keys{BCON_NEW("filename", BCON_INT32(1), "uploadDate", BCON_INT32(1))};

        session_ptr ? files.create_index(*session_ptr, std::move(keys).value())
                    : files.create_index(std::move(keys).value());
    }

    {
        scoped_bson keys{BCON_NEW("files_id", BCON_INT32(1), "n", BCON_INT32(1))};

        v1::indexes::options index_opts;

        index_opts.unique(true);

        session_ptr ? chunks.create_index(*session_ptr, std::move(keys).value(), std::move(index_opts))
                    : chunks.create_index(std::move(keys).value(), std::move(index_opts));
    }

    impl._indexes_created = true;
}

void bucket::internal::upload_from_stream_with_id_impl(v1::gridfs::uploader uploader, std::istream& input) {
    auto const chunk_size = uploader.chunk_size();

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): fixed-size dynamic array: size tracked by `chunk_size`.
    auto const buffer_owner = bsoncxx::make_unique<std::uint8_t[]>(static_cast<std::size_t>(chunk_size));
    auto const buffer = buffer_owner.get();

    do {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): stdlib vs. mongocxx compatibility.
        input.read(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(chunk_size));
        uploader.write(buffer, static_cast<std::size_t>(input.gcount()));
    } while (input);

    // Only throw an exception for non-EOF failures.
    if (input.bad() || (input.fail() && !input.eof())) {
        uploader.abort();
        input.exceptions(std::ios::failbit | std::ios::badbit);
        MONGOCXX_PRIVATE_UNREACHABLE;
    }

    uploader.close();
}

namespace {

bsoncxx::v1::document::value
find_files_doc(v1::collection& files, v1::client_session const* session_ptr, bsoncxx::v1::types::view id) {
    scoped_bson filter;
    append_bson_value("_id", id, filter);

    auto files_doc = session_ptr ? files.find_one(*session_ptr, filter.view()) : files.find_one(filter.view());

    if (!files_doc) {
        throw v1::exception::internal::make(code::not_found);
    }

    return std::move(*files_doc);
}

} // namespace

v1::gridfs::uploader bucket::internal::open_upload_stream_with_id_impl(
    bucket& self,
    v1::client_session const* session_ptr,
    bsoncxx::v1::types::view id,
    bsoncxx::v1::stdx::string_view filename,
    bsoncxx::v1::stdx::optional<std::int32_t> chunk_size_bytes,
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata) {
    auto& impl = impl::with(self);

    auto const chunk_size = internal::compute_chunk_size(self, chunk_size_bytes);

    internal::create_indexes(self, session_ptr);

    return v1::gridfs::uploader::internal::make(
        impl._files,
        impl._chunks,
        session_ptr,
        std::string{filename},
        bsoncxx::v1::types::value{id},
        chunk_size,
        metadata);
}

v1::gridfs::downloader bucket::internal::open_download_stream_impl(
    bucket& self,
    v1::client_session const* session_ptr,
    bsoncxx::v1::types::view id,
    bsoncxx::v1::stdx::optional<std::size_t> start_opt,
    bsoncxx::v1::stdx::optional<std::size_t> end_opt) {
    auto& impl = impl::with(self);

    auto& files = impl._files;
    auto& chunks = impl._chunks;

    auto const files_doc = find_files_doc(files, session_ptr, id);
    auto const file_length = read_integral_field("length", files_doc);

    auto const chunk_size = [&]() -> std::int32_t {
        static constexpr std::int64_t max_chunk_size = {16 * 1024 * 1024};

        static_assert(
            max_chunk_size <= std::numeric_limits<std::int32_t>::max(),
            "chunkSize must be representable as an std::int32_t");

        auto const raw_chunk_size = read_integral_field("chunkSize", files_doc);

        // Each chunk needs to be able to fit in a single document.
        if (raw_chunk_size > max_chunk_size) {
            std::string msg;

            msg += "files document contains unexpected chunk size of ";
            msg += std::to_string(raw_chunk_size);
            msg += ", which exceeds maximum chunk size of ";
            msg += std::to_string(max_chunk_size);

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        } else if (raw_chunk_size <= 0) {
            std::string msg;

            msg += "files document contains unexpected chunk size: ";
            msg += std::to_string(raw_chunk_size);
            msg += "; value must be positive";

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        }

        return static_cast<std::int32_t>(raw_chunk_size);
    }();

    auto const file_len = [&]() -> std::int64_t {
        auto const raw_file_len = read_integral_field("length", files_doc);

        if (raw_file_len < 0) {
            std::string msg;

            msg += "files document contains unexpected negative value for \"length\": ";
            msg += std::to_string(raw_file_len);

            throw v1::exception::internal::make(code::corrupt_data, msg.c_str());
        }

        return raw_file_len;
    }();

    if (file_length == 0) {
        return v1::gridfs::downloader::internal::make();
    }

    if (start_opt.value_or(0u) > end_opt.value_or(0u)) {
        throw v1::exception::internal::make(code::invalid_byte_range, "expected end to be greater than start");
    }

    std::int32_t initial_chunk_number = 0;
    std::int32_t initial_byte_offset = 0;

    if (start_opt.value_or(0u) > 0u) {
        auto const start_zu = *start_opt;

        if (start_zu > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max())) {
            throw v1::exception::internal::make(
                code::invalid_byte_range, "expected start to not be greater than max int64");
        }
        auto const start_i64 = static_cast<std::int64_t>(start_zu);

        if (start_i64 > file_len) {
            throw v1::exception::internal::make(
                code::invalid_byte_range, "expected start to not be greater than the file length");
        }

        auto const div = std::lldiv(start_i64, chunk_size);

        if (div.quot > std::numeric_limits<std::int32_t>::max()) {
            throw v1::exception::internal::make(
                code::invalid_byte_range, "expected chunk offset to be in bounds of int32");
        }
        initial_chunk_number = static_cast<std::int32_t>(div.quot);

        if (div.rem > std::numeric_limits<std::int32_t>::max()) {
            throw v1::exception::internal::make(
                code::invalid_byte_range, "expected bytes offset to be in bounds of int32");
        }
        initial_byte_offset = static_cast<std::int32_t>(div.rem);
    }

    bsoncxx::v1::stdx::optional<std::int64_t> limit_opt;

    if (end_opt) {
        auto const end_zu = *end_opt;

        if (end_zu > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max())) {
            throw v1::exception::internal::make(
                code::invalid_byte_range, "expected end to not be greater than max int64");
        }
        auto const end_i64 = static_cast<std::int64_t>(end_zu);

        if (end_i64 > file_len) {
            throw v1::exception::internal::make(
                code::invalid_byte_range, "expected end to not be greater than the file length");
        }

        if (end_i64 < file_len) {
            // Limit the number of chunks requested according to the position of the last byte to be read.
            limit_opt.emplace(
                (end_i64 / chunk_size) - (static_cast<std::int64_t>(start_opt.value_or(0u)) / chunk_size) + 1);
        }
    }

    scoped_bson chunks_filter;
    append_bson_value("files_id", id, chunks_filter);

    v1::find_options chunks_opts;
    chunks_opts.sort(scoped_bson{BCON_NEW("n", BCON_INT32(1))}.value());
    chunks_opts.skip(initial_chunk_number);
    if (limit_opt) {
        chunks_opts.limit(*limit_opt);
    }

    auto cursor = session_ptr ? chunks.find(*session_ptr, chunks_filter.view(), chunks_opts)
                              : chunks.find(chunks_filter.view(), chunks_opts);

    return v1::gridfs::downloader::internal::make(
        std::move(cursor), std::move(files_doc), file_len, chunk_size, initial_chunk_number, initial_byte_offset);
}

void bucket::internal::download_to_stream_impl(
    v1::gridfs::downloader downloader,
    std::ostream& output,
    bsoncxx::v1::stdx::optional<std::int64_t> start_opt,
    bsoncxx::v1::stdx::optional<std::int64_t> end_opt) {
    // Validated by `open_download_stream_impl()`.
    auto const chunk_size = static_cast<std::size_t>(downloader.chunk_size());
    auto const start = start_opt.value_or(0);
    auto const end = end_opt.value_or(downloader.file_length());

    auto bytes_remaining = static_cast<std::size_t>(end - start);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): fixed-size dynamic array: size tracked by `chunk_size`.
    std::unique_ptr<std::uint8_t[]> buffer{new std::uint8_t[chunk_size]{}};

    while (bytes_remaining > 0) {
        auto const count = downloader.read(buffer.get(), std::min<std::size_t>(bytes_remaining, chunk_size));

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): stdlib vs. mongocxx compatibility.
        output.write(reinterpret_cast<char*>(buffer.get()), static_cast<std::streamsize>(count));
        bytes_remaining -= count;
    }

    downloader.close();
}

void bucket::internal::delete_file_impl(
    bucket& self,
    v1::client_session const* session_ptr,
    bsoncxx::v1::types::view id) {
    auto& impl = impl::with(self);

    auto& files = impl._files;
    auto& chunks = impl._chunks;

    {
        scoped_bson filter;
        append_bson_value("_id", id, filter);

        auto const ret = session_ptr ? files.delete_one(*session_ptr, filter.view()) : files.delete_one(filter.view());

        if (ret && ret->deleted_count() == 0) {
            throw v1::exception::internal::make(code::not_found);
        }
    }

    {
        scoped_bson filter;
        append_bson_value("files_id", id, filter);
        session_ptr ? chunks.delete_many(*session_ptr, filter.view()) : chunks.delete_many(filter.view());
    }
}

bsoncxx::v1::stdx::optional<std::string> const& bucket::options::internal::bucket_name(options const& self) {
    return impl::with(self)._bucket_name;
}

bsoncxx::v1::stdx::optional<v1::read_concern> const& bucket::options::internal::read_concern(options const& self) {
    return impl::with(self)._read_concern;
}

bsoncxx::v1::stdx::optional<v1::read_preference> const& bucket::options::internal::read_preference(
    options const& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& bucket::options::internal::write_concern(options const& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<std::string>& bucket::options::internal::bucket_name(options& self) {
    return impl::with(self)._bucket_name;
}

bsoncxx::v1::stdx::optional<v1::read_concern>& bucket::options::internal::read_concern(options& self) {
    return impl::with(self)._read_concern;
}

bsoncxx::v1::stdx::optional<v1::read_preference>& bucket::options::internal::read_preference(options& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& bucket::options::internal::write_concern(options& self) {
    return impl::with(self)._write_concern;
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
