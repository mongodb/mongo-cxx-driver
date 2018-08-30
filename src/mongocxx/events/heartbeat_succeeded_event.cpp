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

#include <mongocxx/events/heartbeat_succeeded_event.hpp>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace events {

heartbeat_succeeded_event::heartbeat_succeeded_event(const void* event) : _succeeded_event(event) {}

heartbeat_succeeded_event::~heartbeat_succeeded_event() = default;

bsoncxx::document::view heartbeat_succeeded_event::reply() const {
    auto reply = libmongoc::apm_server_heartbeat_succeeded_get_reply(
        static_cast<const mongoc_apm_server_heartbeat_succeeded_t*>(_succeeded_event));
    return {bson_get_data(reply), reply->len};
}

std::int64_t heartbeat_succeeded_event::duration() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_duration(
        static_cast<const mongoc_apm_server_heartbeat_succeeded_t*>(_succeeded_event));
}

bsoncxx::stdx::string_view heartbeat_succeeded_event::host() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_host(
               static_cast<const mongoc_apm_server_heartbeat_succeeded_t*>(_succeeded_event))
        ->host;
}

std::uint16_t heartbeat_succeeded_event::port() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_host(
               static_cast<const mongoc_apm_server_heartbeat_succeeded_t*>(_succeeded_event))
        ->port;
}

}  // namespace events
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
