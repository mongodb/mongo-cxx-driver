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

#include <mongocxx/v1/events/topology_description-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/events/server_description-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <vector>

namespace mongocxx {
namespace v1 {
namespace events {

///
/// `TopologyDescription` from the SDAM Logging and Monitoring specification.
///
/// @see
/// - [SDAM Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class topology_description {
   private:
    void const* _impl; // mongoc_topology_description_t const

   public:
    ///
    /// Return the topology type.
    ///
    /// @returns One of:
    /// - "ReplicaSetNoPrimary"
    /// - "ReplicaSetWithPrimary"
    /// - "Sharded"
    /// - "Single"
    /// - "Unknown"
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) type() const;

    ///
    /// Return true when a readable server is available according to this topology description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) has_readable_server(v1::read_preference const& rp) const;

    ///
    /// Return true when a writable server is available according to this topology description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) has_writable_server() const;

    ///
    /// Return the server descriptions for all servers in this topology description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<v1::events::server_description>) servers() const;

   private:
    /* explicit(false) */ topology_description(void const* impl);
};

} // namespace events
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::events::topology_description.
///
