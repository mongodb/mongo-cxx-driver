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

#include <mongocxx/gridfs/bucket.hpp>

#include <ios>
#include <string>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/gridfs_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/gridfs/private/bucket.hh>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

bucket::bucket(const database& db, const options::gridfs::bucket& options) {
    std::string bucket_name = "fs";
    if (auto name = options.bucket_name()) {
        bucket_name = *name;
    }

    if (bucket_name.empty()) {
        throw logic_error{error_code::k_invalid_parameter, "non-empty bucket name required"};
    }

    std::int32_t default_chunk_size_bytes = 255 * 1024;
    if (auto chunk_size_bytes = options.chunk_size_bytes()) {
        default_chunk_size_bytes = *chunk_size_bytes;
    }

    if (default_chunk_size_bytes <= 0) {
        throw logic_error{error_code::k_invalid_parameter,
                          "positive value for chunk_size_bytes required"};
    }

    collection chunks = db[bucket_name + ".chunks"];
    collection files = db[bucket_name + ".files"];

    _impl = stdx::make_unique<impl>(
        std::move(bucket_name), default_chunk_size_bytes, std::move(chunks), std::move(files));

    if (auto read_concern = options.read_concern()) {
        _get_impl().files.read_concern(*read_concern);
        _get_impl().chunks.read_concern(*read_concern);
    }

    if (auto read_preference = options.read_preference()) {
        _get_impl().files.read_preference(*read_preference);
        _get_impl().chunks.read_preference(*read_preference);
    }

    if (auto write_concern = options.write_concern()) {
        _get_impl().files.write_concern(*write_concern);
        _get_impl().chunks.write_concern(*write_concern);
    }
}

bucket::bucket() noexcept = default;
bucket::bucket(bucket&&) noexcept = default;
bucket& bucket::operator=(bucket&&) noexcept = default;
bucket::~bucket() = default;

bucket::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

bucket::bucket(const bucket& b) {
    if (b) {
        _impl = stdx::make_unique<impl>(b._get_impl());
    }
}

bucket& bucket::operator=(const bucket& b) {
    if (!b) {
        _impl.reset();
    } else if (!*this) {
        _impl = stdx::make_unique<impl>(b._get_impl());
    } else {
        *_impl = b._get_impl();
    }
    return *this;
}

uploader bucket::open_upload_stream(stdx::string_view filename,
                                    const options::gridfs::upload& options) {
    auto id = bsoncxx::types::value{bsoncxx::types::b_oid{}};
    return open_upload_stream_with_id(id, filename, options);
}

uploader bucket::open_upload_stream(const client_session& session,
                                    stdx::string_view filename,
                                    const options::gridfs::upload& options) {
    auto id = bsoncxx::types::value{bsoncxx::types::b_oid{}};
    return open_upload_stream_with_id(session, id, filename, options);
}

uploader bucket::_open_upload_stream_with_id(const client_session* session,
                                             bsoncxx::types::value id,
                                             stdx::string_view filename,
                                             const options::gridfs::upload& options) {
    std::int32_t chunk_size_bytes = _get_impl().default_chunk_size_bytes;

    if (auto chunk_size = options.chunk_size_bytes()) {
        if (*chunk_size <= 0) {
            throw logic_error{
                error_code::k_invalid_parameter,
                "positive value required for options::gridfs::upload::chunk_size_bytes()"};
        }

        chunk_size_bytes = *chunk_size;
    }

    create_indexes_if_nonexistent(session);

    return uploader{session,
                    id,
                    filename,
                    _get_impl().files,
                    _get_impl().chunks,
                    chunk_size_bytes,
                    std::move(options.metadata())};
}

uploader bucket::open_upload_stream_with_id(bsoncxx::types::value id,
                                            stdx::string_view filename,
                                            const options::gridfs::upload& options) {
    return _open_upload_stream_with_id(nullptr, id, filename, options);
}

uploader bucket::open_upload_stream_with_id(const client_session& session,
                                            bsoncxx::types::value id,
                                            stdx::string_view filename,
                                            const options::gridfs::upload& options) {
    return _open_upload_stream_with_id(&session, id, filename, options);
}

