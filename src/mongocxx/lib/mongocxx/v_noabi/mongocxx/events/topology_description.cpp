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

#include <mongocxx/events/server_description.hpp>
#include <mongocxx/events/topology_description.hpp>

//

#include <mongocxx/v1/events/topology_description.hh>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>

#include <mongocxx/read_preference.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v_noabi {
namespace events {

topology_description::server_descriptions::server_descriptions(server_descriptions&& other) noexcept
    : _container{std::move(other._container)}, _sds{exchange(other._sds, nullptr)}, _size{exchange(other._size, 0u)} {}

topology_description::server_descriptions& topology_description::server_descriptions::operator=(
    server_descriptions&& other) noexcept {
    if (this != &other) {
        _container = std::move(other._container);

        auto const old_sds = exchange(_sds, exchange(other._sds, nullptr));
        auto const old_size = exchange(_size, exchange(other._size, 0u));

        libmongoc::server_descriptions_destroy_all(static_cast<mongoc_server_description_t**>(old_sds), old_size);
    }

    return *this;
}

topology_description::server_descriptions::~server_descriptions() {
    libmongoc::server_descriptions_destroy_all(static_cast<mongoc_server_description_t**>(_sds), _size);
}

topology_description::server_descriptions::server_descriptions(void* sds, std::size_t size) : _sds{sds}, _size{size} {
    auto const ptr = static_cast<mongoc_server_description_t**>(_sds);
    _container.reserve(size);
    std::transform(ptr, ptr + size, std::back_inserter(_container), [](mongoc_server_description_t* sd) {
        return server_description{sd};
    });
}

topology_description::topology_description(void const* event)
    : _td{v1::events::topology_description::internal::make(static_cast<mongoc_topology_description_t const*>(event))} {}

bool topology_description::has_readable_server(v_noabi::read_preference const& pref) const {
    return libmongoc::topology_description_has_readable_server(
        v1::events::topology_description::internal::as_mongoc(_td),
        v_noabi::read_preference::internal::as_mongoc(pref));
}

topology_description::server_descriptions topology_description::servers() const {
    std::size_t n = {};
    auto const sds =
        libmongoc::topology_description_get_servers(v1::events::topology_description::internal::as_mongoc(_td), &n);
    return {static_cast<void*>(sds), n};
}

} // namespace events
} // namespace v_noabi
} // namespace mongocxx
