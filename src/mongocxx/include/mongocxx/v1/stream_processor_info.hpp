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

#include <mongocxx/v1/stream_processor_info-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/value-fwd.hpp>
#include <bsoncxx/v1/array/view-fwd.hpp>
#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// Information about a stream processor returned by getStreamProcessor.
///
class stream_processor_info {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~stream_processor_info();
    MONGOCXX_ABI_EXPORT_CDECL() stream_processor_info(stream_processor_info&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(stream_processor_info&) operator=(stream_processor_info&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL() stream_processor_info(stream_processor_info const&);
    MONGOCXX_ABI_EXPORT_CDECL(stream_processor_info&) operator=(stream_processor_info const&);

    ///
    /// Construct from a getStreamProcessor command response document.
    /// Unknown fields in the document are silently ignored.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() explicit stream_processor_info(bsoncxx::v1::document::view doc);

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) id() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) name() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) state() const;

    ///
    /// The processor pipeline as an array of documents.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::array::view) pipeline() const;

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) pipeline_version() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) tier() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) dlq() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) stream_meta_field_name()
        const;
    MONGOCXX_ABI_EXPORT_CDECL(bool) enable_auto_scaling() const;
    MONGOCXX_ABI_EXPORT_CDECL(bool) failover_enabled() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) active_region() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) workspace_default_region() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point>) last_state_change()
        const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point>) last_modified_at()
        const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) modified_by() const;
    MONGOCXX_ABI_EXPORT_CDECL(bool) has_started() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) error_msg() const;
    MONGOCXX_ABI_EXPORT_CDECL(bool) error_retryable() const;
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) error_code() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::stream_processor_info.
///
