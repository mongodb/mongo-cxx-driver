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

#include <mongocxx/events/topology_closed_event-fwd.hpp>

#include <bsoncxx/oid.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// An event notification sent when the driver stops monitoring a server topology and destroys its
/// description.
///
/// @see
/// - "TopologyClosedEvent" in https://github.com/mongodb/specifications/blob/master/source/server-discovery-and-monitoring/server-discovery-and-monitoring.md
///
class topology_closed_event {
   public:
    explicit topology_closed_event(const void* event);

    ///
    /// Destroys a topology_closed_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~topology_closed_event();

    topology_closed_event(topology_closed_event&&) = default;
    topology_closed_event& operator=(topology_closed_event&&) = default;

    topology_closed_event(const topology_closed_event&) = default;
    topology_closed_event& operator=(const topology_closed_event&) = default;

    ///
    /// An opaque id, unique to this topology for this mongocxx::v_noabi::client or
    /// mongocxx::v_noabi::pool.
    ///
    /// @return The id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::oid) topology_id() const;

   private:
    const void* _event;
};

}  // namespace events
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::topology_closed_event.
///
