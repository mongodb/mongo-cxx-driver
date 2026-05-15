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

#include <mongocxx/v1/stream_processors-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/create_stream_processor_options-fwd.hpp>
#include <mongocxx/v1/stream_processor-fwd.hpp>
#include <mongocxx/v1/stream_processor_info-fwd.hpp>

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/create_stream_processor_options.hpp> // IWYU pragma: export
#include <mongocxx/v1/stream_processor.hpp>                // IWYU pragma: export
#include <mongocxx/v1/stream_processor_info.hpp>           // IWYU pragma: export

#include <string>

namespace mongocxx {
namespace v1 {

///
/// A handle for managing stream processors in a stream processing workspace.
///
/// Obtained via stream_processing_client::stream_processors().
/// The caller must ensure the associated stream_processing_client outlives this handle.
///
class stream_processors {
   private:
    stream_processors() = default;

    class impl;
    void* _impl{nullptr};

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~stream_processors();
    MONGOCXX_ABI_EXPORT_CDECL() stream_processors(stream_processors&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(stream_processors&) operator=(stream_processors&&) noexcept;

    stream_processors(stream_processors const&) = delete;
    stream_processors& operator=(stream_processors const&) = delete;

    ///
    /// Creates a new stream processor. Sends createStreamProcessor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    create(
        std::string name,
        bsoncxx::v1::array::view pipeline,
        v1::create_stream_processor_options const& options = {});

    ///
    /// Returns a handle for an existing stream processor by name.
    /// No network call is made.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::stream_processor) get(std::string name);

    ///
    /// Returns information about a stream processor. Sends getStreamProcessor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::stream_processor_info) get_info(std::string const& name);

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::stream_processors.
///
/// @par Includes
/// - @ref mongocxx/v1/stream_processor.hpp
/// - @ref mongocxx/v1/stream_processor_info.hpp
/// - @ref mongocxx/v1/create_stream_processor_options.hpp
///
