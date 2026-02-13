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

#include <mongocxx/gridfs/downloader.hh>

//

#include <mongocxx/v1/exception-fwd.hpp>

#include <mongocxx/v1/gridfs/downloader.hh>

#include <cstddef>
#include <cstdint>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/gridfs_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>

#include <mongocxx/mongoc_error.hh>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

namespace {

template <typename Downloader>
Downloader& check_moved_from(Downloader& downloader) {
    if (!downloader) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_gridfs_downloader_object};
    }
    return downloader;
}

} // namespace

std::size_t downloader::read(std::uint8_t* buffer, std::size_t length) try {
    return check_moved_from(_downloader).read(buffer, length);
} catch (v1::exception const& ex) {
    internal::rethrow_exception(ex);
}

void downloader::close() try { return check_moved_from(_downloader).close(); } catch (v1::exception const& ex) {
    internal::rethrow_exception(ex);
}

std::int32_t downloader::chunk_size() const {
    return check_moved_from(_downloader).chunk_size();
}

std::int64_t downloader::file_length() const {
    return check_moved_from(_downloader).file_length();
}

bsoncxx::v_noabi::document::view downloader::files_document() const {
    return check_moved_from(_downloader).files_document();
}

downloader downloader::internal::make() {
    return v1::gridfs::downloader::internal::make();
}

downloader downloader::internal::make(
    v_noabi::cursor cursor,
    bsoncxx::v_noabi::document::value files_document,
    std::int64_t file_length,
    std::int32_t chunk_size,
    std::int32_t initial_chunk_offset,
    std::int32_t initial_byte_offset) try {
    return v1::gridfs::downloader::internal::make(
        v_noabi::to_v1(std::move(cursor)),
        bsoncxx::v_noabi::to_v1(std::move(files_document)),
        file_length,
        chunk_size,
        initial_chunk_offset,
        initial_byte_offset);
} catch (v1::exception const& ex) {
    rethrow_exception(ex);
}

[[noreturn]]
void downloader::internal::rethrow_exception(v1::exception const& ex) {
    if (ex.code().category() == v1::gridfs::downloader::error_category()) {
        using code = v1::gridfs::downloader::errc;

        switch (static_cast<code>(ex.code().value())) {
            case code::is_closed:
                throw v_noabi::gridfs_exception{
                    v_noabi::error_code::k_gridfs_stream_not_open, strip_ec_msg(ex.what(), ex.code())};

            case code::corrupt_data:
                throw v_noabi::gridfs_exception{
                    v_noabi::error_code::k_gridfs_file_corrupted, strip_ec_msg(ex.what(), ex.code())};

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
