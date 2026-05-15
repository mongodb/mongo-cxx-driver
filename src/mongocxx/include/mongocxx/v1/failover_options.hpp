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

#include <mongocxx/v1/failover_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <string>

namespace mongocxx {
namespace v1 {

///
/// Options for failover behavior when starting a stream processor.
///
class failover_options {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~failover_options();
    MONGOCXX_ABI_EXPORT_CDECL() failover_options(failover_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(failover_options&) operator=(failover_options&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL() failover_options(failover_options const&);
    MONGOCXX_ABI_EXPORT_CDECL(failover_options&) operator=(failover_options const&);

    ///
    /// Construct with a required target region.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() explicit failover_options(std::string region);

    ///
    /// Set the target failover region. Required.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(failover_options&) region(std::string v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) region() const;

    ///
    /// Set the failover mode ("GRACEFUL" or "FORCED"). Defaults to "GRACEFUL".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(failover_options&) mode(std::string v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) mode() const;

    ///
    /// If true, validates the failover request without executing it.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(failover_options&) dry_run(bool v);
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) dry_run() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::failover_options.
///
