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

#include <vector>

#include <mongocxx/events/topology_description-fwd.hpp>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/events/server_description.hpp>
#include <mongocxx/read_preference.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace events {

///
/// A description of the topology of one or more connected MongoDB servers.
///
/// @see
/// - mongocxx::v_noabi::events::topology_changed_event
///
class topology_description {
   public:
    ///
    /// An array of server_description instances.
    ///
    class server_descriptions {
       private:
        using container = std::vector<server_description>;

       public:
        ///
        /// Move constructs a server_descriptions array.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() server_descriptions(server_descriptions&&) noexcept;

        ///
        /// Move assigns a server_descriptions array.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(server_descriptions&) operator=(server_descriptions&&) noexcept;

        server_descriptions(server_descriptions const&) = delete;
        server_descriptions& operator=(server_descriptions const&) = delete;

        ///
        /// Destroys a server_descriptions array.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~server_descriptions();

        ///
        /// The array's iterator type.
        ///
        using iterator = container::iterator;

        ///
        /// The array's const iterator type.
        ///
        using const_iterator = container::const_iterator;

        ///
        /// Returns an iterator to the beginning.
        ///
        /// @{
        MONGOCXX_ABI_EXPORT_CDECL(iterator) begin() noexcept;
        MONGOCXX_ABI_EXPORT_CDECL(const_iterator) begin() const noexcept;
        /// @}
        ///

        ///
        /// Returns an iterator to the end.
        ///
        /// @{
        MONGOCXX_ABI_EXPORT_CDECL(iterator) end() noexcept;
        MONGOCXX_ABI_EXPORT_CDECL(const_iterator) end() const noexcept;
        /// @}
        ///

        ///
        /// The number of server_description instances in the array.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(std::size_t) size() const noexcept;

       private:
        friend ::mongocxx::v_noabi::events::topology_description;

        explicit server_descriptions(void* sds, std::size_t size);
        void swap(server_descriptions& other) noexcept;

        container _container;
        void* _sds;
        std::size_t _size;
    };

    explicit topology_description(void* event);

    ///
    /// Destroys a topology_description.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~topology_description();

    topology_description(topology_description&&) = default;
    topology_description& operator=(topology_description&&) = default;

    topology_description(topology_description const&) = default;
    topology_description& operator=(topology_description const&) = default;

    ///
    /// The topology type: "Unknown", "Sharded", "ReplicaSetNoPrimary", "ReplicaSetWithPrimary", or
    /// "Single".
    ///
    /// @return The type as a short-lived string view.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) type() const;

    ///
    /// Determines if the topology has a readable server available. Servers are
    /// filtered by the given read preferences only if the driver is connected
    /// to a replica set, otherwise the read preferences are ignored. This
    /// function uses the driver's current knowledge of the state of the
    /// MongoDB server or servers it is connected to; it does no I/O.
    ///
    /// @return Whether there is a readable server available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool)
    has_readable_server(mongocxx::v_noabi::read_preference const& pref) const;

    ///
    /// Determines if the topology has a writable server available, such as a
    /// primary, mongos, or standalone. This function uses the driver's current
    /// knowledge of the state of the MongoDB server or servers it is connected
    /// to; it does no I/O.
    ///
    /// @return Whether there is a writable server available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) has_writable_server() const;

    ///
    /// Fetches descriptions for all known servers in the topology.
    ///
    /// @return An array of server_description objects.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_descriptions) servers() const;

   private:
    // Non-const since mongoc_topology_description_has_readable_server/writable_server take
    // non-const. They do server selection, which modifies the mongoc_topology_description_t.
    void* _td;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::topology_description.
///
