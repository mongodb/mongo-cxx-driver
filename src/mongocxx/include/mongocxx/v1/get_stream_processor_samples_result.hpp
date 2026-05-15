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

#include <mongocxx/v1/get_stream_processor_samples_result-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// Result of get_stream_processor_samples().
///
/// Callers MUST check cursor_id: a value of 0 means the cursor is exhausted and no further calls
/// should be made.
///
class get_stream_processor_samples_result {
   public:
    ///
    /// Construct with cursor ID and sampled documents.
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    get_stream_processor_samples_result(std::int64_t cursor_id, std::vector<bsoncxx::v1::document::value> documents);

    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_samples_result(get_stream_processor_samples_result&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_samples_result&)
    operator=(get_stream_processor_samples_result&&) noexcept;

    get_stream_processor_samples_result(get_stream_processor_samples_result const&) = delete;
    get_stream_processor_samples_result& operator=(get_stream_processor_samples_result const&) = delete;

    MONGOCXX_ABI_EXPORT_CDECL() ~get_stream_processor_samples_result();

    ///
    /// The cursor ID for the next call. Zero means the cursor is exhausted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) cursor_id() const noexcept;

    ///
    /// The sampled documents returned by this call.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<bsoncxx::v1::document::value> const&) documents() const noexcept;

   private:
    std::int64_t _cursor_id;
    std::vector<bsoncxx::v1::document::value> _documents;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::get_stream_processor_samples_result.
///
