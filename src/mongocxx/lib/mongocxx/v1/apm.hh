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

#pragma once

#include <mongocxx/v1/apm.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/events/command_failed-fwd.hpp>
#include <mongocxx/v1/events/command_started-fwd.hpp>
#include <mongocxx/v1/events/command_succeeded-fwd.hpp>
#include <mongocxx/v1/events/server_closed-fwd.hpp>
#include <mongocxx/v1/events/server_description_changed-fwd.hpp>
#include <mongocxx/v1/events/server_heartbeat_failed-fwd.hpp>
#include <mongocxx/v1/events/server_heartbeat_started-fwd.hpp>
#include <mongocxx/v1/events/server_heartbeat_succeeded-fwd.hpp>
#include <mongocxx/v1/events/server_opening-fwd.hpp>
#include <mongocxx/v1/events/topology_closed-fwd.hpp>
#include <mongocxx/v1/events/topology_description_changed-fwd.hpp>
#include <mongocxx/v1/events/topology_opening-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <functional>

namespace mongocxx {
namespace v1 {

class apm::internal {
   public:
    template <typename T>
    using fn_type = std::function<void MONGOCXX_ABI_CDECL(T const&)>;

    static fn_type<v1::events::command_started> const& command_started(apm const& self);
    static fn_type<v1::events::command_failed> const& command_failed(apm const& self);
    static fn_type<v1::events::command_succeeded> const& command_succeeded(apm const& self);
    static fn_type<v1::events::server_closed> const& server_closed(apm const& self);
    static fn_type<v1::events::server_description_changed> const& server_description_changed(apm const& self);
    static fn_type<v1::events::server_opening> const& server_opening(apm const& self);
    static fn_type<v1::events::topology_closed> const& topology_closed(apm const& self);
    static fn_type<v1::events::topology_description_changed> const& topology_description_changed(apm const& self);
    static fn_type<v1::events::topology_opening> const& topology_opening(apm const& self);
    static fn_type<v1::events::server_heartbeat_started> const& server_heartbeat_started(apm const& self);
    static fn_type<v1::events::server_heartbeat_failed> const& server_heartbeat_failed(apm const& self);
    static fn_type<v1::events::server_heartbeat_succeeded> const& server_heartbeat_succeeded(apm const& self);

    static fn_type<v1::events::command_started>& command_started(apm& self);
    static fn_type<v1::events::command_failed>& command_failed(apm& self);
    static fn_type<v1::events::command_succeeded>& command_succeeded(apm& self);
    static fn_type<v1::events::server_closed>& server_closed(apm& self);
    static fn_type<v1::events::server_description_changed>& server_description_changed(apm& self);
    static fn_type<v1::events::server_opening>& server_opening(apm& self);
    static fn_type<v1::events::topology_closed>& topology_closed(apm& self);
    static fn_type<v1::events::topology_description_changed>& topology_description_changed(apm& self);
    static fn_type<v1::events::topology_opening>& topology_opening(apm& self);
    static fn_type<v1::events::server_heartbeat_started>& server_heartbeat_started(apm& self);
    static fn_type<v1::events::server_heartbeat_failed>& server_heartbeat_failed(apm& self);
    static fn_type<v1::events::server_heartbeat_succeeded>& server_heartbeat_succeeded(apm& self);
};

} // namespace v1
} // namespace mongocxx
