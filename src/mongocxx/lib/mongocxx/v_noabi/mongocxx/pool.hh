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

#include <mongocxx/pool.hpp> // IWYU pragma: export

//

#include <mongocxx/client-fwd.hpp>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

class pool::internal {
   public:
    static mongoc_client_pool_t* as_mongoc(pool& self);
};

class pool::entry::internal {
   public:
    static entry make(v_noabi::client client, mongoc_client_pool_t* pool);
};

} // namespace v_noabi
} // namespace mongocxx
