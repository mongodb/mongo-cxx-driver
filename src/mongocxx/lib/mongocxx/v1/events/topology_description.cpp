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

#include <mongocxx/v1/events/topology_description.hh>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/events/server_description.hh>
#include <mongocxx/v1/read_preference.hh>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_topology_description_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_topology_description_t const*>(ptr);
}

} // namespace

bsoncxx::v1::stdx::string_view topology_description::type() const {
    return libmongoc::topology_description_type(to_mongoc(_impl));
}

bool topology_description::has_readable_server(v1::read_preference const& rp) const {
    return libmongoc::topology_description_has_readable_server(
        to_mongoc(_impl), v1::read_preference::internal::as_mongoc(rp));
}

bool topology_description::has_writable_server() const {
    return libmongoc::topology_description_has_writable_server(to_mongoc(_impl));
}

std::vector<v1::events::server_description> topology_description::servers() const {
    std::vector<v1::events::server_description> ret;

    std::size_t n = {};
    auto const sds = libmongoc::topology_description_get_servers(to_mongoc(_impl), &n);

    try {
        ret.reserve(n); // Unlikely, but may throw std::length_error.
    } catch (...) {
        libmongoc::server_descriptions_destroy_all(sds, n);
        throw;
    }

    // Transfer ownership of each element, but not the parent array.
    std::transform(sds, sds + n, std::back_inserter(ret), [](mongoc_server_description_t* sd) {
        return v1::events::server_description::internal::make(sd);
    });
    bson_free(sds);

    return ret;
}

topology_description::topology_description(void const* impl) : _impl{impl} {}

topology_description topology_description::internal::make(mongoc_topology_description_t const* sd) {
    return {sd};
}

mongoc_topology_description_t const* topology_description::internal::as_mongoc(topology_description const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
