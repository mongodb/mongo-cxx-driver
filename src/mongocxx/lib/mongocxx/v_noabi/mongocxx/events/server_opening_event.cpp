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

#include <mongocxx/events/server_opening_event.hpp>

//

#include <mongocxx/v1/events/server_opening.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace events {

server_opening_event::server_opening_event(void const* event)
    : _event{v1::events::server_opening::internal::make(static_cast<mongoc_apm_server_opening_t const*>(event))} {}

} // namespace events
} // namespace v_noabi
} // namespace mongocxx
