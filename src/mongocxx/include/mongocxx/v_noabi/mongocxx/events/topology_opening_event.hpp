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

#include <mongocxx/events/topology_opening_event-fwd.hpp>

#include <bsoncxx/oid.hpp>

#include <mongocxx/events/topology_description.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// A new connection to a topology of MongoDB servers.
///
/// @see
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
class topology_opening_event {
   public:
    explicit topology_opening_event(void const* event);

    ///
    /// Destroys a topology_opening_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~topology_opening_event();

    topology_opening_event(topology_opening_event&&) = default;
    topology_opening_event& operator=(topology_opening_event&&) = default;

    topology_opening_event(topology_opening_event const&) = default;
    topology_opening_event& operator=(topology_opening_event const&) = default;

    ///
    /// An opaque id, unique to this topology for this mongocxx::v_noabi::client or
    /// mongocxx::v_noabi::pool.
    ///
    /// @return The id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::oid) topology_id() const;

   private:
    void const* _event;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::topology_opening_event.
///
