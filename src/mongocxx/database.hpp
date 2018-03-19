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

#include <memory>
#include <string>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/gridfs/bucket.hpp>
#include <mongocxx/options/create_collection.hpp>
#include <mongocxx/options/create_view.hpp>
#include <mongocxx/options/gridfs/bucket.hpp>
#include <mongocxx/options/modify_collection.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;

///
/// Class representing a MongoDB database.
///
/// Acts as a gateway for accessing collections that are contained within a database. It inherits
/// all of its default settings from the client that creates it.
///
/// @todo Add auth functions (add_user, remove_all_users, remove_user)
///
class MONGOCXX_API database {
   public:
    ///
    /// Default constructs a new database. The database is not valid for use and is equivalent
    /// to the state of a moved-from database. The only valid actions to take with a default
    /// constructed database are to assign to it, or destroy it.
    ///
    database() noexcept;

    ///
    /// Move constructs a database.
    ///
    database(database&&) noexcept;

    ///
    /// Move assigns a database.
    ///
    database& operator=(database&&) noexcept;

    ///
    /// Copy constructs a database.
    ///
    database(const database&);

    ///
    /// Copy assigns a database.
    ///
    database& operator=(const database&);

    ///
    /// Destroys a database.
    ///
    ~database();

    ///
    /// Returns true if the client is valid, meaning it was not default constructed
    /// or moved from.
    ///
    explicit operator bool() const noexcept;

    ///
    /// Runs a command against this database.
    ///
    /// @see https://docs.mongodb.com/master/reference/method/db.runCommand/
    ///
    /// @param command document representing the command to be run.
    /// @return the result of executing the command.
    ///
    /// @throws mongocxx::operation_exception if the operation fails.
    ///
    bsoncxx::document::value run_command(bsoncxx::document::view_or_value command);

    ///
    /// Explicitly creates a collection in this database with the specified options.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/create/
    ///
    /// @param name
    ///   the new collection's name.
    /// @param collection_options
    ///   the options for the new collection.
    /// @param write_concern
    ///   the write concern to use for this operation. Will default to database
    ///   set write concern if none passed here.
    ///
    /// @exception
    ///   mongocxx::operation_exception if the operation fails.
    ///
    class collection create_collection(
        bsoncxx::string::view_or_value name,
        const options::create_collection& collection_options = options::create_collection{},
        const stdx::optional<write_concern>& write_concern = {});

    ///
    /// Creates a non-materialized view in this database with the specified options.
    /// Non-materialized views are represented by the @c collection objects, and support many of the
    /// same read-only operations that regular collections do.
    ///
    /// @see https://docs.mongodb.com/master/core/views/
    ///
    /// @param name the name of the view to be created.
    /// @param view_on
    ///   the name of the source view or collection in this database from which to create the view.
    /// @param options the options for the new view.
    ///
    /// @throws mongocxx::operation_exception if the operation fails.
    ///
    class collection create_view(bsoncxx::string::view_or_value name,
                                 bsoncxx::string::view_or_value view_on,
                                 const options::create_view& options = options::create_view());

    ///
    /// Modify an existing collection.
    ///
    /// @deprecated
    ///   This method is deprecated.  To modify an existing collection, invoke the "collMod" command
    ///   with database::run_command().
    ///
    /// @see https://docs.mongodb.com/master/reference/command/collMod/
    ///
    /// @param name the name of the collection to be modified.
    /// @param options the modifications to be performed.
    ///
    /// @return the result of executing the command.
    ///
    MONGOCXX_DEPRECATED bsoncxx::document::value modify_collection(
        stdx::string_view name,
        const options::modify_collection& options = options::modify_collection());

    bsoncxx::document::value modify_collection_deprecated(
        stdx::string_view name,
        const options::modify_collection& options = options::modify_collection());

