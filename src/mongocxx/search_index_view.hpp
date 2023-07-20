#pragma once

#include <string>
#include <vector>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing a MongoDB search index view.
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
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    cursor list(const options::aggregate& options = options::aggregate());

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the list operation.
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    cursor list(const client_session& session,
                const options::aggregate& options = options::aggregate());

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
    cursor list(bsoncxx::string::view_or_value name,
                const options::aggregate& options = options::aggregate());

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the list operation.
    /// @param name
    ///   The name of the search index to find.
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    cursor list(const client_session& session,
                bsoncxx::string::view_or_value name,
                const options::aggregate& options = options::aggregate());

    ///
    /// @}
    ///

    ///
    /// @{
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
    bsoncxx::string::view_or_value create_one(bsoncxx::string::view_or_value name,
                                              bsoncxx::document::view_or_value definition);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the operation.
    /// @param name
    ///   The name of the search index to create.
    /// @param definition
    ///   The document describing the search index to be created.
    ///
    /// @return The name of the created search index.
    ///
    bsoncxx::string::view_or_value create_one(const client_session& session,
                                              bsoncxx::string::view_or_value name,
                                              bsoncxx::document::view_or_value definition);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param model
    ///   The search index model to create.
    ///
    /// @return The name of the created index.
    ///
    bsoncxx::string::view_or_value create_one(const search_index_model& model);

    ///
    /// This is a convenience method for creating a single search index.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the operation.
    /// @param model
    ///   The search index model to create.
    ///
    /// @return The name of the created index.
    ///
    bsoncxx::string::view_or_value create_one(const client_session& session,
                                              const search_index_model& model);

    ///
    /// @}
    ///

    ///
    /// @{
    ///
    /// Creates multiple search indexes in the collection.
    ///
    /// @param models
    ///   The search index models to create.
    ///
    /// @return The names of the created indexes.
    ///
    std::vector<bsoncxx::string::view_or_value> create_many(
        const std::vector<search_index_model>& models);

    ///
    /// Creates multiple search indexes in the collection.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the operation.
    /// @param models
    ///   The search index models to create.
    ///
    /// @return The names of the created indexes.
    ///
    std::vector<bsoncxx::string::view_or_value> create_many(
        const client_session& session, const std::vector<search_index_model>& models);

    ///
    /// @}
    ///

    ///
    /// @{
    ///
    /// Drops a single search index from the collection by the index name.
    ///
    /// @param name
    ///    The name of the search index to drop.
    ///
    void drop_one(bsoncxx::string::view_or_value name);

    ///
    /// Drops a single search index from the collection by the index name.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the operation.
    /// @param name
    ///   The name of the search index to drop.
    ///
    void drop_one(const client_session& session, bsoncxx::string::view_or_value name);

    ///
    /// @}
    ///

    ///
    /// @{
    ///
    /// Updates a single search index from the collection by the search index name.
    ///
    /// @param name
    ///   The name of the search index to update.
    /// @param definition
    ///   The definition to update the search index to.
    ///
    void update_one(bsoncxx::string::view_or_value name,
                    bsoncxx::document::view_or_value definition);

    ///
    /// Updates a single search index from the collection by the search index name.
    ///
    /// @param session
    ///   The mongocxx::client_session with which to perform the operation.
    /// @param name
    ///   The name of the search index to update.
    /// @param definition
    ///   The definition to update the search index to.
    ///
    void update_one(const client_session& session,
                    bsoncxx::string::view_or_value name,
                    bsoncxx::document::view_or_value definition);

    ///
    /// @}
    ///

   private:
    friend class collection;
    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE search_index_view(void* coll, void* client);

    MONGOCXX_PRIVATE std::vector<bsoncxx::string::view_or_value> _create_many_helper(
        bsoncxx::array::view created_indexes);

    MONGOCXX_PRIVATE const impl& _get_impl() const;

    MONGOCXX_PRIVATE impl& _get_impl();

   private:
    std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
