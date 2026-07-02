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

#include <mongocxx/v1/events/server_heartbeat_started.hh>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_apm_server_heartbeat_started_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_server_heartbeat_started_t const*>(ptr);
}

} // namespace

bsoncxx::v1::stdx::string_view server_heartbeat_started::host() const {
    return libmongoc::apm_server_heartbeat_started_get_host(to_mongoc(_impl))->host;
}

std::uint16_t server_heartbeat_started::port() const {
    return libmongoc::apm_server_heartbeat_started_get_host(to_mongoc(_impl))->port;
}

bool server_heartbeat_started::awaited() const {
    return libmongoc::apm_server_heartbeat_started_get_awaited(to_mongoc(_impl));
}

server_heartbeat_started::server_heartbeat_started(void const* impl) : _impl{impl} {}

server_heartbeat_started server_heartbeat_started::internal::make(mongoc_apm_server_heartbeat_started_t const* ptr) {
    return {ptr};
}

mongoc_apm_server_heartbeat_started_t const* server_heartbeat_started::internal::as_mongoc(
    server_heartbeat_started const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
