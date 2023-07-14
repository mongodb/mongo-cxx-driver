#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

search_index_model::search_index_model(const bsoncxx::document::view_or_value& definition)
    : _definition(definition.view()) {}
search_index_model::search_index_model(const bsoncxx::string::view_or_value name,
                                       const bsoncxx::document::view_or_value& definition)
    : _name(name), _definition(definition.view()) {}

search_index_model::search_index_model(search_index_model&&) noexcept = default;

search_index_model& search_index_model::operator=(search_index_model&&) = default;

search_index_model::search_index_model(const search_index_model&) = default;

search_index_model::~search_index_model() = default;

bsoncxx::stdx::optional<bsoncxx::string::view_or_value> search_index_model::get_name() const {
    return _name;
}

bsoncxx::document::view search_index_model::get_definition() const {
    return _definition.view();
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
