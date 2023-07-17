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

cursor search_index_view::list(const bsoncxx::document::view& aggregation_opts) {
    return _get_impl().list(nullptr, aggregation_opts);
}

cursor search_index_view::list(const client_session& session,
                               const bsoncxx::document::view& aggregation_opts) {
    return _get_impl().list(&session, aggregation_opts);
}

cursor search_index_view::list(bsoncxx::string::view_or_value name,
                               const bsoncxx::document::view& aggregation_opts) {
    return _get_impl().list(nullptr, name, aggregation_opts);
}

cursor search_index_view::list(const client_session& session,
                               bsoncxx::string::view_or_value name,
                               const bsoncxx::document::view& aggregation_opts) {
    return _get_impl().list(&session, name, aggregation_opts);
}

bsoncxx::stdx::optional<bsoncxx::string::view_or_value> search_index_view::create_one(
    bsoncxx::string::view_or_value name, const bsoncxx::document::view_or_value& definition) {
    return create_one(search_index_model(name, definition));
}

bsoncxx::stdx::optional<bsoncxx::string::view_or_value> search_index_view::create_one(
    const client_session& session,
    bsoncxx::string::view_or_value name,
    const bsoncxx::document::view_or_value& definition) {
    return create_one(session, search_index_model(name, definition));
}

bsoncxx::stdx::optional<bsoncxx::string::view_or_value> search_index_view::create_one(
    const search_index_model& model) {
    return _get_impl().create_one(nullptr, model);
}

bsoncxx::stdx::optional<bsoncxx::string::view_or_value> search_index_view::create_one(
    const client_session& session, const search_index_model& model) {
    return _get_impl().create_one(&session, model);
}

std::vector<bsoncxx::string::view_or_value> search_index_view::create_many(
    const std::vector<search_index_model>& models) {
    auto response = _get_impl().create_many(nullptr, models);
    return _create_many_helper(response["indexesCreated"].get_array().value);
}

std::vector<bsoncxx::string::view_or_value> search_index_view::create_many(
    const client_session& session, const std::vector<search_index_model>& models) {
    auto response = _get_impl().create_many(&session, models);
    return _create_many_helper(response["indexesCreated"].get_array().value);
}

std::vector<bsoncxx::string::view_or_value> search_index_view::_create_many_helper(
    bsoncxx::array::view created_indexes) {
    std::vector<bsoncxx::string::view_or_value> search_index_names;
    for (auto&& index : created_indexes) {
        search_index_names.push_back(
            bsoncxx::string::to_string(index.get_document().value["name"].get_string().value));
    }
    return search_index_names;
}

void search_index_view::drop_one(bsoncxx::string::view_or_value name) {
    _get_impl().drop_one(nullptr, name);
}

void search_index_view::drop_one(const client_session& session,
                                 bsoncxx::string::view_or_value name) {
    _get_impl().drop_one(&session, name);
}

void search_index_view::update_one(bsoncxx::string::view_or_value name,
                                   const bsoncxx::document::view_or_value& definition) {
    _get_impl().update_one(nullptr, name, definition);
}

void search_index_view::update_one(const client_session& session,
                                   bsoncxx::string::view_or_value name,
                                   const bsoncxx::document::view_or_value& definition) {
    _get_impl().update_one(&session, name, definition);
}

search_index_view::impl& search_index_view::_get_impl() {
    return *_impl;
}
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx