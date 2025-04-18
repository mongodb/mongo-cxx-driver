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

#include <mongocxx/events/heartbeat_succeeded_event.hpp>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace events {

heartbeat_succeeded_event::heartbeat_succeeded_event(void const* event) : _succeeded_event(event) {}

heartbeat_succeeded_event::~heartbeat_succeeded_event() = default;

bsoncxx::v_noabi::document::view heartbeat_succeeded_event::reply() const {
    auto casted = static_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(_succeeded_event);
    bson_t const* const reply = libmongoc::apm_server_heartbeat_succeeded_get_reply(casted);
    return {bson_get_data(reply), reply->len};
}

std::int64_t heartbeat_succeeded_event::duration() const {
    auto casted = static_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(_succeeded_event);
    return libmongoc::apm_server_heartbeat_succeeded_get_duration(casted);
}

bsoncxx::v_noabi::stdx::string_view heartbeat_succeeded_event::host() const {
    auto casted = static_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(_succeeded_event);
    return libmongoc::apm_server_heartbeat_succeeded_get_host(casted)->host;
}

std::uint16_t heartbeat_succeeded_event::port() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_host(
               static_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(_succeeded_event))
        ->port;
}

bool heartbeat_succeeded_event::awaited() const {
    return libmongoc::apm_server_heartbeat_succeeded_get_awaited(
        static_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(_succeeded_event));
}

} // namespace events
} // namespace v_noabi
} // namespace mongocxx
