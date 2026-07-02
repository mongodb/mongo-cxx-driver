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

#include <mongocxx/v1/events/server_opening.hh>

//

#include <bsoncxx/v1/oid.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_apm_server_opening_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_server_opening_t const*>(ptr);
}

} // namespace

bsoncxx::v1::stdx::string_view server_opening::host() const {
    return libmongoc::apm_server_opening_get_host(to_mongoc(_impl))->host;
}

std::uint16_t server_opening::port() const {
    return libmongoc::apm_server_opening_get_host(to_mongoc(_impl))->port;
}

bsoncxx::v1::oid server_opening::topology_id() const {
    bson_oid_t id = {};
    libmongoc::apm_server_opening_get_topology_id(to_mongoc(_impl), &id);
    return bsoncxx::v1::oid{id.bytes, sizeof(id)};
}

server_opening::server_opening(void const* impl) : _impl{impl} {}

server_opening server_opening::internal::make(mongoc_apm_server_opening_t const* ptr) {
    return {ptr};
}

mongoc_apm_server_opening_t const* server_opening::internal::as_mongoc(server_opening const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
