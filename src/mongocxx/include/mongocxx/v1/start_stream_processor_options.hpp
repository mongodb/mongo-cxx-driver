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

#include <mongocxx/v1/start_stream_processor_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/failover_options-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/failover_options.hpp> // IWYU pragma: export

#include <cstdint>
#include <string>

namespace mongocxx {
namespace v1 {

///
/// Options for the startStreamProcessor command.
///
class start_stream_processor_options {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~start_stream_processor_options();
    MONGOCXX_ABI_EXPORT_CDECL() start_stream_processor_options(start_stream_processor_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) operator=(start_stream_processor_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL() start_stream_processor_options(start_stream_processor_options const&);
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) operator=(start_stream_processor_options const&);
    MONGOCXX_ABI_EXPORT_CDECL() start_stream_processor_options();

    ///
    /// Number of workers.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) workers(std::int32_t v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) workers() const;

    ///
    /// If true, clears checkpoints before starting.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) clear_checkpoints(bool v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) clear_checkpoints() const;

    ///
    /// Resume from a specific operation time.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) start_at_operation_time(
        bsoncxx::v1::types::b_timestamp v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp>) start_at_operation_time()
        const;

    ///
    /// Compute tier. Valid values: "SP2", "SP5", "SP10", "SP30", "SP50".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) tier(std::string v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) tier() const;

    ///
    /// Enable auto-scaling.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) enable_auto_scaling(bool v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) enable_auto_scaling() const;

    ///
    /// Failover options. When set, the failover field is included in the command.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(start_stream_processor_options&) failover(v1::failover_options v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::failover_options>) failover() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::start_stream_processor_options.
///
/// @par Includes
/// - @ref mongocxx/v1/failover_options.hpp
///
