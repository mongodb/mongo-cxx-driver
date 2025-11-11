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

#include <mongocxx/v1/database-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/collection-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>
#include <mongocxx/v1/pipeline-fwd.hpp>
#include <mongocxx/v1/read_concern-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/aggregate_options.hpp>
#include <mongocxx/v1/change_stream.hpp>
#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/gridfs/bucket.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB database.
///
/// @see
/// - [Databases and Collections in MongoDB (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/databases-and-collections/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class database {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views, collections, cursors, GridFS buckets, and change streams.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~database();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() database(database&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(database&) operator=(database&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() database(database const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(database&) operator=(database const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() database();

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Run an aggregation framework pipeline.
    ///
    /// @see
    /// - [`aggregate` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/aggregate/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) aggregate(
        v1::pipeline const& pipeline,
        v1::aggregate_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor)
    aggregate(v1::client_session const& session, v1::pipeline const& pipeline, v1::aggregate_options const& opts = {});
    /// @}
    ///

    ///
    /// Run an arbitrary database command.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Database Commands](https://www.mongodb.com/docs/manual/reference/command/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) run_command(bsoncxx::v1::document::view command);

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) run_command(
        v1::client_session const& session,
        bsoncxx::v1::document::view command);
    /// @}
    ///

    ///
    /// Run an arbitrary database command against the requested specific server.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Database Commands](https://www.mongodb.com/docs/manual/reference/command/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) run_command(
        bsoncxx::v1::document::view command,
        std::uint32_t server_id);

    ///
    /// Explicitly create a new collection or view in this database.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`create` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/create/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::collection) create_collection(
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view opts = {},
        bsoncxx::v1::stdx::optional<v1::write_concern> wc = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::collection) create_collection(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view opts = {},
        bsoncxx::v1::stdx::optional<v1::write_concern> wc = {});
    /// @}
    ///

    ///
    /// Drop this database and all associated data files.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`dropDatabase` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/dropDatabase/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void) drop(bsoncxx::v1::stdx::optional<v1::write_concern> wc = {});

    MONGOCXX_ABI_EXPORT_CDECL(void) drop(
        v1::client_session const& session,
        bsoncxx::v1::stdx::optional<v1::write_concern> wc = {});
    /// @}
    ///

    ///
    /// Return true when this database contains a collection with the given name.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) has_collection(bsoncxx::v1::stdx::string_view name) const;

    ///
    /// Return an unsorted list of all collections and views in this database.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`listCollections` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/listCollections/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list_collections(bsoncxx::v1::document::view filter = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list_collections(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter = {});
    /// @}
    ///

    ///
    /// Return an unsorted list of the names of all the collections and views in this database.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`listCollections` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/listCollections/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) list_collection_names(bsoncxx::v1::document::view filter = {});

    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) list_collection_names(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter = {});
    /// @}
    ///

    ///
    /// Return the name of this database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) name() const;

    ///
    /// Set the default "readConcern" to use for operations on this database.
    ///
    /// Overrides the default inherited from the associated client. May be overridden by individual operations.
    ///
    /// @see
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_concern(v1::read_concern rc);

    ///
    /// Return the current default "readConcern" to use for operations on this database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::read_concern) read_concern() const;

    ///
    /// Set the default "readPreference" to use for operations on this database.
    ///
    /// Overrides the default inherited from the associated client. May be overridden by individual operations.
    ///
    /// @see
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_preference(v1::read_preference rp);

    ///
    /// Return the current default "readPreference" to use for operations on this database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::read_preference) read_preference() const;

    ///
    /// Set the default "writeConcern" to use for operations on this database.
    ///
    /// Overrides the default inherited from the associated client. May be overridden by individual operations.
    ///
    /// @see
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) write_concern(v1::write_concern wc);

    ///
    /// Return the current default "writeConcern" to use for operations on this database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::write_concern) write_concern() const;

    ///
    /// Access the collection with the given name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::collection) collection(bsoncxx::v1::stdx::string_view name) const;

    ///
    /// Equivalent to `this->collection(name)`.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::collection) operator[](bsoncxx::v1::stdx::string_view name) const;

    ///
    /// Return a GridFS bucket for this database.
    ///
    /// @note When the "bucketName" field is unset, the default bucket name "fs" is used instead.
    /// @note When the "chunkSizeBytes" field is unset, the default chunk size of 255 KiB is used instead.
    ///
    /// @see
    /// - [GridFS (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::bucket) gridfs_bucket(v1::gridfs::bucket::options const& opts = {}) const;

    ///
    /// Return a change stream subscribed to this database.
    ///
    /// @see
    /// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(v1::change_stream::options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(
        v1::client_session const& session,
        v1::change_stream::options const& opts = {});
    /// @}
    ///

    ///
    /// Return a change stream subscribed to this collection with events filtered/modified by `pipeline`.
    ///
    /// @see
    /// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(
        v1::pipeline const& pipeline,
        v1::change_stream::options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream)
    watch(v1::client_session const& session, v1::pipeline const& pipeline, v1::change_stream::options const& opts = {});
    /// @}
    ///
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::database.
///