result::gridfs::upload bucket::upload_from_stream(stdx::string_view filename,
                                                  std::istream* source,
                                                  const options::gridfs::upload& options) {
    auto id = bsoncxx::types::value{bsoncxx::types::b_oid{}};
    upload_from_stream_with_id(id, filename, source, options);
    return id;
}

result::gridfs::upload bucket::upload_from_stream(const client_session& session,
                                                  stdx::string_view filename,
                                                  std::istream* source,
                                                  const options::gridfs::upload& options) {
    auto id = bsoncxx::types::value{bsoncxx::types::b_oid{}};
    upload_from_stream_with_id(session, id, filename, source, options);
    return id;
}

void bucket::_upload_from_stream_with_id(const client_session* session,
                                         bsoncxx::types::value id,
                                         stdx::string_view filename,
                                         std::istream* source,
                                         const options::gridfs::upload& options) {
    uploader upload_stream = _open_upload_stream_with_id(session, id, filename, options);
    std::int32_t chunk_size = upload_stream.chunk_size();
    std::unique_ptr<std::uint8_t[]> buffer =
        stdx::make_unique<std::uint8_t[]>(static_cast<std::size_t>(chunk_size));

    do {
        source->read(reinterpret_cast<char*>(buffer.get()),
                     static_cast<std::streamsize>(chunk_size));
        upload_stream.write(buffer.get(), static_cast<std::size_t>(source->gcount()));
    } while (*source);

    // `(source->fail() && !source->eof())` is our check for EOF, which we don't treat as an error.
    if (source->bad() || (source->fail() && !source->eof())) {
        upload_stream.abort();
        source->exceptions(std::ios::failbit | std::ios::badbit);
        MONGOCXX_UNREACHABLE;
    }

    upload_stream.close();
}

void bucket::upload_from_stream_with_id(bsoncxx::types::value id,
                                        stdx::string_view filename,
                                        std::istream* source,
                                        const options::gridfs::upload& options) {
    return _upload_from_stream_with_id(nullptr, id, filename, source, options);
}

void bucket::upload_from_stream_with_id(const client_session& session,
                                        bsoncxx::types::value id,
                                        stdx::string_view filename,
                                        std::istream* source,
                                        const options::gridfs::upload& options) {
    return _upload_from_stream_with_id(&session, id, filename, source, options);
}

downloader bucket::_open_download_stream(const client_session* session, bsoncxx::types::value id) {
    using namespace bsoncxx;

    builder::basic::document files_filter;
    files_filter.append(builder::basic::kvp("_id", id));

    auto files_doc = session ? _get_impl().files.find_one(*session, files_filter.extract())
                             : _get_impl().files.find_one(files_filter.extract());

    if (!files_doc) {
        throw gridfs_exception{error_code::k_gridfs_file_not_found};
    }

    auto files_doc_view = files_doc->view();

    if (!files_doc_view["length"] || (files_doc_view["length"].type() != type::k_int64 &&
                                      files_doc_view["length"].type() != type::k_int32)) {
        throw gridfs_exception{error_code::k_gridfs_file_corrupted,
                               "expected files document to contain field \"length\" with type "
                               "k_int32 or k_int64"};
    }

    auto length = files_doc_view["length"];

    if ((length.type() == type::k_int64 && !length.get_int64().value) ||
        (length.type() == type::k_int32 && !length.get_int32().value)) {
        return downloader{stdx::nullopt, *files_doc};
    }

    builder::basic::document chunks_filter;
    chunks_filter.append(builder::basic::kvp("files_id", id));

    builder::basic::document chunks_sort;
    chunks_sort.append(builder::basic::kvp("n", 1));

    options::find chunks_options;
    chunks_options.sort(chunks_sort.extract());

    auto cursor = session
                      ? _get_impl().chunks.find(*session, chunks_filter.extract(), chunks_options)
                      : _get_impl().chunks.find(chunks_filter.extract(), chunks_options);

    return downloader{std::move(cursor), *files_doc};
}

downloader bucket::open_download_stream(bsoncxx::types::value id) {
    return _open_download_stream(nullptr, id);
}

downloader bucket::open_download_stream(const client_session& session, bsoncxx::types::value id) {
    return _open_download_stream(&session, id);
}

