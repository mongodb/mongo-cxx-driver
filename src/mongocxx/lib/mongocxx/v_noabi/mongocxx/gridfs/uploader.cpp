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

#include <mongocxx/gridfs/uploader.hh>

//

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/v1/gridfs/uploader.hh>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include <bsoncxx/types/bson_value/value-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/gridfs_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/result/gridfs/upload.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/mongoc_error.hh>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

namespace {

template <typename Uploader>
Uploader& check_moved_from(Uploader& uploader) {
    if (!uploader) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_gridfs_uploader_object};
    }
    return uploader;
}

} // namespace

void uploader::write(std::uint8_t const* bytes, std::size_t length) try {
    check_moved_from(_uploader).write(bytes, length);
} catch (v1::exception const& ex) {
    internal::rethrow_exception(ex);
}

v_noabi::result::gridfs::upload uploader::close() try {
    if (!check_moved_from(_uploader).is_open()) {
        throw v_noabi::logic_error{v_noabi::error_code::k_gridfs_stream_not_open};
    }

    return _uploader.close();
} catch (v1::exception const& ex) {
    internal::rethrow_exception(ex);
}

void uploader::abort() try {
    if (!check_moved_from(_uploader).is_open()) {
        throw v_noabi::logic_error{v_noabi::error_code::k_gridfs_stream_not_open};
    }

    check_moved_from(_uploader).abort();
} catch (v1::exception const& ex) {
    internal::rethrow_exception(ex);
}

std::int32_t uploader::chunk_size() const {
    return check_moved_from(_uploader).chunk_size();
}

uploader uploader::internal::make(
    v_noabi::collection files_coll,
    v_noabi::collection chunks_coll,
    v_noabi::client_session const* session_ptr,
    std::string filename,
    bsoncxx::v_noabi::types::value id,
    std::int32_t chunk_size,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& metadata) {
    using bsoncxx::v_noabi::to_v1;
    using mongocxx::v_noabi::to_v1;

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata_v1;

    if (metadata) {
        metadata_v1.emplace(metadata->view());
    }

    return v1::gridfs::uploader::internal::make(
        to_v1(std::move(files_coll)),
        to_v1(std::move(chunks_coll)),
        session_ptr ? &v_noabi::client_session::internal::as_v1(*session_ptr) : nullptr,
        std::move(filename),
        to_v1(std::move(id)),
        chunk_size,
        metadata_v1);
}

[[noreturn]]
void uploader::internal::rethrow_exception(v1::exception const& ex) {
    if (ex.code().category() == v1::gridfs::uploader::error_category()) {
        using code = v1::gridfs::uploader::errc;

        switch (static_cast<code>(ex.code().value())) {
            case code::is_closed:
                throw v_noabi::gridfs_exception{
                    v_noabi::error_code::k_gridfs_stream_not_open, strip_ec_msg(ex.what(), ex.code())};

            case code::too_many_chunks:
                throw v_noabi::gridfs_exception{
                    v_noabi::error_code::k_gridfs_upload_requires_too_many_chunks, strip_ec_msg(ex.what(), ex.code())};

            case code::zero:
            default:
                v_noabi::throw_exception<v_noabi::gridfs_exception>(ex);
        }
    } else {
        v_noabi::throw_exception<v_noabi::operation_exception>(ex);
    }
}

} // namespace gridfs
} // namespace v_noabi
} // namespace mongocxx
