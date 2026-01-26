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

#include <mongocxx/v1/client-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/apm-fwd.hpp>
#include <mongocxx/v1/auto_encryption_options-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>
#include <mongocxx/v1/database-fwd.hpp>
#include <mongocxx/v1/pipeline-fwd.hpp>
#include <mongocxx/v1/server_api-fwd.hpp>
#include <mongocxx/v1/tls-fwd.hpp>
#include <mongocxx/v1/uri-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/change_stream.hpp>
#include <mongocxx/v1/client_session.hpp>
#include <mongocxx/v1/config/export.hpp>

#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// A connection to a MongoDB deployment.
///
/// @important This interface does NOT fully conform to the CMAP specification!
///
/// @see
/// - [Connection Monitoring and Pooling (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/connection-monitoring-and-pooling/connection-monitoring-and-pooling/)
/// - [Connection Strings (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/connection-string/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class client {
   private:
    class impl;
    void* _impl;

   public:
    class options;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views, databases, cursors, client sessions, and change streams.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~client();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client(client&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) operator=(client&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    client(client const& other) = delete;

    ///
    /// This class is not copyable.
    ///
    client& operator=(client const& other) = delete;

    ///
    /// Initialize with the given URI.
    ///
    /// @note No connection is attempted until the first command executed with this client object or with an associated
    /// object obtained from this client object. Server-side errors will only be encountered during or after the first
    /// command executed.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL() client(v1::uri uri, options opts);

    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() client(v1::uri uri);
    /// @}
    ///

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client();

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Return the @ref mongocxx::v1::uri used to initialize this client.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::uri) uri() const;

    ///
    /// Access the database with the given name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::database) database(bsoncxx::v1::stdx::string_view name);

    ///
    /// Equivalent to `this->database(name)`.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::database) operator[](bsoncxx::v1::stdx::string_view name);

    ///
    /// Return an unsorted list of all existing databases.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`listDatabases` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/listdatabases/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list_databases();

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list_databases(v1::client_session const& session);

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list_databases(bsoncxx::v1::document::view opts);

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list_databases(
        v1::client_session const& session,
        bsoncxx::v1::document::view opts);
    /// @}
    ///

    ///
    /// Return an unsorted list of the names of all existing databases.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`listDatabases` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/listdatabases/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) list_database_names(bsoncxx::v1::document::view filter);

    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) list_database_names(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter);

    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) list_database_names();

    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) list_database_names(v1::client_session const& session);
    /// @}
    ///

    ///
    /// Create a new client session to use with subsequent operations.
    ///
    /// @important The client session object MUST be passed as the first argument to all operations that are intended to
    /// be executed in the context of a session.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Server Sessions (MongoDB Manual)](https://mongodb.com/docs/manual/reference/server-sessions/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::client_session) start_session(v1::client_session::options const& opts);

    MONGOCXX_ABI_EXPORT_CDECL(v1::client_session) start_session();
    /// @}
    ///

    ///
    /// Return a change stream subscribed to this connection.
    ///
    /// @see
    /// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(v1::change_stream::options const& opts);

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch();

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(
        v1::client_session const& session,
        v1::change_stream::options const& opts);

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(v1::client_session const& session);
    /// @}
    ///

    ///
    /// Return a change stream subscribed to this connection with events filtered/modified by `pipeline`.
    ///
    /// @see
    /// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(
        v1::pipeline const& pipeline,
        v1::change_stream::options const& opts);

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream)
    watch(v1::client_session const& session, v1::pipeline const& pipeline, v1::change_stream::options const& opts);

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(v1::pipeline const& pipeline);

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(v1::client_session const& session, v1::pipeline const& pipeline);
    /// @}
    ///

    ///
    /// Invalidate this client object without invaliding existing cursors or sessions.
    ///
    /// @warning Do not call this member function on a client obtained from a @ref v1::pool.
    ///
    /// This function must be invoked by a (forked) child process to prevent its destruction within the child process
    /// from invalidating the state of the client object within the parent process.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @see
    /// - [`mongoc_client_reset`](https://mongoc.org/libmongoc/current/mongoc_client_reset.html)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) reset();

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::client.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,              ///< Zero.
        tls_not_enabled,   ///< TLS is not enabled by URI options.
        tls_not_supported, ///< TLS is not supported by the mongoc library.
    };

    ///
    /// The error category for @ref mongocxx::v1::client::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }

    class internal;

   private:
    /* explicit(false) */ client(void* impl);
};

///
/// Options for @ref mongocxx::v1::client.
///
/// Supported fields include:
/// - `apm_opts`
/// - `auto_encryption_opts`
/// - `server_api_opts`
/// - `tls_opts`
///
/// @attention This feature is experimental! It is not ready for use!
///
class client::options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "tls_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) tls_opts(v1::tls v);

    ///
    /// Return the current "tls_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::tls>) tls_opts() const;

    ///
    /// Set the "auto_encryption_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) auto_encryption_opts(v1::auto_encryption_options v);

    ///
    /// Return the current "auto_encryption_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::auto_encryption_options>) auto_encryption_opts() const;

    ///
    /// Set the "apm_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) apm_opts(v1::apm v);

    ///
    /// Return the current "apm_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::apm>) apm_opts() const;

    ///
    /// Set the "server_api_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) server_api_opts(v1::server_api v);

    ///
    /// Return the current "server_api_opts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::server_api>) server_api_opts() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::client::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::client.
///
