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
#include <stdexcept>
#include <string>

#include <mongocxx/bulk_write-fwd.hpp>
#include <mongocxx/client-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/database-fwd.hpp>
#include <mongocxx/options/transaction-fwd.hpp>
#include <mongocxx/uri-fwd.hpp>
#include <mongocxx/write_concern-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/options/transaction.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// The level of acknowledgment requested for write operations to a MongoDB server.
///
/// @see
/// - [Write Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/write-concern/)
///
class write_concern {
   public:
    ///
    /// A class to represent the write concern level for write operations.
    ///
    /// @see
    /// - [Write Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/write-concern/)
    /// - [Default MongoDB Read Concerns/Write Concerns](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/#write-concern)
    ///
    enum class level {
        k_default,        ///< Represent the implicit default write concern.
        k_majority,       ///< Represent write concern with `w: "majority"`.
        k_tag,            ///< Represent write concern with `w: <custom write concern name>`.
        k_unacknowledged, ///< Represent write concern with `w: 0`.
        k_acknowledged,   ///< Represent write concern with `w: 1`.
    };

    ///
    /// Constructs a new write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write_concern();

    ///
    /// Copy constructs a write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write_concern(write_concern const&);

    ///
    /// Copy assigns a write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) operator=(write_concern const&);

    ///
    /// Move constructs a write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write_concern(write_concern&&) noexcept;

    ///
    /// Move assigns a write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) operator=(write_concern&&) noexcept;

    ///
    /// Destroys a write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~write_concern();

    ///
    /// Sets the journal parameter for this write concern.
    ///
    /// @param journal
    ///   If @c true confirms that the mongod instance has written the data to the on-disk journal
    ///   before reporting a write operations was successful. This ensures that data is not lost if
    ///   the mongod instance shuts down unexpectedly.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) journal(bool journal);

    ///
    /// Sets the number of nodes that are required to acknowledge the write before the operation is
    /// considered successful. Write operations will block until they have been replicated to the
    /// specified number of servers in a replica set.
    ///
    /// @param confirm_from
    ///   The number of replica set nodes that must acknowledge the write.
    ///
    /// @warning Setting the number of nodes to 0 disables write acknowledgment and all other
    /// write concern options.
    ///
    /// @warning Setting the number of nodes required to an amount greater than the number of
    /// available nodes will cause writes using this write concern to block forever if no timeout
    /// is set.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) nodes(std::int32_t confirm_from);

    ///
    /// Sets the acknowledge level.
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/write-concern/#w-option
    ///
    /// @param confirm_level
    ///   Either level::k_unacknowledged, level::k_acknowledged, level::k_default, or
    ///   level::k_majority.
    ///
    /// @note
    ///   the acknowledge level of level::k_tag is set automatically when a tag is set.
    ///
    /// @warning
    ///   Setting this to level::k_unacknowledged disables write acknowledgment and all other
    ///   write concern options.
    ///
    /// @warning
    ///   Unacknowledged writes using dollar-prefixed or dotted keys may be silently rejected by
    ///   pre-5.0 servers.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::exception for setting a tag acknowledge level. Use tag()
    ///   instead.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) acknowledge_level(level confirm_level);

    ///
    /// Requires that a majority of the nodes in a replica set acknowledge a write operation before
    /// it is considered a success.
    ///
    /// @param timeout
    ///   The amount of time to wait before the write operation times out if it cannot reach
    ///   the majority of nodes in the replica set. If the value is zero, then no timeout is set.
    ///
    /// @throws mongocxx::v_noabi::logic_error for an invalid timeout value.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) majority(std::chrono::milliseconds timeout);

    ///
    /// Sets the name representing the server-side getLastErrorMode entry containing the list of
    /// nodes that must acknowledge a write operation before it is considered a success.
    ///
    /// @note the acknowledge level of level::k_tag is set automatically when a tag is set.
    ///
    /// @param tag
    ///   The string representing on of the "getLastErrorModes" in the replica set configuration.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) tag(bsoncxx::v_noabi::stdx::string_view tag);

    ///
    /// Sets an upper bound on the time a write concern can take to be satisfied. If the write
    /// concern cannot be satisfied within the timeout, the operation is considered a failure.
    ///
    /// @param timeout
    ///   The timeout (in milliseconds) for this write concern. If the value is zero, then no
    ///   timeout is set.
    ///
    /// @throws mongocxx::v_noabi::logic_error for an invalid timeout value.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) timeout(std::chrono::milliseconds timeout);

    ///
    /// Gets the current status of the journal parameter.
    ///
    /// @return @c true if journal is required, @c false if not.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) journal() const;

    ///
    /// Gets the current number of nodes that this write_concern requires operations to reach.
    /// This value will be unset if the acknowledge_level is set to majority, default, or tag.
    ///
    /// This is unset by default.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/write-concern/#w-option
    ///
    /// @return The number of required nodes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>) nodes() const;

    ///
    /// Gets the current acknowledgment level.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/write-concern/#w-option
    ///
    /// @return The acknowledgment level.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(level) acknowledge_level() const;

    ///
    /// Gets the current getLastErrorMode that is required by this write_concern.
    ///
    /// @return The current getLastErrorMode.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::string>) tag() const;

    ///
    /// Gets whether the majority of nodes is currently required by this write_concern.
    ///
    /// @return The current majority setting.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) majority() const;

    ///
    /// Gets the current timeout for this write_concern.
    ///
    /// @return Current timeout in milliseconds.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::chrono::milliseconds) timeout() const;

    ///
    /// Gets whether this write_concern requires an acknowledged write.
    ///
    /// @return Whether this write concern requires an acknowledged write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) is_acknowledged() const;

    ///
    /// Gets the document form of this write_concern.
    ///
    /// @return
    ///   Document representation of this write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value) to_document() const;

    ///
    /// @relates mongocxx::v_noabi::write_concern
    ///
    /// Compares two write_concern objects for (in)-equality.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(write_concern const&, write_concern const&);
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(write_concern const&, write_concern const&);
    /// @}
    ///

   private:
    friend ::mongocxx::v_noabi::bulk_write;
    friend ::mongocxx::v_noabi::client;
    friend ::mongocxx::v_noabi::collection;
    friend ::mongocxx::v_noabi::database;
    friend ::mongocxx::v_noabi::options::transaction;
    friend ::mongocxx::v_noabi::uri;

    class impl;

    write_concern(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::write_concern.
///
