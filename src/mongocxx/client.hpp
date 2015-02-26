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

#include <memory>

#include <mongocxx/database.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/write_concern.hpp>
#include <mongocxx/options/client.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing a client connection to MongoDB.
///
/// Acts as a logical gateway for working with databases contained within a MongoDB server.
///
/// Databases that are created via this client inherit the @c read_preference and @c write_concern
/// settings of this client when they are created. The lifetimes of objects created via a client
/// object (databases, collections, cursors, etc...) @b must be a subset of the lifetime of the
/// client that created them.
///
/// Example:
/// @code
///   mongocxx::client mongo_client;
///   mongocxx::client mongo_client("mongodb://localhost:27017");
/// @endcode
///
/// @todo Make iterable for databases on the server
/// @todo Add + implement missing client api methods
///
class MONGOCXX_API client {

   public:

    ///
    /// Creates a new client connection to MongoDB.
    ///
    /// @param mongodb_uri
    ///   A MongoDB URI representing the connection parameters
    /// @param options
    ///   Additional options that cannot be specified via the mongodb_uri
    ///
    client(
        const class uri& mongodb_uri = mongocxx::uri(),
        const options::client& options = options::client()
    );

    ///
    /// Move constructs a client.
    ///
    client(client&&) noexcept;

    ///
    /// Move assigns a client.
    ///
    client& operator=(client&&) noexcept;

    ///
    /// Destroys a client.
    ///
    ~client();

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
    /// Sets the read preference for this client.
    ///
    /// Modifications at this level do not effect existing databases instances that have have been
    /// created by this client but do effect new ones as databases inherit the @c read_preference
    /// settings of their parent upon instantiation.
    ///
    /// @param rp
    ///   The new @c read_preference
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    void read_preference(class read_preference rp);

    ///
    /// Returns the current read preference for this client.
    ///
    /// @return The current @c read_preference
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    class read_preference read_preference() const;

    ///
    /// Returns the current uri for this client.
    ///
    /// @return The @c uri that this client was created with.
    ///
    class uri uri() const;

    ///
    /// Sets the write concern for this client.
    ///
    /// @note Modifications at this level do not effect existing databases or collection instances
    /// that have come from this client but do effect new ones as databases will receive a copy of
    /// this client's @c write_concern upon instantiation.
    ///
    /// @param wc
    ///   The new write concern
    ///
    void write_concern(class write_concern wc);

    ///
    /// Returns the current write concern for this client.
    ///
    /// @return the current @c write_concern
    class write_concern write_concern() const;

    ///
    /// Obtains a database that represents a logical grouping of collections on a MongoDB server.
    ///
    /// @note A database cannot be obtained from a temporary client object.
    ///
    /// @param name
    ///   The name of the database to get
    ///
    /// @return The database
    ///
    class database database(const std::string& name) const &;
    class database database(const std::string& name) const && = delete;

    ///
    /// Allows the syntax @c client["db_name"] as a convenient shorthand for the client::database()
    /// method by implementing the array subscript operator.
    ///
    /// @note A database cannot be obtained from a temporary client object.
    ///
    /// @param name
    ///   The name of the database.
    ///
    /// @return Client side representation of a server side database
    ///
    inline MONGOCXX_INLINE class database operator[](const std::string& name) const &;
    inline MONGOCXX_INLINE class database operator[](const std::string& name) const && = delete;

   private:
    friend class database;
    friend class collection;

    class MONGOCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;

};

inline MONGOCXX_INLINE database client::operator[](const std::string& name) const & {
    return database(name);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
