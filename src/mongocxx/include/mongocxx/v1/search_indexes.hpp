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

#include <mongocxx/v1/search_indexes-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/aggregate_options.hpp>
#include <mongocxx/v1/config/export.hpp>

#include <string>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// Support for MongoDB Atlas Search indexes.
///
/// @note Not to be confused with collection indexes or Atlas Vector Search indexes.
///
/// @see
/// - [Queries and Indexes (MongoDB Manual)](https://www.mongodb.com/docs/atlas/atlas-search/searching/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class search_indexes {
   private:
    class impl;
    void* _impl;

   public:
    class model;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~search_indexes();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() search_indexes(search_indexes&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(search_indexes&) operator=(search_indexes&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() search_indexes(search_indexes const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(search_indexes&) operator=(search_indexes const& other);

    ///
    /// Return information for all search indexes in the associated collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list(v1::aggregate_options const& opts = {});

    ///
    /// Return information for all search indexes in the associated collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list(
        v1::client_session const& session,
        v1::aggregate_options const& opts = {});

    ///
    /// Return information for the search index with the given name in the associated collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list(
        bsoncxx::v1::stdx::string_view name,
        v1::aggregate_options const& opts = {});

    ///
    /// Return information for the search index with the given name in the associated collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor)
    list(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view name,
        v1::aggregate_options const& opts = {});

    ///
    /// Create a single search index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single search index model.
    ///
    /// @returns The name of the new search index.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(bsoncxx::v1::document::view definition);

    ///
    /// Create a single search index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single search index model.
    ///
    /// @returns The name of the new search index.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(
        v1::client_session const& session,
        bsoncxx::v1::document::view definition);

    ///
    /// Create a single search index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single search index model.
    ///
    /// @returns The name of the new search index.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view definition);

    ///
    /// Create a single search index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single search index model.
    ///
    /// @returns The name of the new search index.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string)
    create_one(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view definition);

    ///
    /// Create a single search index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single search index model.
    ///
    /// @returns The name of the new search index.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(model const& index);

    ///
    /// Create a single search index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single search index model.
    ///
    /// @returns The name of the new search index.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(client_session const& session, model const& index);

    ///
    /// Create multiple search indexes in the associated collection.
    ///
    /// @returns The names of the new search indexes
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) create_many(std::vector<search_indexes::model> const& indexes);

    ///
    /// Create multiple search indexes in the associated collection.
    ///
    /// @returns The names of the new search indexes.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`createSearchIndexes` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/createSearchIndexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) create_many(
        client_session const& session,
        std::vector<model> const& indexes);

    ///
    /// Drop the search index with the given name in the associated collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`dropSearchIndex` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/dropSearchIndex/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(bsoncxx::v1::stdx::string_view name);

    ///
    /// Drop the search index with the given name in the associated collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`dropSearchIndex` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/dropSearchIndex/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(v1::client_session const& session, bsoncxx::v1::stdx::string_view name);

    ///
    /// Update the definition of the search index with the given name in the associated collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`updateSearchIndex` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/updateSearchIndex/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) update_one(
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view definition);

    ///
    /// Update the definition of the search index with the given name in the associated collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`updateSearchIndex` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/updateSearchIndex/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    update_one(
        client_session const& session,
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view definition);
};

///
/// A description of a MongoDB Atlas Search index.
///
/// Supported fields include:
/// - `definition`
/// - `name`
/// - `type`
///
/// @see
/// - [Queries and Indexes (MongoDB Manual)](https://www.mongodb.com/docs/atlas/atlas-search/searching/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class search_indexes::model {
    // This class implements both `SearchIndexModel` and `SearchIndexOptions` ("Index View API"):
    //  - https://specifications.readthedocs.io/en/latest/index-management/index-management/

   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~model();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() model(model&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(model&) operator=(model&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() model(model const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(model&) operator=(model const& other);

    ///
    /// Initialize this search index model with the given "name" and "definition".
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    model(bsoncxx::v1::stdx::string_view name, bsoncxx::v1::document::value definition);

    ///
    /// Initialize this search index model with the given "name".
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    model(bsoncxx::v1::document::value definition);

    ///
    /// Return the current "name" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) name() const;

    ///
    /// Return the current "definition" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) definition() const;

    ///
    /// Return the current "type" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) type() const;

    ///
    /// Set the "type" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(model&) type(bsoncxx::v1::stdx::string_view type);
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::search_indexes.
///
