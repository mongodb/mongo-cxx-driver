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

void map::clear() noexcept {
    // Clients must outlive the entities created from it.
    // @see: https://isocpp.org/wiki/faq/dtors#order-dtors-for-members
    _map.clear();
    _client_map.clear();
}

}  // namespace entity
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
