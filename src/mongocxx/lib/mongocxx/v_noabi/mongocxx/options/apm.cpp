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

#include <mongocxx/options/apm.hpp>

//

#include <mongocxx/v1/config/export.hpp>

#include <mongocxx/v1/apm.hh>

#include <functional>
#include <utility>

#include <mongocxx/events/command_failed_event.hpp>
#include <mongocxx/events/command_started_event.hpp>
#include <mongocxx/events/command_succeeded_event.hpp>
#include <mongocxx/events/heartbeat_failed_event.hpp>
#include <mongocxx/events/heartbeat_started_event.hpp>
#include <mongocxx/events/heartbeat_succeeded_event.hpp>
#include <mongocxx/events/server_changed_event.hpp>
#include <mongocxx/events/server_closed_event.hpp>
#include <mongocxx/events/server_opening_event.hpp>
#include <mongocxx/events/topology_changed_event.hpp>
#include <mongocxx/events/topology_closed_event.hpp>
#include <mongocxx/events/topology_opening_event.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

namespace {

template <typename Event>
struct event_to_v1;

// clang-format off
template <> struct event_to_v1<v_noabi::events::command_started_event> { using type = v1::events::command_started; };
template <> struct event_to_v1<v_noabi::events::command_failed_event> { using type = v1::events::command_failed; };
template <> struct event_to_v1<v_noabi::events::command_succeeded_event> { using type = v1::events::command_succeeded; };
template <> struct event_to_v1<v_noabi::events::server_closed_event> { using type = v1::events::server_closed; };
template <> struct event_to_v1<v_noabi::events::server_changed_event> { using type = v1::events::server_description_changed; };
template <> struct event_to_v1<v_noabi::events::server_opening_event> { using type = v1::events::server_opening; };
template <> struct event_to_v1<v_noabi::events::topology_closed_event> { using type = v1::events::topology_closed; };
template <> struct event_to_v1<v_noabi::events::topology_changed_event> { using type = v1::events::topology_description_changed; };
template <> struct event_to_v1<v_noabi::events::topology_opening_event> { using type = v1::events::topology_opening; };
template <> struct event_to_v1<v_noabi::events::heartbeat_started_event> { using type = v1::events::server_heartbeat_started; };
template <> struct event_to_v1<v_noabi::events::heartbeat_failed_event> { using type = v1::events::server_heartbeat_failed; };
template <> struct event_to_v1<v_noabi::events::heartbeat_succeeded_event> { using type = v1::events::server_heartbeat_succeeded; };
// clang-format on

// C++14 or newer is required for [x = std::move(expr)] captures.
template <typename Fn>
struct invoke_as_v1 {
    Fn _fn;

    template <typename v_noabi_type, typename v1_type = typename event_to_v1<v_noabi_type>::type>
    void MONGOCXX_ABI_CDECL operator()(v_noabi_type const& event) const {
        _fn(static_cast<v1_type>(event));
    }
};

template <typename Fn>
auto v_noabi_to_v1(Fn&& fn) -> invoke_as_v1<Fn> {
    return invoke_as_v1<Fn>{std::forward<Fn>(fn)};
}

} // namespace

apm::apm(v1::apm other)
    : _command_started{v_noabi_to_v1(std::move(v1::apm::internal::command_started(other)))},
      _command_failed{v_noabi_to_v1(std::move(v1::apm::internal::command_failed(other)))},
      _command_succeeded{v_noabi_to_v1(std::move(v1::apm::internal::command_succeeded(other)))},
      _server_closed{v_noabi_to_v1(std::move(v1::apm::internal::server_closed(other)))},
      _server_changed{v_noabi_to_v1(std::move(v1::apm::internal::server_description_changed(other)))},
      _server_opening{v_noabi_to_v1(std::move(v1::apm::internal::server_opening(other)))},
      _topology_closed{v_noabi_to_v1(std::move(v1::apm::internal::topology_closed(other)))},
      _topology_changed{v_noabi_to_v1(std::move(v1::apm::internal::topology_description_changed(other)))},
      _topology_opening{v_noabi_to_v1(std::move(v1::apm::internal::topology_opening(other)))},
      _heartbeat_started{v_noabi_to_v1(std::move(v1::apm::internal::server_heartbeat_started(other)))},
      _heartbeat_failed{v_noabi_to_v1(std::move(v1::apm::internal::server_heartbeat_failed(other)))},
      _heartbeat_succeeded{v_noabi_to_v1(std::move(v1::apm::internal::server_heartbeat_succeeded(other)))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
