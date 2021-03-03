// Copyright 2020 MongoDB Inc.
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

#include <mongocxx/config/private/prelude.hh>

#include <unordered_map>

#include <bsoncxx/stdx/variant.hpp>
#include <mongocxx/client.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

namespace entity {

class map {
   public:
    using key_type = mongocxx::stdx::string_view;
    using mapped_type = bsoncxx::stdx::variant<mongocxx::client,
                                               mongocxx::database,
                                               mongocxx::collection,
                                               mongocxx::client_session,
                                               mongocxx::gridfs::bucket>;

    map() = default;

    map(const map&) = delete;
    map operator=(const map&) = delete;

    map(map&&) = default;
    map& operator=(map&&) = default;

    void insert(const key_type& key, mongocxx::client&& client);

    bool contains(const key_type& key) const;

   private:
    std::unordered_map<key_type, mapped_type> _map;
};
}  // namespace entity
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
