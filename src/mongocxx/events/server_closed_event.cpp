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

#include <mongocxx/events/server_closed_event.hpp>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace events {

server_closed_event::server_closed_event(const void* event) : _event(event) {}

server_closed_event::~server_closed_event() = default;

bsoncxx::stdx::string_view server_closed_event::host() const {
    return libmongoc::apm_server_changed_get_host(
               static_cast<const mongoc_apm_server_changed_t*>(_event))
        ->host;
}

std::uint16_t server_closed_event::port() const {
    return libmongoc::apm_server_changed_get_host(
               static_cast<const mongoc_apm_server_changed_t*>(_event))
        ->port;
}

const bsoncxx::oid server_closed_event::topology_id() const {
    bson_oid_t boid;
    libmongoc::apm_server_changed_get_topology_id(
        static_cast<const mongoc_apm_server_changed_t*>(_event), &boid);

    return bsoncxx::oid{reinterpret_cast<const char*>(boid.bytes), sizeof(boid.bytes)};
}

}  // namespace events
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
