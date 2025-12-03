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

#include <mongocxx/events/topology_description-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/events/topology_description.hpp> // IWYU pragma: export

#include <cstddef>
#include <vector>

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
   private:
    v1::events::topology_description _td;

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
        iterator begin() noexcept {
            return _container.begin();
        }
        const_iterator begin() const noexcept {
            return _container.begin();
        }
        /// @}
        ///

        ///
        /// Returns an iterator to the end.
        ///
        /// @{
        iterator end() noexcept {
            return _container.end();
        }
        const_iterator end() const noexcept {
            return _container.begin();
        }
        /// @}
        ///

        ///
        /// The number of server_description instances in the array.
        ///
        std::size_t size() const noexcept {
            return _container.size();
        }

       private:
        friend topology_description;

        server_descriptions(void* sds, std::size_t size);

        container _container;
        void* _sds;
        std::size_t _size;
    };

    explicit MONGOCXX_ABI_EXPORT_CDECL() topology_description(void const* td);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ topology_description(v1::events::topology_description const& td) : _td{td} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::events::topology_description() const {
        return _td;
    }

    ///
    /// The topology type: "Unknown", "Sharded", "ReplicaSetNoPrimary", "ReplicaSetWithPrimary", or
    /// "Single".
    ///
    /// @return The type as a short-lived string view.
    ///
    bsoncxx::v_noabi::stdx::string_view type() const {
        return _td.type();
    }

    ///
    /// Determines if the topology has a readable server available. Servers are
    /// filtered by the given read preferences only if the driver is connected
    /// to a replica set, otherwise the read preferences are ignored. This
    /// function uses the driver's current knowledge of the state of the
    /// MongoDB server or servers it is connected to; it does no I/O.
    ///
    /// @return Whether there is a readable server available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) has_readable_server(v_noabi::read_preference const& pref) const;

    ///
    /// Determines if the topology has a writable server available, such as a
    /// primary, mongos, or standalone. This function uses the driver's current
    /// knowledge of the state of the MongoDB server or servers it is connected
    /// to; it does no I/O.
    ///
    /// @return Whether there is a writable server available.
    ///
    bool has_writable_server() const {
        return _td.has_writable_server();
    }

    ///
    /// Fetches descriptions for all known servers in the topology.
    ///
    /// @return An array of server_description objects.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_descriptions) servers() const;
};

} // namespace events
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::events::topology_description from_v1(v1::events::topology_description const& v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::events::topology_description to_v1(v_noabi::events::topology_description const& v) {
    return v1::events::topology_description{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::events::topology_description.
///
/// @par Includes
/// - @ref mongocxx/v1/events/topology_description.hpp
///
