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
#include <mongocxx/test/spec/monitoring.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

namespace entity {

class map {
   public:
    using key_type = std::string;
    using mapped_type = bsoncxx::stdx::variant<mongocxx::database,
                                               mongocxx::collection,
                                               mongocxx::client_session,
                                               mongocxx::gridfs::bucket>;

    map() noexcept = default;

    map(const map&) = delete;
    map operator=(const map&) = delete;

    map(map&&) noexcept = default;
    map& operator=(map&&) noexcept = default;

    ~map() = default;

    template <typename Entity>
    bool insert(const key_type& key, Entity&& e) {
        bool result{};
        std::tie(std::ignore, result) = _map.emplace(key, std::forward<Entity>(e));
        return result;
    }

    bool insert(const key_type& key, client&& c);

    client& get_client(const key_type& key);
    database& get_database(const key_type& key);
    collection& get_collection(const key_type& key);

    database& get_database_by_name(stdx::string_view name);

    void clear() noexcept;

    spec::apm_checker& get_apm_checker();

   private:
    spec::apm_checker _apm;
    // Objects are destroyed in reverse order of their appearance in the class definition. Since the
    // client must outlive the objects created from it, the client objects are held in a separate
    // map and declared first.
    //
    // @see: http://mongoc.org/libmongoc/current/lifecycle.html#object-lifecycle
    // @see: https://isocpp.org/wiki/faq/dtors#order-dtors-for-members
    std::unordered_map<key_type, client> _client_map;
    std::unordered_map<key_type, mapped_type> _map;
};
}  // namespace entity
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
