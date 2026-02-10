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

#include <mongocxx/client_session.hpp> // IWYU pragma: export

//

#include <bsoncxx/builder/basic/document-fwd.hpp>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/options/client_session-fwd.hpp>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v_noabi {

class client_session::internal {
   public:
    static client_session
    make(mongoc_client_session_t* session, v_noabi::client& client, v_noabi::options::client_session opts);

    static v1::client_session const& as_v1(client_session const& self);
    static mongoc_client_session_t const* as_mongoc(client_session const& self);

    static void append_to(client_session const& self, scoped_bson& out);
    static void append_to(client_session const& self, bsoncxx::builder::basic::document& builder);
};

} // namespace v_noabi
} // namespace mongocxx
