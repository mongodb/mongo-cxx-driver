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

#include <mongocxx/events/topology_changed_event-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/events/topology_description_changed.hpp> // IWYU pragma: export

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
   private:
    v1::events::topology_description_changed _event;

   public:
    ///
    /// @deprecated For internal use only.
    ///
    explicit MONGOCXX_ABI_NO_EXPORT topology_changed_event(void const* event);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ topology_changed_event(v1::events::topology_description_changed const& event)
        : _event{event} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::events::topology_description_changed() const {
        return _event;
    }

    ///
    /// An opaque id, unique to this topology for this mongocxx::v_noabi::client or
    /// mongocxx::v_noabi::pool.
    ///
    /// @return The id.
    ///
    bsoncxx::v_noabi::oid topology_id() const {
        return _event.topology_id();
    }

    ///
    /// The server's description before it changed.
    ///
    /// @return The topology_description.
    ///
    topology_description previous_description() const {
        return _event.previous_description();
    }

    ///
    /// The server's description after it changed.
    ///
    /// @return The topology_description.
    ///
    topology_description new_description() const {
        return _event.new_description();
    }
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::events::topology_changed_event from_v1(v1::events::topology_description_changed const& v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::events::topology_description_changed to_v1(v_noabi::events::topology_changed_event const& v) {
    return v1::events::topology_description_changed{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::topology_changed_event.
///
/// @par Includes
/// - @ref mongocxx/v1/events/topology_description_changed.hpp
///
