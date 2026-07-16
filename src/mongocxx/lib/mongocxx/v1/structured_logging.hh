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

#include <mongocxx/v1/structured_logging.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/structured_log.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class structured_logging::internal {
   public:
    // The current handler, by reference (used by the C bridge to avoid copying).
    static v1::structured_log_handler const& handler(structured_logging const& self);

    // Build a fresh mongoc_structured_log_opts_t populated from `self` (caller destroys). The
    // handler is registered with `&self` as user_data, so `self` must outlive the client/pool.
    static mongoc_structured_log_opts_t* make_opts(structured_logging& self);

    // Apply this configuration to a client / pool. `self` must outlive the client/pool.
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(void) apply_to(mongoc_client_t* client, structured_logging& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(void) apply_to(mongoc_client_pool_t* pool, structured_logging& self);
};

} // namespace v1
} // namespace mongocxx
