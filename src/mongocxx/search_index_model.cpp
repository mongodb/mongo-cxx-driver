#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

search_index_model::search_index_model(const bsoncxx::document::view_or_value& document)
    : _document(document.view()) {}
search_index_model::search_index_model(const std::string name,
                                       const bsoncxx::document::view_or_value& document)
    : _name(name), _document(document.view()) {}

search_index_model::search_index_model(search_index_model&&) noexcept = default;

search_index_model& search_index_model::operator=(search_index_model&&) noexcept = default;

search_index_model::search_index_model(const search_index_model&) = default;

search_index_model::~search_index_model() = default;

bsoncxx::stdx::optional<std::string> search_index_model::get_name() const {
    return _name;
}

bsoncxx::document::view search_index_model::get_document() const {
    return _document.view();
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
