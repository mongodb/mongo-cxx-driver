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

#include <mongo/driver/config/prelude.hpp>

#include <memory>

#include <mongo/driver/database.hpp>
#include <mongo/driver/read_preference.hpp>
#include <mongo/driver/uri.hpp>
#include <mongo/driver/write_concern.hpp>
#include <mongo/driver/options/client.hpp>

namespace mongo {
namespace driver {

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
///   mongo::driver::client mongo_client;
///   mongo::driver::client mongo_client("mongodb://localhost:27017");
/// @endcode
///
/// @todo Make iterable for databases on the server
/// @todo Add + implement missing client api methods
///
class LIBMONGOCXX_API client {

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
        const uri& mongodb_uri = uri(),
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
    inline class database operator[](const std::string& name) const &;
    inline class database operator[](const std::string& name) const && = delete;

   private:
    friend class database;
    friend class collection;

    class impl;
    std::unique_ptr<impl> _impl;

};

inline database client::operator[](const std::string& name) const & {
    return database(name);
}

}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
