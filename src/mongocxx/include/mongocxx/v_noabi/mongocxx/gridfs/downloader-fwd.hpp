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

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace gridfs {

struct chunks_and_bytes_offset;

class MONGOCXX_API downloader;

}  // namespace gridfs
}  // namespace v_noabi
}  // namespace mongocxx

namespace mongocxx {
namespace gridfs {

using ::mongocxx::v_noabi::gridfs::chunks_and_bytes_offset;

using ::mongocxx::v_noabi::gridfs::downloader;

}  // namespace gridfs
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Declares utilities to download GridFS files.
///

#if defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace mongocxx {
namespace gridfs {

/// @ref mongocxx::v_noabi::gridfs::chunks_and_bytes_offset
struct chunks_and_bytes_offset {};

/// @ref mongocxx::v_noabi::gridfs::downloader
class downloader {};

}  // namespace gridfs
}  // namespace mongocxx

#endif  // defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