void bucket::_download_to_stream(const client_session* session,
                                 bsoncxx::types::value id,
                                 std::ostream* destination) {
    downloader download_stream = _open_download_stream(session, id);
    std::int32_t chunk_size = download_stream.chunk_size();
    std::unique_ptr<std::uint8_t[]> buffer =
        stdx::make_unique<std::uint8_t[]>(static_cast<std::size_t>(chunk_size));
    std::size_t bytes_read;

    while ((bytes_read =
                download_stream.read(buffer.get(), static_cast<std::size_t>(chunk_size))) != 0) {
        destination->write(reinterpret_cast<char*>(buffer.get()),
                           static_cast<std::streamsize>(bytes_read));
    }

    download_stream.close();
}

void bucket::download_to_stream(bsoncxx::types::value id, std::ostream* destination) {
    _download_to_stream(nullptr, id, destination);
}

void bucket::download_to_stream(const client_session& session,
                                bsoncxx::types::value id,
                                std::ostream* destination) {
    _download_to_stream(&session, id, destination);
}

void bucket::_delete_file(const client_session* session, bsoncxx::types::value id) {
    using namespace bsoncxx;

    builder::basic::document files_builder;
    files_builder.append(builder::basic::kvp("_id", id));

    auto result = session ? _get_impl().files.delete_one(*session, files_builder.extract())
                          : _get_impl().files.delete_one(files_builder.extract());
    if (result) {
        if (result->deleted_count() == 0) {
            throw gridfs_exception{error_code::k_gridfs_file_not_found};
        }
    }

    builder::basic::document chunks_builder;
    chunks_builder.append(builder::basic::kvp("files_id", id));
    document::value chunks_filter = chunks_builder.extract();

    if (session) {
        _get_impl().chunks.delete_many(*session, chunks_filter.view());
    } else {
        _get_impl().chunks.delete_many(chunks_filter.view());
    }
}

void bucket::delete_file(bsoncxx::types::value id) {
    _delete_file(nullptr, id);
}

void bucket::delete_file(const client_session& session, bsoncxx::types::value id) {
    _delete_file(&session, id);
}

cursor bucket::find(bsoncxx::document::view_or_value filter, const options::find& options) {
    return _get_impl().files.find(filter, options);
}

cursor bucket::find(const client_session& session,
                    bsoncxx::document::view_or_value filter,
                    const options::find& options) {
    return _get_impl().files.find(session, filter, options);
}

stdx::string_view bucket::bucket_name() const {
    return _get_impl().bucket_name;
}

void bucket::create_indexes_if_nonexistent(const client_session* session) {
    if (_get_impl().indexes_created) {
        return;
    }

    bsoncxx::builder::basic::document filter;
    filter.append(bsoncxx::builder::basic::kvp("_id", 1));

    auto find_options =
        options::find{}.projection(filter.view()).read_preference(read_preference{});

    if (session) {
        if (_get_impl().files.find_one(*session, {}, find_options)) {
            return;
        }
    } else if (_get_impl().files.find_one({}, find_options)) {
        return;
    }

    bsoncxx::builder::basic::document files_index;
    files_index.append(bsoncxx::builder::basic::kvp("filename", 1));
    files_index.append(bsoncxx::builder::basic::kvp("uploadDate", 1));

    if (session) {
        _get_impl().files.create_index(*session, files_index.extract());
    } else {
        _get_impl().files.create_index(files_index.extract());
    }

    bsoncxx::builder::basic::document chunks_index;
    chunks_index.append(bsoncxx::builder::basic::kvp("files_id", 1));
    chunks_index.append(bsoncxx::builder::basic::kvp("n", 1));

    options::index chunks_index_options;
    chunks_index_options.unique(true);

    if (session) {
        _get_impl().chunks.create_index(*session, chunks_index.extract(), chunks_index_options);
    } else {
        _get_impl().chunks.create_index(chunks_index.extract(), chunks_index_options);
    }

    _get_impl().indexes_created = true;
}

const bucket::impl& bucket::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_gridfs_bucket_object};
    }
    return *_impl;
}

bucket::impl& bucket::_get_impl() {
    auto cthis = const_cast<const bucket*>(this);
    return const_cast<bucket::impl&>(cthis->_get_impl());
}

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
