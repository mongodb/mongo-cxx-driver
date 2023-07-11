#pragma once

#include <string>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing a search index on a MongoDB server.
///
class MONGOCXX_API search_index_model {
   public:
    ///
    /// Initializes a new search_index_model over a mongocxx::collection.
    ///
    search_index_model(const bsoncxx::document::view_or_value& definition);
    search_index_model(const std::string name, const bsoncxx::document::view_or_value& definition);

    search_index_model() = delete;

    ///
    /// Move constructs a search_index_model.
    ///
    search_index_model(search_index_model&&) noexcept;

    ///
    /// Move assigns a search_index_model.
    ///
    search_index_model& operator=(search_index_model&&);

    ///
    /// Copy constructs a search_index_model.
    ///
    search_index_model(const search_index_model&);

    search_index_model& operator=(const search_index_model&) = delete;

    ///
    /// Destroys a search_index_model.
    ///
    ~search_index_model();

    ///
    /// Retrieves name of a search_index_model.
    ///
    bsoncxx::stdx::optional<std::string> get_name() const;

    ///
    /// Retrieves definition of a search_index_model.
    ///
    bsoncxx::document::view get_definition() const;

   private:
    bsoncxx::stdx::optional<std::string> _name;
    bsoncxx::document::value _definition;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>