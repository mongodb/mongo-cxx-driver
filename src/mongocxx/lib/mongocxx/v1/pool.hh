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

#include <mongocxx/v1/pool.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/apm-fwd.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class pool::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(pool) make(mongoc_client_pool_t* ptr);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_client_pool_t*) as_mongoc(pool& self);

    static void set_apm(pool& self, v1::apm v);
};

class pool::options::internal {
   public:
    static v1::client::options& client_opts(options& self);
};

class pool::entry::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL(entry) make(mongoc_client_pool_t* pool, mongoc_client_t* client);
};

} // namespace v1
} // namespace mongocxx
