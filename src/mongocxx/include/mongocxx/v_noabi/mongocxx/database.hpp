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

#include <memory>
#include <string>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/client_encryption-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/database-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/gridfs/bucket.hpp>
#include <mongocxx/options/gridfs/bucket.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A MongoDB database.
///
/// Acts as a gateway for accessing collections that are contained within a database. It inherits
/// all of its default settings from the client that creates it.
///
class database {
   public:
    ///
    /// Default constructs a new database. The database is not valid for use and is equivalent
    /// to the state of a moved-from database. The only valid actions to take with a default
    /// constructed database are to assign to it, or destroy it.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() database() noexcept;

    ///
    /// Move constructs a database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() database(database&&) noexcept;

    ///
    /// Move assigns a database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(database&) operator=(database&&) noexcept;

    ///
    /// Copy constructs a database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() database(database const&);

    ///
    /// Copy assigns a database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(database&) operator=(database const&);

    ///
    /// Destroys a database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~database();

    ///
    /// Returns true if the client is valid, meaning it was not default constructed
    /// or moved from.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const noexcept;

    ///
    /// Runs an aggregation framework pipeline against this database for
    /// pipeline stages that do not require an underlying collection,
    /// such as $currentOp and $listLocalSessions.
    ///
    /// @param pipeline
    ///   The pipeline of aggregation operations to perform.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::aggregate.
    ///
    /// @return A mongocxx::v_noabi::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::v_noabi::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/#dbcmd.aggregate
    ///
    /// @note
    ///   In order to pass a read concern to this, you must use the
    ///   database level set read concern - database::read_concern(rc).
    ///   (Write concern supported only for MongoDB 3.4+).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    aggregate(pipeline const& pipeline, options::aggregate const& options = options::aggregate());

