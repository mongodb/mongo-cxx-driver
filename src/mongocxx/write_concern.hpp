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

#include <mongocxx/config/prelude.hpp>

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <memory>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;
class collection;
class database;
class bulk_write;

///
/// Class representing the server-side requirement for reporting the success of a write
/// operation. The strength of the write concern setting determines the level of guarantees
/// that you will receive from MongoDB regarding write durability.
///
/// Weaker requirements that provide fewer guarantees report on success quickly while stronger
/// requirements that provide greater guarantees will take longer (or potentially forever, if
/// the write_concern's requirements are not satisfied and no timeout is set).
///
/// MongoDB drivers allow for different levels of write concern to better address the specific
/// needs of applications. Clients may adjust write concern to ensure that the most important
/// operations persist successfully to an entire MongoDB deployment. However, for other less
/// critical operations, clients can adjust the write concern to ensure better performance
/// rather than persistence to the entire deployment.
///
/// @see http://docs.mongodb.org/manual/core/write-concern/
///
/// @todo this interface is terrible -- it's exactly what you get with the c-driver essentially.
///
class MONGOCXX_API write_concern {

   public:

    ///
    /// Constructs a new write_concern.
    ///
    write_concern();

    ///
    /// Copy constructs a write_concern.
    ///
    write_concern(const write_concern&);

    ///
    /// Copy assigns a write_concern.
    ///
    write_concern& operator=(const write_concern&);

    ///
    /// Move constructs a write_concern.
    ///
    write_concern(write_concern&&) noexcept;

    ///
    /// Move assigns a write_concern.
    ///
    write_concern& operator=(write_concern&&) noexcept;

    ///
    /// Destroys a write_concern.
    ///
    ~write_concern();

    ///
    /// Sets the fsync parameter for this write concern.
    ///
    /// @param fsync
    ///   If @c true forces the database to fsync all files before reporting a write operation was
    ///   successful.
    ///
    void fsync(bool fsync);

    ///
    /// Gets a handle to the underlying implementation.
    ///
    /// Returned pointer is only valid for the lifetime of this object.
    ///
    /// @deprecated Future versions of the driver reserve the right to change the implementation
    ///   and remove this interface entirely.
    ///
    /// @return Pointer to implementation of this object, or nullptr if not available.
    ///
    MONGOCXX_DEPRECATED void* implementation() const;

    ///
    /// Sets the journal parameter for this write concern.
    ///
    /// @param journal
    ///   If @c true confirms that the mongod instance has written the data to the on-disk journal
    ///   before reporting a write operations was successful. This ensures that data is not lost if
    ///   the mongod instance shuts down unexpectedly.
    ///
    void journal(bool journal);

    ///
    /// Sets the number of nodes that are required to acknowledge the write before the operation is
    /// considered successful. Write operations will block until they have been replicated to the
    /// specified number of servers in a replica set.
    ///
    /// @param confirm_from
    ///   The number of replica set nodes that must acknowledge the write.
    ///
    /// @warning Setting the number of nodes to 0 disables write acknowledgement and all other
    /// write concern options.
    ///
    /// @warning Setting the number of nodes required to an amount greater than the number of
    /// available nodes will cause writes using this write concern to block forever if no timeout
    /// is set.
    ///
    void nodes(std::int32_t confirm_from);

    ///
    /// Requires that a majority of the nodes in a replica set acknowledge a write operation before
    /// it is considered a success. A timeout is required when setting this write concern.
    ///
    /// @param timeout
    ///   The amount of time to wait before the write operation times out if it cannot reach
    ///   the majority of nodes in the replica set.
    ///
    void majority(std::chrono::milliseconds timeout);

    ///
    /// Sets the name representing the server-side getLastErrorMode entry containing the list of
    /// nodes that must acknowledge a write operation before it is considered a success.
    ///
    /// @param tag
    ///   The string representing on of the "getLastErrorModes" in the replica set configuration.
    ///
    void tag(stdx::string_view tag);

    ///
    /// Sets an upper bound on the time a write concern can take to be satisfied. If the write
    /// concern cannot be satisfied within the timeout, the operation is considered a failure.
    ///
    /// @param timeout
    ///   The timeout (in milliseconds) for this write concern.
    ///
    /// @throw exception::write
    ///
    void timeout(std::chrono::milliseconds timeout);

    ///
    /// Gets the current status of the fsync parameter.
    ///
    /// @return @c true if fsync is required, @c false if not.
    ///
    bool fsync() const;

    ///
    /// Gets the current status of the journal parameter.
    ///
    /// @return @c true if journal is required, @c false if not.
    ///
    bool journal() const;

    ///
    /// Gets the current number of nodes that this write_concern requires operations to reach.
    ///
    /// @return The number of required nodes.
    ///
    std::int32_t nodes() const;

    ///
    /// Gets the current getLastErrorMode that is required by this write_concern.
    ///
    /// @return The current getLastErrorMode.
    ///
    stdx::optional<std::string> tag() const;

    ///
    /// Gets whether the majority of nodes is currently required by this write_concern.
    ///
    /// @return The current majority setting.
    ///
    bool majority() const;

    ///
    /// Gets the current timeout for this write_concern.
    ///
    /// @return Current timeout in milliseconds.
    ///
    std::chrono::milliseconds timeout() const;

   private:
    friend client;
    friend collection;
    friend database;
    friend bulk_write;

    class MONGOCXX_PRIVATE impl;

    write_concern(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;

};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
