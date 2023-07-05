#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/private/search_index_view.hh>
#include <mongocxx/search_index_view.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

search_index_view::search_index_view(void* coll, void* client)
    : _impl{stdx::make_unique<impl>(static_cast<mongoc_collection_t*>(coll),
                                    static_cast<mongoc_client_t*>(client))} {}

search_index_view::search_index_view(search_index_view&&) noexcept = default;
search_index_view& search_index_view::operator=(search_index_view&&) noexcept = default;
search_index_view::~search_index_view() = default;

std::string search_index_view::create_one(const std::string name,
                                          const bsoncxx::document::view_or_value& definition,
                                          const options::search_index_view& options) {
    return create_one(search_index_model(name, definition), options);
}

std::string search_index_view::create_one(const search_index_model& model,
                                          const options::search_index_view& options) {
    throw "IMPLEMENT ME";
    return "";
}

std::vector<std::string> search_index_view::create_many(
    const std::vector<search_index_model>& models, const options::search_index_view& options) {
    throw "IMPLEMENT ME";
    return {};
}

void search_index_view::drop_one(const std::string name,
                                 const options::search_index_view& options) {
    throw "IMPLEMENT ME";
}

void search_index_view::update_one(std::string name, const options::search_index_view& options) {
    throw "IMPLEMENT ME";
}

search_index_view::impl& search_index_view::_get_impl() {
    return *_impl;
}
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx