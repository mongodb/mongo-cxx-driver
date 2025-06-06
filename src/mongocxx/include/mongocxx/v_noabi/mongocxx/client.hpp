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
#include <mongocxx/client_session-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/database-fwd.hpp>
#include <mongocxx/options/auto_encryption-fwd.hpp>
#include <mongocxx/options/client_encryption-fwd.hpp>
#include <mongocxx/pool-fwd.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/client_encryption.hpp>
#include <mongocxx/options/client_session.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A client connection to a MongoDB server.
///
/// Acts as a logical gateway for working with databases contained within a MongoDB server.
///
/// Databases that are created via this client inherit the @c read_concern, @c read_preference, and
/// @c write_concern settings of this client when they are created. The lifetimes of objects created
/// via a client object (databases, collections, cursors, etc...) @b must be a subset of the
/// lifetime of the client that created them.
///
/// @par Example
/// ```cpp
/// mongocxx::v_noabi::client mongo_client{mongocxx::v_noabi::uri{}};
/// mongocxx::v_noabi::client mongo_client{mongocxx::v_noabi::uri{"mongodb://localhost:27017"}};
/// ```
///
/// Note that client is not thread-safe. See
/// https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/thread-safety/ for more details.
class client {
   public:
    ///
    /// Default constructs a new client. The client is not connected and is equivalent to the
    /// state of a moved-from client. The only valid actions to take with a default constructed
    /// 'client' are to assign to it, or destroy it.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client() noexcept;

    ///
    /// Creates a new client connection to MongoDB.
    ///
    /// @param mongodb_uri
    ///   A MongoDB URI representing the connection parameters
    /// @param options
    ///   Additional options that cannot be specified via the mongodb_uri
    ///
    /// @throws mongocxx::v_noabi::exception if invalid options are provided
    /// (whether from the URI or provided client options).
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    client(mongocxx::v_noabi::uri const& mongodb_uri, options::client const& options = options::client());

    ///
    /// Move constructs a client.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client(client&&) noexcept;

    ///
    /// Move assigns a client.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) operator=(client&&) noexcept;

    ///
    /// Destroys a client.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~client();

    client(client const&) = delete;
    client& operator=(client const&) = delete;

    ///
    /// Returns true if the client is valid, meaning it was not default constructed
    /// or moved from.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const noexcept;

    ///
    /// Sets the read concern for this client.
    ///
    /// Modifications at this level do not affect existing database instances that have been
    /// created by this client but do affect new ones as databases inherit the @c read_concern
    /// settings of their parent upon instantiation.
    ///
    /// @deprecated
    ///   This method is deprecated. Read concerns should be set either in the URI or directly on
    ///   database or collection objects.
    ///
    /// @param rc
    ///   The new @c read_concern
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/read-concern/
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(void) read_concern(mongocxx::v_noabi::read_concern rc);

    MONGOCXX_ABI_EXPORT_CDECL(void) read_concern_deprecated(mongocxx::v_noabi::read_concern rc);

