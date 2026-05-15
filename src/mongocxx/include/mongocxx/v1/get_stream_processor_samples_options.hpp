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

#include <mongocxx/v1/get_stream_processor_samples_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {

///
/// Options for get_stream_processor_samples().
///
/// On the initial call (cursor_id absent or zero), set `limit` to cap total documents.
/// On subsequent calls (cursor_id non-zero), set `batch_size` to control batch size.
///
class get_stream_processor_samples_options {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~get_stream_processor_samples_options();
    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_samples_options(get_stream_processor_samples_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_samples_options&)
    operator=(get_stream_processor_samples_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_samples_options(get_stream_processor_samples_options const&);
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_samples_options&)
    operator=(get_stream_processor_samples_options const&);
    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_samples_options();

    ///
    /// The cursor ID from a previous call. If absent or zero, opens a new sample cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_samples_options&) cursor_id(std::int64_t v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) cursor_id() const;

    ///
    /// Maximum documents to sample. Only sent on the initial call (cursor_id absent or zero).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_samples_options&) limit(std::int32_t v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) limit() const;

    ///
    /// Documents per batch. Only sent on subsequent calls (cursor_id non-zero).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_samples_options&) batch_size(std::int32_t v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) batch_size() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::get_stream_processor_samples_options.
///
