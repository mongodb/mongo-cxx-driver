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

#include "entity.hh"

#include <exception>

#include <bsoncxx/string/to_string.hpp>

namespace mongocxx {
namespace entity {

template <typename Key, typename Entity, typename Map>
bool _insert(Key const& k, Entity&& e, Map& m) {
    bool result{};
    std::tie(std::ignore, result) = m.emplace(k, std::forward<Entity>(e));
    return result;
}

bool map::insert(key_type const& key, client&& c) {
    return _insert(key, std::move(c), _client_map);
}

bool map::insert(key_type const& key, mongocxx::database&& db) {
    return _insert(key, std::move(db), _database_map);
}

bool map::insert(key_type const& key, mongocxx::collection&& coll) {
    return _insert(key, std::move(coll), _collection_map);
}

bool map::insert(key_type const& key, mongocxx::client_session&& session) {
    return _insert(key, std::move(session), _session_map);
}

bool map::insert(key_type const& key, mongocxx::gridfs::bucket&& bucket) {
    return _insert(key, std::move(bucket), _bucket_map);
}

bool map::insert(key_type const& key, mongocxx::change_stream&& stream) {
    return _insert(key, std::move(stream), _stream_map);
}

bool map::insert(key_type const& key, bsoncxx::types::bson_value::value&& value) {
    return _insert(key, std::move(value), _value_map);
}

bool map::insert(key_type const& key, mongocxx::cursor&& value) {
    return _insert(key, std::move(value), _cursor_map);
}

bool map::insert(key_type const& key, mongocxx::client_encryption&& value) {
    return _insert(key, std::move(value), _client_encryption_map);
}

client& map::get_client(key_type const& key) {
    return _client_map.at(key);
}

database& map::get_database(key_type const& key) {
    return _database_map.at(key);
}

collection& map::get_collection(key_type const& key) {
    return _collection_map.at(key);
}

client_session& map::get_client_session(key_type const& key) {
    return _session_map.at(key);
}

gridfs::bucket& map::get_bucket(key_type const& key) {
    return _bucket_map.at(key);
}

change_stream& map::get_change_stream(key_type const& key) {
    return _stream_map.at(key);
}

bsoncxx::types::bson_value::value& map::get_value(key_type const& key) {
    return _value_map.at(key);
}

mongocxx::cursor& map::get_cursor(key_type const& key) {
    return _cursor_map.at(key);
}

mongocxx::client_encryption& map::get_client_encryption(key_type const& key) {
    return _client_encryption_map.at(key);
}

std::type_info const& map::type(key_type const& key) {
    if (_client_encryption_map.find(key) != _client_encryption_map.end())
        return typeid(mongocxx::client_encryption);
    if (_database_map.find(key) != _database_map.end())
        return typeid(mongocxx::database);
    if (_collection_map.find(key) != _collection_map.end())
        return typeid(mongocxx::collection);
    if (_session_map.find(key) != _session_map.end())
        return typeid(mongocxx::client_session);
    if (_stream_map.find(key) != _stream_map.end())
        return typeid(mongocxx::change_stream);
    if (_bucket_map.find(key) != _bucket_map.end())
        return typeid(mongocxx::gridfs::bucket);
    if (_value_map.find(key) != _value_map.end())
        return typeid(bsoncxx::types::bson_value::value);
    if (_cursor_map.find(key) != _cursor_map.end())
        return typeid(mongocxx::cursor);
    if (_client_map.find(key) == _client_map.end())
        throw std::logic_error{"no key '" + key + "' in map"};
    return typeid(mongocxx::client);
}

database& map::get_database_by_name(bsoncxx::stdx::string_view name) {
    for (auto&& kvp : _database_map)
        if (name == kvp.second.name())
            return kvp.second;
    throw std::logic_error{"database name {" + bsoncxx::string::to_string(name) + "} not found."};
}

void map::clear() noexcept {
    // Clients must outlive the entities created from it.
    // @see: https://isocpp.org/wiki/faq/dtors#order-dtors-for-members
    _database_map.clear();
    _collection_map.clear();
    _session_map.clear();
    _bucket_map.clear();
    _stream_map.clear();
    _value_map.clear();
    _client_map.clear();
    _cursor_map.clear();
    _client_encryption_map.clear();
}

void map::erase(key_type const& key) {
    _database_map.erase(key) || _collection_map.erase(key) || _session_map.erase(key) || _bucket_map.erase(key) ||
        _stream_map.erase(key) || _value_map.erase(key) || _client_map.erase(key) || _cursor_map.erase(key) ||
        _client_encryption_map.erase(key);
}

} // namespace entity
} // namespace mongocxx
