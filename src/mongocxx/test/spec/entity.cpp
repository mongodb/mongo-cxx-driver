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

void map::insert(const key_type& key, mongocxx::client&& client) {
    _map[key] = std::move(client);
}

bool map::contains(const key_type& key) const {
    return _map.find(key) != std::end(_map);
}

}  // namespace entity
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
