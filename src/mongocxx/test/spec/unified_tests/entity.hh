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

#include <typeinfo>
#include <unordered_map>

#include <bsoncxx/types/bson_value/value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/client_encryption.hpp>

namespace mongocxx {
namespace entity {

class map {
   public:
    using key_type = std::string;

    map() = default;

    map(map const&) = delete;
    map operator=(map const&) = delete;

    map(map&&) = default;
    map& operator=(map&&) = default;

    ~map() = default;

    bool insert(key_type const& key, client&& c);
    bool insert(key_type const& key, mongocxx::database&&);
    bool insert(key_type const& key, mongocxx::collection&&);
    bool insert(key_type const& key, mongocxx::client_session&&);
    bool insert(key_type const& key, mongocxx::gridfs::bucket&&);
    bool insert(key_type const& key, mongocxx::change_stream&&);
    bool insert(key_type const& key, bsoncxx::types::bson_value::value&&);
    bool insert(key_type const& key, mongocxx::cursor&&);
    bool insert(key_type const& key, mongocxx::client_encryption&&);

    client& get_client(key_type const& key);
    database& get_database(key_type const& key);
    collection& get_collection(key_type const& key);
    change_stream& get_change_stream(key_type const& key);
    client_session& get_client_session(key_type const& key);
    gridfs::bucket& get_bucket(key_type const& key);
    bsoncxx::types::bson_value::value& get_value(key_type const& key);
    cursor& get_cursor(key_type const& key);
    mongocxx::client_encryption& get_client_encryption(key_type const& key);

    database& get_database_by_name(bsoncxx::stdx::string_view name);

    void clear() noexcept;
    std::type_info const& type(key_type const& key);

    void erase(key_type const& key);

   private:
    // Objects are destroyed in reverse order of their appearance in the class definition. Since the
    // client must outlive the objects created from it, the client objects are held in a separate
    // map and declared first.
    //
    // See:
    // - https://mongoc.org/libmongoc/current/lifecycle.html#object-lifecycle
    // - https://isocpp.org/wiki/faq/dtors#order-dtors-for-members
    std::unordered_map<key_type, client> _client_map;
    std::unordered_map<key_type, mongocxx::database> _database_map;
    std::unordered_map<key_type, mongocxx::collection> _collection_map;
    std::unordered_map<key_type, mongocxx::client_session> _session_map;
    std::unordered_map<key_type, mongocxx::gridfs::bucket> _bucket_map;
    std::unordered_map<key_type, mongocxx::change_stream> _stream_map;
    std::unordered_map<key_type, bsoncxx::types::bson_value::value> _value_map;
    std::unordered_map<key_type, mongocxx::cursor> _cursor_map;
    std::unordered_map<key_type, mongocxx::client_encryption> _client_encryption_map;
};

} // namespace entity
} // namespace mongocxx
