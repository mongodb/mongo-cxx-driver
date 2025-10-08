#pragma once

#include <string>
#include <vector>

#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/search_index_view-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

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
   public:
    MONGOCXX_ABI_EXPORT_CDECL() search_index_view(search_index_view&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(search_index_view&) operator=(search_index_view&&) noexcept;

    MONGOCXX_ABI_EXPORT_CDECL() search_index_view(search_index_view const&);
    MONGOCXX_ABI_EXPORT_CDECL(search_index_view&) operator=(search_index_view const&);

    MONGOCXX_ABI_EXPORT_CDECL() ~search_index_view();

    ///
    /// Returns a cursor over all the search indexes.
    ///
    /// @param options
    ///   Options included in the aggregate operation.
    ///
    /// @return A cursor to the list of the search indexes returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list(options::aggregate const& options = options::aggregate());

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
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list(client_session const& session, options::aggregate const& options = options::aggregate());

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
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list(bsoncxx::v_noabi::string::view_or_value name, options::aggregate const& options = options::aggregate());

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
    MONGOCXX_ABI_EXPORT_CDECL(cursor)
    list(
        client_session const& session,
        bsoncxx::v_noabi::string::view_or_value name,
        options::aggregate const& options = options::aggregate());

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
    create_one(client_session const& session, bsoncxx::v_noabi::document::view_or_value definition);

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
        client_session const& session,
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
    MONGOCXX_ABI_EXPORT_CDECL(std::string) create_one(search_index_model const& model);

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
    create_one(client_session const& session, search_index_model const& model);

    ///
    /// Creates multiple search indexes in the collection.
    ///
    /// @param models
    ///   The search index models to create.
    ///
    /// @return The names of the created indexes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>)
    create_many(std::vector<search_index_model> const& models);

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
    create_many(client_session const& session, std::vector<search_index_model> const& models);

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
    drop_one(client_session const& session, bsoncxx::v_noabi::string::view_or_value name);

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
        client_session const& session,
        bsoncxx::v_noabi::string::view_or_value name,
        bsoncxx::v_noabi::document::view_or_value definition);

   private:
    friend ::mongocxx::v_noabi::collection;

    class impl;

    search_index_view(void* coll, void* client);

    std::vector<std::string> _create_many_helper(bsoncxx::v_noabi::array::view created_indexes);

    impl const& _get_impl() const;

    impl& _get_impl();

   private:
    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::search_index_view.
///
