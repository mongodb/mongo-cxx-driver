// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and limitations under the
// License.

#include <mongocxx/gridfs/bucket.hpp>

//

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/v1/gridfs/bucket.hh>
#include <mongocxx/v1/gridfs/upload_result.hh>
#include <mongocxx/v1/gridfs/uploader.hh>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <istream>
#include <ostream>
#include <string>
#include <utility>

#include <bsoncxx/types/bson_value/value-fwd.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/gridfs_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/gridfs/upload.hpp>
#include <mongocxx/result/gridfs/upload.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/gridfs/downloader.hh>
#include <mongocxx/gridfs/uploader.hh>
#include <mongocxx/mongoc_error.hh>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

using internal = v1::gridfs::bucket::internal;

namespace {

template <typename Bucket>
Bucket& check_moved_from(Bucket& bucket) {
    if (!bucket) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_gridfs_bucket_object};
    }
    return bucket;
}

[[noreturn]]
void rethrow_exception(v1::exception const& ex) {
    if (ex.code().category() == v1::gridfs::bucket::error_category()) {
        using code = v1::gridfs::bucket::errc;

        switch (static_cast<code>(ex.code().value())) {
            case code::invalid_bucket_name:
            case code::invalid_chunk_size_bytes:
                throw v_noabi::logic_error{
                    v_noabi::error_code::k_invalid_parameter, strip_ec_msg(ex.what(), ex.code())};

            case code::not_found:
                throw v_noabi::gridfs_exception{
                    v_noabi::error_code::k_gridfs_file_not_found, strip_ec_msg(ex.what(), ex.code())};

            case code::corrupt_data:
                throw v_noabi::gridfs_exception{
                    v_noabi::error_code::k_gridfs_file_corrupted, strip_ec_msg(ex.what(), ex.code())};

            case code::invalid_byte_range:
                throw v_noabi::logic_error{
                    v_noabi::error_code::k_invalid_parameter, strip_ec_msg(ex.what(), ex.code())};

            case code::zero:
            default:
                v_noabi::throw_exception<v_noabi::gridfs_exception>(ex);
        }
    } else if (ex.code().category() == v1::gridfs::uploader::error_category()) {
        v_noabi::gridfs::uploader::internal::rethrow_exception(ex);
    } else if (ex.code().category() == v1::gridfs::downloader::error_category()) {
        v_noabi::gridfs::downloader::internal::rethrow_exception(ex);
    } else {
        v_noabi::throw_exception<v_noabi::operation_exception>(ex);
    }
}

} // namespace

bucket::bucket(bucket const& other) {
    if (other) {
        _bucket = check_moved_from(other)._bucket;
    }
}

bucket& bucket::operator=(bucket const& other) {
    if (this != &other) {
        if (!other) {
            _bucket = v1::gridfs::bucket{};
        } else {
            _bucket = check_moved_from(other)._bucket;
        }
    }
    return *this;
}

v_noabi::gridfs::uploader bucket::open_upload_stream(
    bsoncxx::v_noabi::stdx::string_view filename,
    v_noabi::options::gridfs::upload const& options) {
    return this->open_upload_stream_with_id(bsoncxx::v_noabi::types::value{bsoncxx::v_noabi::oid{}}, filename, options);
}

v_noabi::gridfs::uploader bucket::open_upload_stream(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::stdx::string_view filename,
    v_noabi::options::gridfs::upload const& options) {
    return this->open_upload_stream_with_id(
        session, bsoncxx::v_noabi::types::value{bsoncxx::v_noabi::oid{}}, filename, options);
}

