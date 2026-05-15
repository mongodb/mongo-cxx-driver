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

#include <mongocxx/v1/get_stream_processor_stats_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

namespace mongocxx {
namespace v1 {

///
/// Options for the getStreamProcessorStats command.
///
class get_stream_processor_stats_options {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~get_stream_processor_stats_options();
    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_stats_options(get_stream_processor_stats_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_stats_options&)
    operator=(get_stream_processor_stats_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_stats_options(get_stream_processor_stats_options const&);
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_stats_options&)
    operator=(get_stream_processor_stats_options const&);
    MONGOCXX_ABI_EXPORT_CDECL() get_stream_processor_stats_options();

    ///
    /// If true, includes per-operator statistics in the response.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(get_stream_processor_stats_options&) verbose(bool v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) verbose() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::get_stream_processor_stats_options.
///
