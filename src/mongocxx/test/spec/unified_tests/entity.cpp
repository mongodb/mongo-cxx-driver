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

#include "entity.hh"

#include <exception>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace entity {

bool map::insert(const key_type& key, client&& c) {
    _client_map.emplace(key, std::move(c));
    return _client_map.find(key) != std::end(_client_map);
}

client& map::get_client(const key_type& key) {
    return _client_map.at(key);
}

database& map::get_database(const key_type& key) {
    auto& e = _map.at(key);
    return e.get<0>();
}

collection& map::get_collection(const key_type& key) {
    auto& e = _map.at(key);
    return e.get<1>();
}

client_session& map::get_client_session(const key_type& key) {
    auto& e = _map.at(key);
    return e.get<2>();
}

gridfs::bucket& map::get_bucket(const key_type& key) {
    auto& e = _map.at(key);
    return e.get<3>();
}

change_stream& map::get_change_stream(const key_type& key) {
    auto& e = _map.at(key);
    return e.get<4>();
}

bsoncxx::types::bson_value::value& map::get_value(const key_type& key) {
    auto& e = _map.at(key);
    return e.get<5>();
}

const std::type_info& map::type(const key_type& key) {
    if (_map.find(key) != _map.end())
        return _map.at(key).type();

    if (_client_map.find(key) == _client_map.end())
        throw std::logic_error{"no key '" + key + "' in map"};
    return typeid(mongocxx::client);
}

database& map::get_database_by_name(stdx::string_view name) {
    for (auto&& kvp : _map)
        if (typeid(database) == kvp.second.type() && name == kvp.second.get<0>().name())
            return kvp.second.get<0>();
    throw std::logic_error{"database name {" + name.to_string() + "} not found."};
}

void map::clear() noexcept {
    // Clients must outlive the entities created from it.
    // @see: https://isocpp.org/wiki/faq/dtors#order-dtors-for-members
    _map.clear();
    _client_map.clear();
}

void map::erase(const key_type& key) {
    if (_map.find(key) != _map.end()) {
        _map.erase(_map.find(key));
        return;
    }

    if (_client_map.find(key) == _client_map.end()) {
        throw std::logic_error{"key '" + key + "' not found."};
    }

    _client_map.erase(_client_map.find(key));
}

}  // namespace entity
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
