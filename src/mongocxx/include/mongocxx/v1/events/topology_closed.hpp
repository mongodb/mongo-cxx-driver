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

#include <mongocxx/v1/events/topology_closed-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/oid-fwd.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

namespace mongocxx {
namespace v1 {
namespace events {

///
/// `TopologyClosedEvent` from the SDAM Logging and Monitoring specification.
///
/// @see
/// - [SDAM Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/server-discovery-and-monitoring/server-discovery-and-monitoring-logging-and-monitoring/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class topology_closed {
   private:
    void const* _impl; // mongoc_apm_topology_closed_t const

   public:
    ///
    /// Return the client-generated unique topology ID.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::oid) topology_id() const;

   private:
    /* explicit(false) */ topology_closed(void const* impl);
};

} // namespace events
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::events::topology_closed.
///
