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

#include <mongocxx/v1/database.hpp> // IWYU pragma: export

//

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class database::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(database) make(mongoc_database_t* db, mongoc_client_t* client);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_database_t const*) as_mongoc(database const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_database_t*) as_mongoc(database& self);

    static mongoc_client_t* get_client(database& self);
};

} // namespace v1
} // namespace mongocxx
