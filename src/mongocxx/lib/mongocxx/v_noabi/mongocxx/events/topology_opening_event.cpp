// Copyright 2018-present MongoDB Inc.
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

#include <mongocxx/events/topology_opening_event.hpp>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace events {

topology_opening_event::topology_opening_event(const void* event) : _event(event) {}

topology_opening_event::~topology_opening_event() = default;

bsoncxx::oid topology_opening_event::topology_id() const {
    bson_oid_t boid;
    libmongoc::apm_topology_opening_get_topology_id(
        static_cast<const mongoc_apm_topology_opening_t*>(_event), &boid);
    return bsoncxx::oid(reinterpret_cast<const char*>(boid.bytes), sizeof(boid.bytes));
}

}  // namespace events
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
