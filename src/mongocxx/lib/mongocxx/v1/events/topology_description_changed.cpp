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

#include <mongocxx/v1/events/topology_description_changed.hh>

//

#include <bsoncxx/v1/oid.hpp>

#include <mongocxx/v1/events/topology_description.hh>

#include <cstdint>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_apm_topology_changed_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_topology_changed_t const*>(ptr);
}

} // namespace

bsoncxx::v1::oid topology_description_changed::topology_id() const {
    bson_oid_t id = {};
    libmongoc::apm_topology_changed_get_topology_id(to_mongoc(_impl), &id);
    return bsoncxx::v1::oid{id.bytes, sizeof(id)};
}

v1::events::topology_description topology_description_changed::previous_description() const {
    return v1::events::topology_description::internal::make(
        libmongoc::apm_topology_changed_get_previous_description(to_mongoc(_impl)));
}

v1::events::topology_description topology_description_changed::new_description() const {
    return v1::events::topology_description::internal::make(
        libmongoc::apm_topology_changed_get_new_description(to_mongoc(_impl)));
}

topology_description_changed::topology_description_changed(void const* impl) : _impl{impl} {}

topology_description_changed topology_description_changed::internal::make(mongoc_apm_topology_changed_t const* ptr) {
    return {ptr};
}

mongoc_apm_topology_changed_t const* topology_description_changed::internal::as_mongoc(
    topology_description_changed const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
