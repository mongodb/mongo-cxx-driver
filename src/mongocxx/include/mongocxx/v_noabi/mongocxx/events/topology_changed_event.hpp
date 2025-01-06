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

#include <mongocxx/events/topology_changed_event-fwd.hpp>

#include <bsoncxx/oid.hpp>

#include <mongocxx/events/topology_description.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// A change in the topology description (including its server descriptions).
///
/// @see
/// - [SDAM Logging and Monitoring Specification (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
class topology_changed_event {
   public:
    explicit topology_changed_event(void const* event);

    ///
    /// Destroys a topology_changed_event.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~topology_changed_event();

    topology_changed_event(topology_changed_event&&) = default;
    topology_changed_event& operator=(topology_changed_event&&) = default;

    topology_changed_event(topology_changed_event const&) = default;
    topology_changed_event& operator=(topology_changed_event const&) = default;

    ///
    /// An opaque id, unique to this topology for this mongocxx::v_noabi::client or
    /// mongocxx::v_noabi::pool.
    ///
    /// @return The id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::oid) topology_id() const;

    ///
    /// The server's description before it changed.
    ///
    /// @return The topology_description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(topology_description) previous_description() const;

    ///
    /// The server's description after it changed.
    ///
    /// @return The topology_description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(topology_description) new_description() const;

   private:
    void const* _event;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::topology_changed_event.
///
