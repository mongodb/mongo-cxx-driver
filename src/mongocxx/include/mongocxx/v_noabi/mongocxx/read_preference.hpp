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

#include <mongocxx/read_preference-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/detail/macros.hpp>

#include <mongocxx/v1/read_preference.hpp>

#include <chrono>
#include <cstdint>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <string> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <mongocxx/client-fwd.hpp>                      // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/collection-fwd.hpp>                  // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/database-fwd.hpp>                    // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/events/topology_description-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/options/transaction-fwd.hpp>         // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/search_index_view-fwd.hpp>           // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/uri-fwd.hpp>                         // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/options/transaction.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

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
   private:
    v1::read_preference _rp;

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
    read_preference() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ read_preference(v1::read_preference rp) : _rp{std::move(rp)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated iterators and views.
    ///
    explicit operator v1::read_preference() && {
        return std::move(_rp);
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::read_preference() const& {
        return _rp;
    }

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
    MONGOCXX_DEPRECATED read_preference(read_mode mode) : read_preference{mode, deprecated_tag{}} {}

    // @cond DOXYGEN_DISABLE
    read_preference(read_mode mode, deprecated_tag) {
        this->mode(mode);
    }
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
    MONGOCXX_DEPRECATED read_preference(read_mode mode, bsoncxx::v_noabi::document::view_or_value tags)
        : read_preference{mode, tags, deprecated_tag{}} {}

    // @cond DOXYGEN_DISABLE
    read_preference(read_mode mode, bsoncxx::v_noabi::document::view_or_value tags, deprecated_tag) {
        this->mode(mode).tags(tags.view());
    }
    // @endcond

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
    read_preference& mode(read_mode mode) {
        _rp.mode(static_cast<v1::read_preference::read_mode>(mode));
        return *this;
    }

    ///
    /// Returns the current read_mode for this read_preference.
    ///
    /// @return The current read_mode.
    ///
    read_mode mode() const {
        return static_cast<read_mode>(_rp.mode());
    }

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
    read_preference& tags(bsoncxx::v_noabi::document::view_or_value tag_set_list) {
        _rp.tags(bsoncxx::v_noabi::to_v1(tag_set_list.view()));
        return *this;
    }

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
    read_preference& tags(bsoncxx::v_noabi::array::view_or_value tag_set_list) {
        _rp.tags(bsoncxx::v_noabi::to_v1(tag_set_list.view()));
        return *this;
    }

    ///
    /// Returns the current tag set list for this read_preference.
    ///
    /// @return The optionally set current tag set list.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference-tags/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> tags() const {
        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> ret;

        auto const v = _rp.tags();

        if (!v.empty()) {
            ret.emplace(v);
        }

        return ret;
    }

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
    bsoncxx::v_noabi::stdx::optional<std::chrono::seconds> max_staleness() const {
        return _rp.max_staleness();
    }

    BSONCXX_PRIVATE_WARNINGS_PUSH();
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wdeprecated-declarations"));
    BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4996));

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
    MONGOCXX_DEPRECATED read_preference& hedge(bsoncxx::v_noabi::document::view_or_value hedge) {
        _rp.hedge(bsoncxx::v_noabi::to_v1(hedge.view()));
        return *this;
    }

    ///
    /// Gets the current hedge document to be used for the read preference.
    ///
    /// @deprecated Hedged reads are deprecated in MongoDB Server version 8.0.
    ///
    /// @return A hedge document if one was set.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const)
    hedge() const {
        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> ret;
        if (auto const opt = _rp.hedge()) {
            ret.emplace(*opt);
        }
        return ret;
    }

    BSONCXX_PRIVATE_WARNINGS_POP();

    ///
    /// @relates mongocxx::v_noabi::read_preference
    ///
    /// Compares two read_preference objects for (in)-equality.
    ///
    /// @{
    friend bool operator==(read_preference const& lhs, read_preference const& rhs) {
        return (lhs.mode() == rhs.mode()) && (lhs.tags() == rhs.tags()) && (lhs.max_staleness() == rhs.max_staleness());
    }

    friend bool operator!=(read_preference const& lhs, read_preference const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///

    class internal;
};

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::read_preference from_v1(v1::read_preference v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::read_preference to_v1(v_noabi::read_preference v) {
    return v1::read_preference{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::read_preference.
///
/// @par Includes
/// - @ref mongocxx/v1/read_preference.hpp
///
