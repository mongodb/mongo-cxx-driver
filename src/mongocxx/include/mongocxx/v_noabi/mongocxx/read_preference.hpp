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

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/database-fwd.hpp>
#include <mongocxx/events/topology_description-fwd.hpp>
#include <mongocxx/options/transaction-fwd.hpp>
#include <mongocxx/read_preference-fwd.hpp>
#include <mongocxx/search_index_view-fwd.hpp>
#include <mongocxx/uri-fwd.hpp>

#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/options/transaction.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Describes how MongoDB clients route read operations to the members of a replica set or sharded
/// cluster.
///
/// @see
/// - [Read Preference (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/read-preference/)
///
class read_preference {
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
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/#read-preference-modes
    ///
    enum class read_mode : std::uint8_t {
        ///
        /// Only read from a primary node.
        ///
        k_primary,

        ///
        /// Prefer to read from a primary node.
        ///
        k_primary_preferred,

        ///
        /// Only read from secondary nodes.
        ///
        k_secondary,

        ///
        /// Prefer to read from secondary nodes.
        ///
        k_secondary_preferred,

        ///
        /// Read from the node with the lowest latency irrespective of state.
        ///
        k_nearest
    };

    ///
    /// Constructs a new read_preference with read_mode set to k_primary.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_preference();

    // @cond DOXYGEN_DISABLE
    struct deprecated_tag {};
    // @endcond

    ///
    /// Constructs a new read_preference.
    ///
    /// @param mode
    ///   Specifies the read_mode.
    ///
    /// @deprecated Use @ref mode instead.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL() read_preference(read_mode mode);

    // @cond DOXYGEN_DISABLE
    MONGOCXX_ABI_EXPORT_CDECL() read_preference(read_mode mode, deprecated_tag);
    // @endcond

    ///
    /// Constructs a new read_preference with tags.
    ///
    /// @param mode
    ///   A read_preference read_mode.
    /// @param tags
    ///   A document representing tags to use for the read_preference.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/#tag-sets
    ///
    /// @deprecated Use @ref tags instead.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL() read_preference(
        read_mode mode,
        bsoncxx::v_noabi::document::view_or_value tags);

    // @cond DOXYGEN_DISABLE
    MONGOCXX_ABI_EXPORT_CDECL()
    read_preference(read_mode mode, bsoncxx::v_noabi::document::view_or_value tags, deprecated_tag);
    // @endcond

    ///
    /// Copy constructs a read_preference.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_preference(read_preference const&);

    ///
    /// Copy assigns a read_preference.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) operator=(read_preference const&);

    ///
    /// Move constructs a read_preference.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_preference(read_preference&&) noexcept;

    ///
    /// Move assigns a read_preference.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) operator=(read_preference&&) noexcept;

    ///
    /// Destroys a read_preference.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~read_preference();

    ///
    /// Sets a new mode for this read_preference.
    ///
    /// @param mode
    ///   The new read preference mode.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) mode(read_mode mode);

    ///
    /// Returns the current read_mode for this read_preference.
    ///
    /// @return The current read_mode.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_mode) mode() const;

    ///
    /// Sets or updates the tag set list for this read_preference.
    ///
    /// @param tag_set_list
    ///   Document representing the tag set list.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference-tags/
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&)
    tags(bsoncxx::v_noabi::document::view_or_value tag_set_list);

    ///
    /// Sets or updates the tag set list for this read_preference.
    ///
    /// @param tag_set_list
    ///   Array of tag sets.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference-tags/
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&)
    tags(bsoncxx::v_noabi::array::view_or_value tag_set_list);

    ///
    /// Sets or updates the tag set list for this read_preference.
    ///
    /// @return The optionally set current tag set list.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference-tags/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>)
    tags() const;

    ///
    /// Sets the max staleness setting for this read_preference.  Secondary
    /// servers with an estimated lag greater than this value will be excluded
    /// from selection under modes that allow secondaries.
    ///
    /// Max staleness must be at least 90 seconds, and also at least
    /// the sum (in seconds) of the client's heartbeatFrequencyMS and the
    /// server's idle write period, which is 10 seconds.  For general use,
    /// 90 seconds is the effective minimum.  If less, an exception will be
    /// thrown when an operation is attempted.
    ///
    /// Max staleness may only be used with MongoDB version 3.4 or later.
    /// If used with an earlier version, an exception will be thrown when an
    /// operation is attempted.
    ///
    /// @note
    ///     The max-staleness feature is designed to prevent badly-lagging
    ///     servers from being selected.  The staleness estimate is
    ///     imprecise and shouldn't be used to try to select "up-to-date"
    ///     secondaries.
    ///
    /// @param max_staleness
    ///    The new max staleness setting.  It must be positive.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the argument is invalid.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) max_staleness(std::chrono::seconds max_staleness);

    ///
    /// Returns the current max staleness setting for this read_preference.
    ///
    /// @return The optionally current max staleness setting.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::chrono::seconds>)
    max_staleness() const;

    ///
    /// Sets the hedge document to be used for the read preference. Sharded clusters running MongoDB
    /// 4.4 or later can dispatch read operations in parallel, returning the result from the fastest
    /// host and cancelling the unfinished operations.
    ///
    /// This may be an empty document or a document of the form { enabled: &lt;boolean&gt; }.
    ///
    /// Hedged reads are automatically enabled in MongoDB 4.4+ when using a ``nearest`` read
    /// preference. To explicitly enable or disable hedging, the ``hedge`` document must be
    /// passed. An empty document uses server defaults to control hedging, but the ``enabled`` key
    /// may be set to ``true`` or ``false`` to explicitly enable or disable hedged reads.
    ///
    /// @param hedge
    ///   The hedge document to set. For example, the document { enabled: true }.
    ///
    /// @deprecated Hedged reads are deprecated in MongoDB Server version 8.0.
    ///
    /// @return A reference to the object on which this member function is being called. This
    /// facilitates method chaining.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(read_preference&) hedge(
        bsoncxx::v_noabi::document::view_or_value hedge);

    ///
    /// Gets the current hedge document to be used for the read preference.
    ///
    /// @deprecated Hedged reads are deprecated in MongoDB Server version 8.0.
    ///
    /// @return A hedge document if one was set.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<
                                                  bsoncxx::v_noabi::document::view> const)
    hedge() const;

    ///
    /// @relates mongocxx::v_noabi::read_preference
    ///
    /// Compares two read_preference objects for (in)-equality.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(read_preference const&, read_preference const&);
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(read_preference const&, read_preference const&);
    /// @}
    ///

   private:
    friend ::mongocxx::v_noabi::client;
    friend ::mongocxx::v_noabi::collection;
    friend ::mongocxx::v_noabi::database;
    friend ::mongocxx::v_noabi::events::topology_description;
    friend ::mongocxx::v_noabi::options::transaction;
    friend ::mongocxx::v_noabi::search_index_view;
    friend ::mongocxx::v_noabi::uri;

    class impl;

    read_preference(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::read_preference.
///