    ///
    /// Returns the current read concern for this client.
    ///
    /// @return The current @c read_concern
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::read_concern) read_concern() const;

    ///
    /// Sets the read preference for this client.
    ///
    /// Modifications at this level do not affect existing database instances that have been
    /// created by this client but do affect new ones as databases inherit the @c read_preference
    /// settings of their parent upon instantiation.
    ///
    /// @deprecated
    ///   This method is deprecated. Read preferences should be set either in the URI or directly on
    ///   database or collection objects.
    ///
    /// @param rp
    ///   The new @c read_preference
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(void) read_preference(mongocxx::v_noabi::read_preference rp);

    MONGOCXX_ABI_EXPORT_CDECL(void)
    read_preference_deprecated(mongocxx::v_noabi::read_preference rp);

    ///
    /// Returns the current read preference for this client.
    ///
    /// @return The current @c read_preference
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::read_preference) read_preference() const;

    ///
    /// Returns the current uri for this client.
    ///
    /// @return The @c uri that this client was created with.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::uri) uri() const;

    ///
    /// Sets the write concern for this client.
    ///
    /// @note Modifications at this level do not affect existing databases or collection instances
    /// that have come from this client but do affect new ones as databases will receive a copy of
    /// this client's @c write_concern upon instantiation.
    ///
    /// @deprecated
    ///   This method is deprecated. Write concerns should be set either in the URI or directly on
    ///   database or collection objects.
    ///
    /// @param wc
    ///   The new write concern
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(void) write_concern(mongocxx::v_noabi::write_concern wc);

    MONGOCXX_ABI_EXPORT_CDECL(void) write_concern_deprecated(mongocxx::v_noabi::write_concern wc);

    ///
    /// Returns the current write concern for this client.
    ///
    /// @return the current @c write_concern
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::write_concern) write_concern() const;

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
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::database)
    database(bsoncxx::v_noabi::string::view_or_value name) const&;

    mongocxx::v_noabi::database database(bsoncxx::v_noabi::string::view_or_value name) const&& = delete;

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
    mongocxx::v_noabi::database operator[](bsoncxx::v_noabi::string::view_or_value name) const& {
        return database(name);
    }

    mongocxx::v_noabi::database operator[](bsoncxx::v_noabi::string::view_or_value name) const&& = delete;
    ///
    /// Enumerates the databases in the client.
    ///
    /// @return A mongocxx::v_noabi::cursor containing a BSON document for each
    ///   database. Each document contains a name field with the database
    ///   name, a sizeOnDisk field with the total size of the database file on
    ///   disk in bytes, and an empty field specifying whether the database
    ///   has any data.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listDatabases' command
    /// fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listDatabases
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor) list_databases() const;

    ///
    /// Enumerates the databases in the client.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    ///
    /// @return A mongocxx::v_noabi::cursor containing a BSON document for each
    ///   database. Each document contains a name field with the database
    ///   name, a sizeOnDisk field with the total size of the database file on
    ///   disk in bytes, and an empty field specifying whether the database
    ///   has any data.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listDatabases' command
    /// fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listDatabases
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor) list_databases(client_session const& session) const;

    ///
    /// Enumerates the databases in the client.
    ///
    /// @param opts
    ///   Options passed directly to the 'listDatabases' command.
    ///
    /// @return A mongocxx::v_noabi::cursor containing a BSON document for each
    ///   database. Each document contains a name field with the database
    ///   name, a sizeOnDisk field with the total size of the database file on
    ///   disk in bytes, and an empty field specifying whether the database
    ///   has any data.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listDatabases' command
    /// fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listDatabases
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list_databases(bsoncxx::v_noabi::document::view_or_value const opts) const;

    ///
    /// Enumerates the databases in the client.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    ///
    /// @param opts
    ///   Options passed directly to the 'listDatabases' command.
    ///
    /// @return A mongocxx::v_noabi::cursor containing a BSON document for each
    ///   database. Each document contains a name field with the database
    ///   name, a sizeOnDisk field with the total size of the database file on
    ///   disk in bytes, and an empty field specifying whether the database
    ///   has any data.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listDatabases' command
    /// fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listDatabases
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list_databases(client_session const& session, bsoncxx::v_noabi::document::view_or_value const opts) const;

    ///
    /// Queries the MongoDB server for a list of known databases.
    ///
    /// @param filter
    ///   An optional query expression to filter the returned database names.
    ///
    /// @return std::vector<std::string> containing the database names.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listDatabases'
    /// command fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listDatabases
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    list_database_names(bsoncxx::v_noabi::document::view_or_value const filter = {}) const;

    ///
    /// Queries the MongoDB server for a list of known databases.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    ///
    /// @param filter
    ///   An optional query expression to filter the returned database names.
    ///
    /// @return std::vector<std::string> containing the database names.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the underlying 'listDatabases'
    /// command fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listDatabases
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    list_database_names(client_session const& session, bsoncxx::v_noabi::document::view_or_value const filter = {})
        const;

    ///
    /// Create a client session for a sequence of operations.
    ///
    /// @return A client_session object. See `mongocxx::v_noabi::client_session` for more
    /// information.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the driver is not built with crypto
    /// support, if options is misconfigured, or if the session is configured with options that the
    /// server does not support.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_session)
    start_session(options::client_session const& options = {});

    ///
    /// Get a change stream on this client with an empty pipeline.
    ///
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this client.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream) watch(options::change_stream const& options = {});

    ///
    /// Get a change stream on this client with an empty pipeline.
    ///
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the watch operation.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this client.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream)
    watch(client_session const& session, options::change_stream const& options = {});

    ///
    /// Get a change stream on this client.
    ///
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
    ///  A change stream on this client.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream)
    watch(pipeline const& pipe, options::change_stream const& options = {});

    ///
    /// Get a change stream on this client.
    ///
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the watch operation.
    /// @param pipe
    ///   The aggregation pipeline to be used on the change notifications.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this client.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream)
    watch(client_session const& session, pipeline const& pipe, options::change_stream const& options = {});

    ///
    /// Prevents resource cleanup in the child process from interfering
    /// with the parent process after forking.
    ///
    /// Clients should not be reused after forking. Call this method in the
    /// child after forking to safely destroy the client. This method should
    /// not be used with multi-threaded clients.
    ///
    /// This method causes the client to clear its session pool without sending
    /// endSessions.  It also increments an internal generation counter on the
    /// given client. After this method is called, cursors from
    /// previous generations will not issue a killCursors command when
    /// they are destroyed. Client sessions from previous generations
    /// cannot be used and should be destroyed.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) reset();

   private:
    friend ::mongocxx::v_noabi::client_session;
    friend ::mongocxx::v_noabi::collection;
    friend ::mongocxx::v_noabi::database;
    friend ::mongocxx::v_noabi::options::auto_encryption;
    friend ::mongocxx::v_noabi::options::client_encryption;
    friend ::mongocxx::v_noabi::pool;

    explicit client(void* implementation);

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
/// Provides @ref mongocxx::v_noabi::client.
///
