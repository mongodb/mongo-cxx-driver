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

#include <mongocxx/v1/gridfs/downloader.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <mongocxx/v1/cursor-fwd.hpp>

#include <cstdint>

#include <mongocxx/private/export.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

class downloader::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(downloader) make();

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(downloader) make(
        v1::cursor cursor,
        bsoncxx::v1::document::value files_doc,
        std::int64_t file_length,
        std::int32_t chunk_size,
        std::int32_t initial_chunk_number,
        std::int32_t initial_byte_offset);
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
