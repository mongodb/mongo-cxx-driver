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

#include <mongocxx/gridfs/uploader.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/exception-fwd.hpp>

#include <cstdint>
#include <string>

#include <bsoncxx/types/bson_value/value-fwd.hpp>

#include <mongocxx/client_session-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

class uploader::internal {
   public:
    static uploader make(
        v_noabi::collection files_coll,
        v_noabi::collection chunks_coll,
        v_noabi::client_session const* session_ptr,
        std::string filename,
        bsoncxx::v_noabi::types::value id,
        std::int32_t chunk_size,
        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& metadata);

    [[noreturn]]
    static void rethrow_exception(v1::exception const& ex);
};

} // namespace gridfs
} // namespace v_noabi
} // namespace mongocxx
