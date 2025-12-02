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

#include <mongocxx/v1/events/server_heartbeat_succeeded.hh>

//

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_apm_server_heartbeat_succeeded_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(ptr);
}

} // namespace

bsoncxx::v1::document::view server_heartbeat_succeeded::reply() const {
    return scoped_bson_view{libmongoc::apm_server_heartbeat_succeeded_get_reply(to_mongoc(_impl))}.view();
}

std::int64_t server_heartbeat_succeeded::duration() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_duration(to_mongoc(_impl));
}

bsoncxx::v1::stdx::string_view server_heartbeat_succeeded::host() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_host(to_mongoc(_impl))->host;
}

std::uint16_t server_heartbeat_succeeded::port() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_host(to_mongoc(_impl))->port;
}

bool server_heartbeat_succeeded::awaited() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_awaited(to_mongoc(_impl));
}

server_heartbeat_succeeded::server_heartbeat_succeeded(void const* impl) : _impl{impl} {}

server_heartbeat_succeeded server_heartbeat_succeeded::internal::make(
    mongoc_apm_server_heartbeat_succeeded_t const* ptr) {
    return {ptr};
}

mongoc_apm_server_heartbeat_succeeded_t const* server_heartbeat_succeeded::internal::as_mongoc(
    server_heartbeat_succeeded const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
