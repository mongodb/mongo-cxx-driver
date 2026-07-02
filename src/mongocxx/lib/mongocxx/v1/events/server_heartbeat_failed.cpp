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

#include <mongocxx/v1/events/server_heartbeat_failed.hh>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>
#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_apm_server_heartbeat_failed_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_server_heartbeat_failed_t const*>(ptr);
}

} // namespace

std::string server_heartbeat_failed::message() const {
    bson_error_t error = {};
    libmongoc::apm_server_heartbeat_failed_get_error(to_mongoc(_impl), &error);
    return error.message;
}

std::int64_t server_heartbeat_failed::duration() const {
    return libmongoc::apm_server_heartbeat_failed_get_duration(to_mongoc(_impl));
}

bsoncxx::v1::stdx::string_view server_heartbeat_failed::host() const {
    return libmongoc::apm_server_heartbeat_failed_get_host(to_mongoc(_impl))->host;
}

std::uint16_t server_heartbeat_failed::port() const {
    return libmongoc::apm_server_heartbeat_failed_get_host(to_mongoc(_impl))->port;
}

bool server_heartbeat_failed::awaited() const {
    return libmongoc::apm_server_heartbeat_failed_get_awaited(to_mongoc(_impl));
}

server_heartbeat_failed::server_heartbeat_failed(void const* impl) : _impl{impl} {}

server_heartbeat_failed server_heartbeat_failed::internal::make(mongoc_apm_server_heartbeat_failed_t const* ptr) {
    return {ptr};
}

mongoc_apm_server_heartbeat_failed_t const* server_heartbeat_failed::internal::as_mongoc(
    server_heartbeat_failed const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
