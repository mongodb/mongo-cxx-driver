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

#pragma once

#include <mongocxx/v1/gridfs/bucket.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/collection-fwd.hpp>
#include <mongocxx/v1/gridfs/downloader-fwd.hpp>
#include <mongocxx/v1/gridfs/uploader-fwd.hpp>
#include <mongocxx/v1/read_concern-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/document/view.hpp> // C2139: "an undefined class is not allowed as an argument to compiler intrinsic type trait" (???)
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <string>

#include <mongocxx/private/export.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

class bucket::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bucket)
    make(v1::collection files, v1::collection chunks, std::string bucket_name, std::int32_t default_chunk_size);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(std::int32_t) default_chunk_size(bucket const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(v1::collection const&) files(bucket const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(v1::collection&) files(bucket& self);

    static std::int32_t compute_chunk_size(
        bucket const& self,
        bsoncxx::v1::stdx::optional<std::int32_t> chunk_size_bytes);

    static void create_indexes(bucket& self, v1::client_session const* session_ptr);

    static v1::gridfs::uploader open_upload_stream_with_id_impl(
        bucket& self,
        v1::client_session const* session_ptr,
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view filename,
        bsoncxx::v1::stdx::optional<std::int32_t> chunk_size_bytes,
        bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata);

    static void upload_from_stream_with_id_impl(v1::gridfs::uploader uploader, std::istream& input);

    static v1::gridfs::downloader open_download_stream_impl(
        bucket& self,
        v1::client_session const* session_ptr,
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::optional<std::size_t> start_opt = {},
        bsoncxx::v1::stdx::optional<std::size_t> end_opt = {});

    static void download_to_stream_impl(
        v1::gridfs::downloader downloader,
        std::ostream& output,
        bsoncxx::v1::stdx::optional<std::int64_t> start_opt = {},
        bsoncxx::v1::stdx::optional<std::int64_t> end_opt = {});

    static void delete_file_impl(bucket& self, v1::client_session const* session_ptr, bsoncxx::v1::types::view id);
};

class bucket::options::internal {
   public:
    static bsoncxx::v1::stdx::optional<std::string> const& bucket_name(options const& self);
    static bsoncxx::v1::stdx::optional<v1::read_concern> const& read_concern(options const& self);
    static bsoncxx::v1::stdx::optional<v1::read_preference> const& read_preference(options const& self);
    static bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern(options const& self);

    static bsoncxx::v1::stdx::optional<std::string>& bucket_name(options& self);
    static bsoncxx::v1::stdx::optional<v1::read_concern>& read_concern(options& self);
    static bsoncxx::v1::stdx::optional<v1::read_preference>& read_preference(options& self);
    static bsoncxx::v1::stdx::optional<v1::write_concern>& write_concern(options& self);
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
