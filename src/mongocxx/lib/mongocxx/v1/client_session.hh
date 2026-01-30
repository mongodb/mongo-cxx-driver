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

#include <mongocxx/v1/client_session.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/client-fwd.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v1 {

class client_session::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL(client_session) make(mongoc_client_session_t* session, v1::client& client);

    static v1::client_session const& as_v1(client_session const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_client_session_t const*) as_mongoc(client_session const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_client_session_t*) as_mongoc(client_session& self);

    static MONGOCXX_ABI_EXPORT_CDECL(void) append_to(client_session const& self, scoped_bson& out);
};

class client_session::options::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(options) make(mongoc_session_opt_t* impl);

    static MONGOCXX_ABI_EXPORT_CDECL(mongoc_session_opt_t const*) as_mongoc(options const& self);
};

} // namespace v1
} // namespace mongocxx
