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

#include <mongocxx/v1/stream_processor-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/get_stream_processor_samples_options-fwd.hpp>
#include <mongocxx/v1/get_stream_processor_samples_result-fwd.hpp>
#include <mongocxx/v1/get_stream_processor_stats_options-fwd.hpp>
#include <mongocxx/v1/start_stream_processor_options-fwd.hpp>

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/get_stream_processor_samples_options.hpp> // IWYU pragma: export
#include <mongocxx/v1/get_stream_processor_samples_result.hpp>  // IWYU pragma: export
#include <mongocxx/v1/get_stream_processor_stats_options.hpp>   // IWYU pragma: export
#include <mongocxx/v1/start_stream_processor_options.hpp>       // IWYU pragma: export

#include <string>

namespace mongocxx {
namespace v1 {

///
/// A handle for a specific named stream processor.
///
/// Does not imply the processor currently exists on the server. All methods
/// send commands to the admin database of the associated workspace.
///
class stream_processor {
   private:
    stream_processor() = default;

    class impl;
    void* _impl{nullptr};

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~stream_processor();
    MONGOCXX_ABI_EXPORT_CDECL() stream_processor(stream_processor&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(stream_processor&) operator=(stream_processor&&) noexcept;

    stream_processor(stream_processor const&) = delete;
    stream_processor& operator=(stream_processor const&) = delete;

    class internal;

    ///
    /// Returns the processor name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) name() const;

    ///
    /// Starts the processor. Sends startStreamProcessor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) start(v1::start_stream_processor_options const& options = {});

    ///
    /// Stops the processor. Sends stopStreamProcessor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) stop();

    ///
    /// Permanently deletes the processor. Sends dropStreamProcessor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop();

    ///
    /// Returns runtime statistics. Sends getStreamProcessorStats.
    /// Returns an error if the processor is not in the STARTED state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value)
    stats(v1::get_stream_processor_stats_options const& options = {});

    ///
    /// Retrieves a batch of sampled documents from a running stream processor.
    ///
    /// On the initial call (cursor_id absent or zero in options), sends startSampleStreamProcessor
    /// followed immediately by getMoreSampleStreamProcessor to return the first batch.
    /// On subsequent calls (cursor_id non-zero), sends getMoreSampleStreamProcessor directly.
    ///
    /// Check the returned cursor_id: zero means the cursor is exhausted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::get_stream_processor_samples_result)
    get_stream_processor_samples(v1::get_stream_processor_samples_options const& options = {});
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::stream_processor.
///
/// @par Includes
/// - @ref mongocxx/v1/start_stream_processor_options.hpp
/// - @ref mongocxx/v1/get_stream_processor_stats_options.hpp
/// - @ref mongocxx/v1/get_stream_processor_samples_options.hpp
/// - @ref mongocxx/v1/get_stream_processor_samples_result.hpp
///
