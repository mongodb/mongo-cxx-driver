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

#include <mongocxx/v1/gridfs/uploader.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/collection-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <cstdint>
#include <string>

#include <mongocxx/private/export.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

class uploader::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(uploader) make(
        v1::collection files,
        v1::collection chunks,
        v1::client_session const* session_ptr,
        std::string filename,
        bsoncxx::v1::types::value id,
        std::int32_t chunk_size,
        bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> metadata);
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