v_noabi::gridfs::uploader bucket::open_upload_stream_with_id(
    bsoncxx::v_noabi::types::view id,
    bsoncxx::v_noabi::stdx::string_view filename,
    v_noabi::options::gridfs::upload const& options) try {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata_v1;

    if (auto const& opt = options.metadata()) {
        metadata_v1.emplace(opt->view());
    }

    return internal::open_upload_stream_with_id_impl(
        check_moved_from(_bucket),
        nullptr,
        bsoncxx::v_noabi::to_v1(id),
        filename,
        options.chunk_size_bytes(),
        metadata_v1);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

v_noabi::gridfs::uploader bucket::open_upload_stream_with_id(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::types::view id,
    bsoncxx::v_noabi::stdx::string_view filename,
    v_noabi::options::gridfs::upload const& options) try {
    auto const& session_v1 = v_noabi::client_session::internal::as_v1(session);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata_v1;

    if (auto const& opt = options.metadata()) {
        metadata_v1.emplace(opt->view());
    }

    return internal::open_upload_stream_with_id_impl(
        check_moved_from(_bucket),
        &session_v1,
        bsoncxx::v_noabi::to_v1(id),
        filename,
        options.chunk_size_bytes(),
        metadata_v1);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

v_noabi::result::gridfs::upload bucket::upload_from_stream(
    bsoncxx::v_noabi::stdx::string_view filename,
    std::istream* source,
    v_noabi::options::gridfs::upload const& options) {
    bsoncxx::v_noabi::types::value id{bsoncxx::v_noabi::oid{}};

    this->upload_from_stream_with_id(id, filename, source, options);

    return v1::gridfs::upload_result::internal::make(bsoncxx::v_noabi::to_v1(std::move(id)));
}

v_noabi::result::gridfs::upload bucket::upload_from_stream(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::stdx::string_view filename,
    std::istream* source,
    v_noabi::options::gridfs::upload const& options) {
    bsoncxx::v_noabi::types::value id{bsoncxx::v_noabi::oid{}};

    this->upload_from_stream_with_id(session, id, filename, source, options);

    return v1::gridfs::upload_result::internal::make(bsoncxx::v_noabi::to_v1(std::move(id)));
}

void bucket::upload_from_stream_with_id(
    bsoncxx::v_noabi::types::view id,
    bsoncxx::v_noabi::stdx::string_view filename,
    std::istream* source,
    v_noabi::options::gridfs::upload const& options) try {
    internal::upload_from_stream_with_id_impl(
        v_noabi::to_v1(this->open_upload_stream_with_id(id, filename, options)), *source);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::upload_from_stream_with_id(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::types::view id,
    bsoncxx::v_noabi::stdx::string_view filename,
    std::istream* source,
    v_noabi::options::gridfs::upload const& options) try {
    internal::upload_from_stream_with_id_impl(
        v_noabi::to_v1(this->open_upload_stream_with_id(session, id, filename, options)), *source);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

v_noabi::gridfs::downloader bucket::open_download_stream(bsoncxx::v_noabi::types::view id) try {
    return internal::open_download_stream_impl(check_moved_from(_bucket), nullptr, bsoncxx::v_noabi::to_v1(id));
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

v_noabi::gridfs::downloader bucket::open_download_stream(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::types::view id) try {
    auto const& session_v1 = v_noabi::client_session::internal::as_v1(session);

    return internal::open_download_stream_impl(check_moved_from(_bucket), &session_v1, bsoncxx::v_noabi::to_v1(id));
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::download_to_stream(bsoncxx::v_noabi::types::view id, std::ostream* destination) try {
    internal::download_to_stream_impl(
        v_noabi::to_v1(this->open_download_stream(bsoncxx::v_noabi::to_v1(id))), *destination);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::download_to_stream(
    bsoncxx::v_noabi::types::view id,
    std::ostream* destination,
    std::size_t start,
    std::size_t end) try {
    using internal = v1::gridfs::bucket::internal;

    internal::download_to_stream_impl(
        internal::open_download_stream_impl(
            check_moved_from(_bucket), nullptr, bsoncxx::v_noabi::to_v1(id), start, end),
        *destination,
        static_cast<std::int64_t>(start),
        static_cast<std::int64_t>(end));
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::download_to_stream(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::types::view id,
    std::ostream* destination) try {
    internal::download_to_stream_impl(v_noabi::to_v1(this->open_download_stream(session, id)), *destination);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::download_to_stream(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::types::view id,
    std::ostream* destination,
    std::size_t start,
    std::size_t end) try {
    using internal = v1::gridfs::bucket::internal;

    auto const& session_v1 = v_noabi::client_session::internal::as_v1(session);

    internal::download_to_stream_impl(
        internal::open_download_stream_impl(
            check_moved_from(_bucket), &session_v1, bsoncxx::v_noabi::to_v1(id), start, end),
        *destination,
        static_cast<std::int64_t>(start),
        static_cast<std::int64_t>(end));
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::delete_file(bsoncxx::v_noabi::types::view id) try {
    internal::delete_file_impl(check_moved_from(_bucket), nullptr, bsoncxx::v_noabi::to_v1(id));
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

void bucket::delete_file(v_noabi::client_session const& session, bsoncxx::v_noabi::types::view id) try {
    auto const& session_v1 = v_noabi::client_session::internal::as_v1(session);

    internal::delete_file_impl(check_moved_from(_bucket), &session_v1, bsoncxx::v_noabi::to_v1(id));
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

v_noabi::cursor bucket::find(bsoncxx::v_noabi::document::view_or_value filter, v_noabi::options::find const& options) {
    return v_noabi::from_v1(internal::files(check_moved_from(_bucket))).find(filter, options);
}

v_noabi::cursor bucket::find(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find const& options) {
    return v_noabi::from_v1(internal::files(check_moved_from(_bucket))).find(session, filter, options);
}

bsoncxx::v_noabi::stdx::string_view bucket::bucket_name() const {
    return check_moved_from(_bucket).bucket_name();
}

} // namespace gridfs
} // namespace v_noabi
} // namespace mongocxx
