// Copyright 2017 MongoDB Inc.
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

#include <string>
#include <vector>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/options/search_index_view.hpp>
#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing a MongoDB index view.
///
class MONGOCXX_API search_index_view {
   public:
    search_index_view(search_index_view&&) noexcept;
    search_index_view& operator=(search_index_view&&) noexcept;

    ~search_index_view();

    ///
    /// @{
    ///
    /// Returns a cursor over all the search indexes.
    ///
    cursor list();

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the list operation.
    ///
    cursor list(const client_session& session);

    /**
     * This is a convenience method for creating a single serach index.
     *
     * @param name
     *    The name of the search index to create.
     * @param definition
     *    The document describing the search index to be created.
     *
     * @return The name of the created search index.
     *
     */
    bsoncxx::stdx::optional<std::string> create_one(
        const std::string name,
        const bsoncxx::document::view_or_value& definition,
        const options::search_index_view& options = options::search_index_view{});

    /**
     * This is a convenience method for creating a single index.
     *
     * @param model
     *    The search index model to create.
     *
     * @return The name of the created index.
     *
     */
    bsoncxx::stdx::optional<std::string> create_one(
        const search_index_model& model,
        const options::search_index_view& options = options::search_index_view{});

    /**
     * Creates multiple search indexes in the collection.
     *
     * @param models
     *    The search index models to create.
     *
     * @return The names of the created indexes.
     *
     */
    std::vector<std::string> create_many(
        const std::vector<search_index_model>& models,
        const options::search_index_view& options = options::search_index_view{});

    /**
     * Drops a single search index from the collection by the index name.
     *
     * @param name
     *    The name of the search index to drop.
     *
     */
    void drop_one(const std::string name,
                  const options::search_index_view& options = options::search_index_view{});

    /**
     * Updates a single search index from the collection by the index name.
     *
     *
     * @param name
     *    The name of the search index to update.
     *
     */
    void update_one(std::string name,
                    const bsoncxx::document::view_or_value& definition,
                    const options::search_index_view& options = options::search_index_view{});

   private:
    friend class collection;
    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE search_index_view(void* coll, void* client);

    MONGOCXX_PRIVATE impl& _get_impl();

   private:
    std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
