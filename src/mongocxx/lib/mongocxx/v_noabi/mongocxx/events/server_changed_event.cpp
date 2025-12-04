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

#include <mongocxx/events/server_changed_event.hpp>

//

#include <mongocxx/v1/events/server_description_changed.hh>

#include <mongocxx/events/server_description.hpp>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace events {

server_changed_event::server_changed_event(void const* event)
    : _event{v1::events::server_description_changed::internal::make(
          static_cast<mongoc_apm_server_changed_t const*>(event))} {}

server_description const server_changed_event::previous_description() const {
    return server_description{libmongoc::apm_server_changed_get_previous_description(
        v1::events::server_description_changed::internal::as_mongoc(_event))};
}

server_description const server_changed_event::new_description() const {
    return server_description{libmongoc::apm_server_changed_get_new_description(
        v1::events::server_description_changed::internal::as_mongoc(_event))};
}

} // namespace events
} // namespace v_noabi
} // namespace mongocxx
