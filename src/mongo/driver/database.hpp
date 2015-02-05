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
#include <string>

#include <mongo/driver/collection.hpp>
#include <mongo/driver/write_concern.hpp>
#include <mongo/driver/read_preference.hpp>

namespace mongo {
namespace driver {

class client;

///
/// Class representing a MongoDB database.
///
/// Acts as a gateway for accessing collections that are contained within a database. It inherits
/// all of its default settings from the client that creates it.
///
/// @todo Make iterable for collections in the database
/// @todo Make copyable when c-driver supports this
/// @todo Add auth functions (add_user, remove_all_users, remove_user)

class LIBMONGOCXX_API database {

   public:

    ///
    /// Move constructs a database.
    ///
    database(database&&) noexcept;

    ///
    /// Move assigns a database.
    ///
    database& operator=(database&&) noexcept;

    ///
    /// Destroys a database.
    ///
    ~database();

    ///
    /// Runs a command against this database.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.runCommand/
    ///
    /// @param command document representing the command to be run.
    /// @return the result of executing the command.
    /// @throws operation_exception if the operation fails.
    ///
    bson::document::value command(bson::document::view command);

    ///
    /// Explicitly creates a collection in this database with the specified options.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.createCollection/
    ///
    /// @param name the new collection's name.
    /// @param options the options for the new collection.
    ///
    class collection create_collection(const std::string& name, bson::document::view options);

    ///
    /// Drops the database and all its collections.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.dropDatabase/
    ///
    void drop();

    ///
    /// Checks whether this database contains a collection having the given name.
    ///
    /// @param name the name of the collection.
    /// @return bool whether the collection exists in this database.
    ///
    bool has_collection(const std::string& name);

    ///
    /// Enumerates the collections in this database.
    ///
    /// @return mongo::driver::cursor containing the collection information.
    ///
    cursor list_collections();

    ///
    /// Get the name of this database.
    ///
    /// @return the name of this database.
    ///
    const std::string& name() const;

    ///
    /// Renames this database.
    ///
    /// @param new_name the new name for the database.
    /// @param drop_target_before_rename whether to drop existing databases with the new name.
    ///
    void rename(
        const std::string& new_name,
        bool drop_target_before_rename
    );

    ///
    /// Get server-side statistics for the database.
    ///
    /// @return document containing statistics for this database.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/dbStats/
    ///
    bson::document::value stats();

    ///
    /// Sets the read_preference for this database.
    ///
    /// @note Modifications at this level do not effect existing collection instances that have come
    /// from this database, but do affect new ones. New collections will receive a copy of the
    /// new read_preference for this database upon instantiation.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    /// @param rp the new read_preference.
    ///
    void read_preference(class read_preference rp);

    ///
    /// The current read preference for this database.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    /// @return the current read_preference
    ///
    class read_preference read_preference() const;

    ///
    /// Sets the write_concern for this database.
    ///
    /// @note Modifications at this level do not effect existing collection instances that have come
    /// from this database, but do effect new ones as new collections will receive a copy of the
    /// write_concern of this database upon instantiation.
    ///
    void write_concern(class write_concern wc);

    ///
    /// The current write_concern for this database.
    ///
    /// @return the current write_concern
    ///
    class write_concern write_concern() const;

    ///
    /// Access a collection (logical grouping of documents) within this database.
    ///
    /// @param name the name of the collection to get.
    ///
    /// @return the collection.
    ///
    class collection collection(const std::string& name) const;

    ///
    /// Allows the db["collection_name"] syntax to be used to access a collection within this database.
    ///
    /// @param name the name of the collection to get.
    ///
    /// @return the collection.
    ///
    inline class collection operator[](const std::string& name) const;

   private:
    friend class client;
    friend class collection;

    database(const class client& client, const std::string& name);

    class impl;
    std::unique_ptr<impl> _impl;

};

inline collection database::operator[](const std::string& name) const {
    return collection(name);
}

}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
