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

#include <mongocxx/search_index_view-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/search_indexes.hpp> // IWYU pragma: export

#include <string>
#include <utility>
#include <vector>

#include <mongocxx/client_session-fwd.hpp>
#include <mongocxx/collection-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A MongoDB Atlas Search Index.
///
/// @note Not to be confused with a MongoDB index (@ref mongocxx::v_noabi::index_view).
///
class search_index_view {
   private:
    v1::search_indexes _indexes;

   public:
    search_index_view(search_index_view&& other) noexcept = default;
    search_index_view& operator=(search_index_view&& other) noexcept = default;

    MONGOCXX_ABI_EXPORT_CDECL() search_index_view(search_index_view const& other);
    MONGOCXX_ABI_EXPORT_CDECL(search_index_view&) operator=(search_index_view const& other);

    ~search_index_view() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ search_index_view(v1::search_indexes search_index_view)
        : _indexes{std::move(search_index_view)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated objects.
    ///
    explicit operator v1::search_indexes() && {
        return std::move(_indexes);
    }

    ///
    /// This class is not copyable.
    ///
    explicit operator v1::search_indexes() const& {
        return _indexes;
    }

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    list(v_noabi::options::aggregate const& options = {});

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the list operation.
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    list(v_noabi::client_session const& session, v_noabi::options::aggregate const& options = {});

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param name
    ///   The name of the search index to find.
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    list(bsoncxx::v_noabi::string::view_or_value name, v_noabi::options::aggregate const& options = {});

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the list operation.
    /// @param name
    ///   The name of the search index to find.
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    list(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::string::view_or_value name,
        v_noabi::options::aggregate const& options = {});

    ///
    /// This is a convenience method for creating a single search index with a default name.
    ///
    /// @param definition
    ///    The document describing the search index to be created.
    ///
    /// @return The name of the created search index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string)
    create_one(bsoncxx::v_noabi::document::view_or_value definition);

    ///
    /// This is a convenience method for creating a single search index with a default name.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param definition
    ///    The document describing the search index to be created.
    ///
    /// @return The name of the created search index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string)
    create_one(v_noabi::client_session const& session, bsoncxx::v_noabi::document::view_or_value definition);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param name
    ///    The name of the search index to create.
    /// @param definition
    ///    The document describing the search index to be created.
    ///
    /// @return The name of the created search index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string)
    create_one(bsoncxx::v_noabi::string::view_or_value name, bsoncxx::v_noabi::document::view_or_value definition);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param name
    ///   The name of the search index to create.
    /// @param definition
    ///   The document describing the search index to be created.
    ///
    /// @return The name of the created search index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string)
    create_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::string::view_or_value name,
        bsoncxx::v_noabi::document::view_or_value definition);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param model
    ///   The search index model to create.
    ///
    /// @return The name of the created index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(v_noabi::search_index_model const& model);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param model
    ///   The search index model to create.
    ///
    /// @return The name of the created index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string)
    create_one(v_noabi::client_session const& session, v_noabi::search_index_model const& model);

    ///
    /// Creates multiple search indexes in the collection.
    ///
    /// @param models
    ///   The search index models to create.
    ///
    /// @return The names of the created indexes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    create_many(std::vector<v_noabi::search_index_model> const& models);

    ///
    /// Creates multiple search indexes in the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param models
    ///   The search index models to create.
    ///
    /// @return The names of the created indexes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    create_many(v_noabi::client_session const& session, std::vector<v_noabi::search_index_model> const& models);

    ///
    /// Drops a single search index from the collection by the index name.
    ///
    /// @param name
    ///    The name of the search index to drop.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(bsoncxx::v_noabi::string::view_or_value name);

    ///
    /// Drops a single search index from the collection by the index name.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param name
    ///   The name of the search index to drop.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(v_noabi::client_session const& session, bsoncxx::v_noabi::string::view_or_value name);

    ///
    /// Updates a single search index from the collection by the search index name.
    ///
    /// @param name
    ///   The name of the search index to update.
    /// @param definition
    ///   The definition to update the search index to.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    update_one(bsoncxx::v_noabi::string::view_or_value name, bsoncxx::v_noabi::document::view_or_value definition);

    ///
    /// Updates a single search index from the collection by the search index name.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param name
    ///   The name of the search index to update.
    /// @param definition
    ///   The definition to update the search index to.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    update_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::string::view_or_value name,
        bsoncxx::v_noabi::document::view_or_value definition);
};

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::search_index_view from_v1(v1::search_indexes v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::search_indexes to_v1(v_noabi::search_index_view v) {
    return v1::search_indexes{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::search_index_view.
///
/// @par Includes
/// - @ref mongocxx/v1/search_indexes.hpp
///
