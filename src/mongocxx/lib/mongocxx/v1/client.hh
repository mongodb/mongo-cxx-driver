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

#include <mongocxx/v1/client.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/apm-fwd.hpp>
#include <mongocxx/v1/auto_encryption_options-fwd.hpp>
#include <mongocxx/v1/server_api-fwd.hpp>
#include <mongocxx/v1/tls-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class client::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(client) make(mongoc_client_t* client);

    static void set_apm(client& self, v1::apm v);

    static void disown(client& self);

    static mongoc_client_t const* as_mongoc(client const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_client_t*) as_mongoc(client& self);
};

class client::options::internal {
   public:
    static bsoncxx::v1::stdx::optional<v1::tls>& tls_opts(options& self);
    static bsoncxx::v1::stdx::optional<v1::auto_encryption_options>& auto_encryption_opts(options& self);
    static bsoncxx::v1::stdx::optional<v1::apm>& apm_opts(options& self);
    static bsoncxx::v1::stdx::optional<v1::server_api>& server_api_opts(options& self);
};

} // namespace v1
} // namespace mongocxx
