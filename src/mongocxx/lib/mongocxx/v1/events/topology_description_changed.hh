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

#include <mongocxx/v1/events/topology_description_changed.hpp> // IWYU pragma: export

//

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {
namespace events {

class topology_description_changed::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(topology_description_changed) make(
        mongoc_apm_topology_changed_t const* ptr);

    static mongoc_apm_topology_changed_t const* as_mongoc(topology_description_changed const& self);
};

} // namespace events
} // namespace v1
} // namespace mongocxx