    ///
    /// Drops the database and all its collections.
    ///
    /// @param write_concern (optional)
    ///   The write concern to be used for this operation. If not passed here, the write concern
    ///   set on the database will be used.
    ///
    /// @exception
    ///   mongocxx::operation_exception if the operation fails.
    ///
    /// @see
    ///   https://docs.mongodb.com/manual/reference/command/dropDatabase/
    ///
    void drop(const bsoncxx::stdx::optional<mongocxx::write_concern>& write_concern = {});

    ///
    /// Checks whether this database contains a collection having the given name.
    ///
    /// @param name the name of the collection.
    ///
    /// @return bool whether the collection exists in this database.
    ///
    /// @throws mongocxx::operation_exception if the underlying 'listCollections'
    /// command fails.
    ///
    bool has_collection(bsoncxx::string::view_or_value name) const;

    ///
    /// Enumerates the collections in this database.
    ///
    /// @param filter
    ///   An optional query expression to filter the returned collections.
    ///
    /// @return mongocxx::cursor containing the collection information.
    ///
    /// @throws mongocxx::operation_exception if the underlying 'listCollections'
    /// command fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/listCollections/
    ///
    cursor list_collections(bsoncxx::document::view_or_value filter = {});

    ///
    /// Get the name of this database.
    ///
    /// @return the name of this database.
    ///
    stdx::string_view name() const;

    ///
    /// Sets the read_concern for this database.
    ///
    /// @note Modifications at this level do not affect existing collection instances that have come
    /// from this database, but do affect new ones. New collections will receive a copy of the
    /// new read_concern for this database upon instantiation.
    ///
    /// @param rc
    ///   The new @c read_concern
    ///
    /// @see https://docs.mongodb.com/master/reference/read-concern/
    ///
    void read_concern(class read_concern rc);

    ///
    /// The current read concern for this database.
    ///
    /// If the read_concern is not explicitly set on this database object, it inherits the
    /// read_concern from its parent client object.
    ///
    /// @return the current read_concern
    ///
    class read_concern read_concern() const;

    ///
    /// Sets the read_preference for this database.
    ///
    /// @note Modifications at this level do not affect existing collection instances that have come
    /// from this database, but do affect new ones. New collections will receive a copy of the
    /// new read_preference for this database upon instantiation.
    ///
    /// @see https://docs.mongodb.com/master/core/read-preference/
    ///
    /// @param rp the new read_preference.
    ///
    void read_preference(class read_preference rp);

    ///
    /// The current read preference for this database.
    ///
    /// @see https://docs.mongodb.com/master/core/read-preference/
    ///
    /// @return the current read_preference
    ///
    class read_preference read_preference() const;

    ///
    /// Sets the write_concern for this database.
    ///
    /// @note Modifications at this level do not affect existing collection instances that have come
    /// from this database, but do affect new ones as new collections will receive a copy of the
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
    class collection collection(bsoncxx::string::view_or_value name) const;

    ///
    /// Allows the db["collection_name"] syntax to be used to access a collection within this
    /// database.
    ///
    /// @param name the name of the collection to get.
    ///
    /// @return the collection.
    ///
    MONGOCXX_INLINE class collection operator[](bsoncxx::string::view_or_value name) const;

    ///
    /// Access a GridFS bucket within this database.
    ///
    /// @param options
    ///   The options for the bucket.
    ///
    /// @return
    ///   The GridFS bucket.
    ///
    /// @note
    ///   See the class comment for `gridfs::bucket` for more information about GridFS.
    ///
    /// @throws mongocxx::logic_error if `options` are invalid.
    ///
    class gridfs::bucket gridfs_bucket(
        const options::gridfs::bucket& options = options::gridfs::bucket()) const;

   private:
    friend class client;
    friend class collection;

    MONGOCXX_PRIVATE database(const class client& client, bsoncxx::string::view_or_value name);

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE impl& _get_impl();
    MONGOCXX_PRIVATE const impl& _get_impl() const;

    std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE collection database::operator[](bsoncxx::string::view_or_value name) const {
    return collection(name);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
