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

#include <mongocxx/options/pool-fwd.hpp>

#include <mongocxx/options/client.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::pool.
///
class pool {
   public:
    ///
    /// Constructs a new pool options object. Note that options::pool is implictly convertible from
    /// options::client.
    ///
    /// @param client_opts
    ///   The client options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() pool(client client_opts = client());

    ///
    /// The current client options.
    ///
    /// @return The client options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client const&) client_opts() const;

   private:
    client _client_opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::pool.
///
