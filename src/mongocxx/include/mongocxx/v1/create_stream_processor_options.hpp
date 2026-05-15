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

#include <mongocxx/v1/create_stream_processor_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <string>

namespace mongocxx {
namespace v1 {

///
/// Options for the createStreamProcessor command.
///
class create_stream_processor_options {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~create_stream_processor_options();
    MONGOCXX_ABI_EXPORT_CDECL() create_stream_processor_options(create_stream_processor_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(create_stream_processor_options&) operator=(create_stream_processor_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL() create_stream_processor_options(create_stream_processor_options const&);
    MONGOCXX_ABI_EXPORT_CDECL(create_stream_processor_options&) operator=(create_stream_processor_options const&);
    MONGOCXX_ABI_EXPORT_CDECL() create_stream_processor_options();

    ///
    /// Dead letter queue configuration document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_stream_processor_options&) dlq(bsoncxx::v1::document::value v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) dlq() const;

    ///
    /// Field name for stream metadata.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_stream_processor_options&) stream_meta_field_name(std::string v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) stream_meta_field_name()
        const;

    ///
    /// Compute tier.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_stream_processor_options&) tier(std::string v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) tier() const;

    ///
    /// Enable failover.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_stream_processor_options&) failover(bool v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) failover() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::create_stream_processor_options.
///
