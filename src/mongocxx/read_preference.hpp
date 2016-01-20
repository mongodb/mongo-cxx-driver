// Copyright 2014 MongoDB Inc.
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

#include <cstdint>
#include <string>
#include <memory>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;
class collection;
class database;
class uri;

///
/// Class representing a preference for how the driver routes read operations to members of a
/// replica set or to a sharded cluster.
///
/// By default read operations are directed to the primary member in a replica set. Reading from the
/// primary guarantees that read operations reflect the latest version of a document. However, by
/// distributing some or all reads to secondary members of the replica set, you can improve read
/// throughput or reduce latency for an application that does not require fully up-to-date data.
///
/// Read preference can be broadly specified by setting a mode. It is also possible to
/// set tags in the read preference for more granular control, and to target specific members of a
/// replica set via attributes other than their current state as a primary or secondary node.
///
/// Read preferences are ignored for direct connections to a single mongod instance. However,
/// in order to perform read operations on a direct connection to a secondary member of a replica
/// set, you must set a read preference that allows reading from secondaries.
///
/// @see http://docs.mongodb.org/manual/core/read-preference/
///
class MONGOCXX_API read_preference {
   public:
    ///
    /// Determines which members in a replica set are acceptable to read from.
    ///
    /// @warning Read preference tags are not respected when the mode is set to primary.
    ///
    /// @warning All read preference modes except primary may return stale data because secondaries
    /// replicate operations from the primary with some delay. Ensure that your application
    /// can tolerate stale data if you choose to use a non-primary mode.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/#read-preference-modes
    ///
    enum class read_mode : std::uint8_t {
        ///
        /// Only read from a primary node.
        ///
        k_primary = 0x01,

        ///
        /// Prefer to read from a primary node.
        ///
        k_primary_preferred = 0x05,

        ///
        /// Only read from secondary nodes.
        ///
        k_secondary = 0x02,

        ///
        /// Prefer to read from secondary nodes.
        ///
        k_secondary_preferred = 0x06,

        ///
        /// Read from the node with the lowest latency irrespective of state.
        ///
        k_nearest = 0x0A
    };

    ///
    /// Constructs a new read_preference.
    ///
    /// @param mode
    ///   Optional parameter to specify the read_mode, defaults to k_primary.
    ///
    explicit read_preference(read_mode mode = read_mode::k_primary);

    ///
    /// Constructs a new read_preference with tags.
    ///
    /// @param mode
    ///   A read_preference read_mode.
    /// @param tags
    ///   A document representing tags to use for the read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/#tag-sets
    ///
    read_preference(read_mode mode, bsoncxx::document::view_or_value tags);

    ///
    /// Copy constructs a read_preference.
    ///
    read_preference(const read_preference&);

    ///
    /// Copy assigns a read_preference.
    ///
    read_preference& operator=(const read_preference&);

    ///
    /// Move constructs a read_preference.
    ///
    read_preference(read_preference&&) noexcept;

    ///
    /// Move assigns a read_preference.
    ///
    read_preference& operator=(read_preference&&) noexcept;

    ///
    /// Destroys a read_preference.
    ///
    ~read_preference();

    ///
    /// Sets a new mode for this read_preference.
    ///
    /// @param mode
    ///   The new read preference mode.
    ///
    void mode(read_mode mode);

    ///
    /// Returns the current read_mode for this read_preference.
    ///
    /// @return The current read_mode.
    ///
    read_mode mode() const;

    ///
    /// Sets or updates the tags for this read_preference.
    ///
    /// @param tags
    ///   Document representing the tags.
    ///
    void tags(bsoncxx::document::view_or_value tags);

    ///
    /// Returns the current tags for this read_preference.
    ///
    /// @return The optionally set current tags.
    ///
    stdx::optional<bsoncxx::document::view> tags() const;

   private:
    friend client;
    friend collection;
    friend database;
    friend uri;
    friend MONGOCXX_API bool MONGOCXX_CALL
    operator==(const read_preference&, const read_preference&);

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE read_preference(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