    ///
    /// Runs an aggregation framework pipeline against this database for
    /// pipeline stages that do not require an underlying collection,
    /// such as $currentOp and $listLocalSessions.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    /// @param pipeline
    ///   The pipeline of aggregation operations to perform.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::aggregate.
    ///
    /// @return A mongocxx::v_noabi::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::v_noabi::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/#dbcmd.aggregate
    ///
    /// @note
    ///   In order to pass a read concern to this, you must use the
    ///   database level set read concern - database::read_concern(rc).
    ///   (Write concern supported only for MongoDB 3.4+).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    aggregate(
        client_session const& session,
        pipeline const& pipeline,
        options::aggregate const& options = options::aggregate());

    ///
    /// Runs a command against this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/db.runCommand/
    ///
    /// @param command document representing the command to be run.
    /// @return the result of executing the command.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
    run_command(bsoncxx::v_noabi::document::view_or_value command);

    ///
    /// Runs a command against this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/db.runCommand/
    ///
    /// @param session The mongocxx::v_noabi::client_session with which to run the command.
    /// @param command document representing the command to be run.
    /// @return the result of executing the command.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
    run_command(client_session const& session, bsoncxx::v_noabi::document::view_or_value command);

    ///
    /// Executes a command on a specific server using this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/db.runCommand/
    ///
    /// @param command document representing the command to be run.
    /// @param server_id specifying which server to use.
    /// @return the result of executing the command.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
    run_command(bsoncxx::v_noabi::document::view_or_value command, uint32_t server_id);

    ///
    /// Explicitly creates a collection in this database with the specified options.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/create/
    ///
    /// @note This function can also be used to create a Time Series Collection. See:
    /// https://www.mongodb.com/docs/manual/core/timeseries-collections/
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
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::collection)
    create_collection(
        bsoncxx::v_noabi::stdx::string_view name,
        bsoncxx::v_noabi::document::view_or_value collection_options = {},
        bsoncxx::v_noabi::stdx::optional<write_concern> const& write_concern = {});

    ///
    /// Explicitly creates a collection in this database with the specified options.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/create/
    ///
    /// @note This function can also be used to create a Time Series Collection. See:
    /// https://www.mongodb.com/docs/manual/core/timeseries-collections/
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the create operation.
    /// @param name
    ///   the new collection's name.
    /// @param collection_options
    ///   the options for the new collection.
    /// @param write_concern
    ///   the write concern to use for this operation. Will default to database
    ///   set write concern if none passed here.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::collection)
    create_collection(
        client_session const& session,
        bsoncxx::v_noabi::stdx::string_view name,
        bsoncxx::v_noabi::document::view_or_value collection_options = {},
        bsoncxx::v_noabi::stdx::optional<write_concern> const& write_concern = {});

    ///
    /// Drops the database and all its collections.
    ///
    /// @param write_concern (optional)
    ///   The write concern to be used for this operation. If not passed here, the write concern
    ///   set on the database will be used.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/dropDatabase/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern = {});

    ///
    /// Drops the database and all its collections.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    /// @param write_concern (optional)
    ///   The write concern to be used for this operation. If not passed here, the write concern
    ///   set on the database will be used.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/dropDatabase/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop(
        client_session const& session,
        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern = {});

    ///
    /// Checks whether this database contains a collection having the given name.
    ///
    /// @param name the name of the collection.
    ///
    /// @return bool whether the collection exists in this database.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listCollections'
    /// command fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool)
    has_collection(bsoncxx::v_noabi::string::view_or_value name) const;

    ///
    /// Enumerates the collections in this database.
    ///
    /// @param filter
    ///   An optional query expression to filter the returned collections.
    ///
    /// @return mongocxx::v_noabi::cursor containing the collection information.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listCollections/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list_collections(bsoncxx::v_noabi::document::view_or_value filter = {});

    ///
    /// Enumerates the collections in this database.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    /// @param filter
    ///   An optional query expression to filter the returned collections.
    ///
    /// @return mongocxx::v_noabi::cursor containing the collection information.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listCollections/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list_collections(client_session const& session, bsoncxx::v_noabi::document::view_or_value filter = {});

    ///
    /// Enumerates the collection names in this database.
    ///
    /// @param filter
    ///   An optional query expression to filter the returned collection names.
    ///
    /// @return std::vector<std::string> containing the collection names.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listCollections'
    /// command fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listCollections/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    list_collection_names(bsoncxx::v_noabi::document::view_or_value filter = {});

    ///
    /// Enumerates the collection names in this database.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    /// @param filter
    ///   An optional query expression to filter the returned collection names.
    ///
    /// @return std::vector<std::string> containing the collection names.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listCollections'
    /// command fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listCollections/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    list_collection_names(client_session const& session, bsoncxx::v_noabi::document::view_or_value filter = {});

    ///
    /// Get the name of this database.
    ///
    /// @return the name of this database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) name() const;

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/read-concern/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_concern(mongocxx::v_noabi::read_concern rc);

    ///
    /// The current read concern for this database.
    ///
    /// If the read_concern is not explicitly set on this database object, it inherits the
    /// read_concern from its parent client object.
    ///
    /// @return the current read_concern
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::read_concern) read_concern() const;

    ///
    /// Sets the read_preference for this database.
    ///
    /// @note Modifications at this level do not affect existing collection instances that have come
    /// from this database, but do affect new ones. New collections will receive a copy of the
    /// new read_preference for this database upon instantiation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/
    ///
    /// @param rp the new read_preference.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_preference(mongocxx::v_noabi::read_preference rp);

    ///
    /// The current read preference for this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/
    ///
    /// @return the current read_preference
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::read_preference) read_preference() const;

    ///
    /// Sets the write_concern for this database.
    ///
    /// @note Modifications at this level do not affect existing collection instances that have come
    /// from this database, but do affect new ones as new collections will receive a copy of the
    /// write_concern of this database upon instantiation.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) write_concern(mongocxx::v_noabi::write_concern wc);

    ///
    /// The current write_concern for this database.
    ///
    /// @return the current write_concern
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::write_concern) write_concern() const;

    ///
    /// Access a collection (logical grouping of documents) within this database.
    ///
    /// @param name the name of the collection to get.
    ///
    /// @return the collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::collection)
    collection(bsoncxx::v_noabi::string::view_or_value name) const;

    ///
    /// Allows the db["collection_name"] syntax to be used to access a collection within this
    /// database.
    ///
    /// @param name the name of the collection to get.
    ///
    /// @return the collection.
    ///
    mongocxx::v_noabi::collection operator[](bsoncxx::v_noabi::string::view_or_value name) const {
        return collection(name);
    }

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
    /// @throws mongocxx::v_noabi::logic_error if `options` are invalid.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(gridfs::bucket)
    gridfs_bucket(options::gridfs::bucket const& options = options::gridfs::bucket()) const;

    ///
    /// Gets a change stream on this database with an empty pipeline.
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream) watch(options::change_stream const& options = {});

    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the watch operation.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream)
    watch(client_session const& session, options::change_stream const& options = {});

    ///
    /// Gets a change stream on this database.
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param pipe
    ///   The aggregation pipeline to be used on the change notifications.
    ///   Only a subset of pipeline operations are supported for change streams. For more
    ///   information see the change streams documentation.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream)
    watch(pipeline const& pipe, options::change_stream const& options = {});

    ///
    /// Gets a change stream on this database.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the watch operation.
    /// @param pipe
    ///   The aggregation pipeline to be used on the change notifications.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this database.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream)
    watch(client_session const& session, pipeline const& pipe, options::change_stream const& options = {});

   private:
    friend ::mongocxx::v_noabi::client_encryption;
    friend ::mongocxx::v_noabi::client;
    friend ::mongocxx::v_noabi::collection;

    database(mongocxx::v_noabi::client const& client, bsoncxx::v_noabi::string::view_or_value name);

    cursor _aggregate(client_session const* session, pipeline const& pipeline, options::aggregate const& options);

    bsoncxx::v_noabi::document::value _run_command(
        client_session const* session,
        bsoncxx::v_noabi::document::view_or_value command);

    mongocxx::v_noabi::collection _create_collection(
        client_session const* session,
        bsoncxx::v_noabi::stdx::string_view name,
        bsoncxx::v_noabi::document::view_or_value collection_options,
        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern);

    cursor _list_collections(client_session const* session, bsoncxx::v_noabi::document::view_or_value filter);

    std::vector<std::string> _list_collection_names(
        client_session const* session,
        bsoncxx::v_noabi::document::view_or_value filter);

    void _drop(
        client_session const* session,
        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern);

    change_stream _watch(client_session const* session, pipeline const& pipe, options::change_stream const& options);

    class impl;

    impl& _get_impl();
    impl const& _get_impl() const;

    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::database.
///
