#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/search_index_view.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/search_index_view.hh>

namespace mongocxx {
namespace v_noabi {

search_index_view::search_index_view(void* coll, void* client)
    : _impl{
          bsoncxx::make_unique<impl>(static_cast<mongoc_collection_t*>(coll), static_cast<mongoc_client_t*>(client))} {}

search_index_view::search_index_view(search_index_view&&) noexcept = default;
search_index_view& search_index_view::operator=(search_index_view&&) noexcept = default;

search_index_view::search_index_view(search_index_view const& other)
    : _impl(bsoncxx::make_unique<impl>(other._get_impl())) {}

search_index_view& search_index_view::operator=(search_index_view const& other) {
    _get_impl() = other._get_impl();
    return *this;
}

search_index_view::~search_index_view() = default;

cursor search_index_view::list(options::aggregate const& options) {
    return _get_impl().list(nullptr, options);
}

cursor search_index_view::list(client_session const& session, options::aggregate const& options) {
    return _get_impl().list(&session, options);
}

cursor search_index_view::list(bsoncxx::v_noabi::string::view_or_value name, options::aggregate const& options) {
    return _get_impl().list(nullptr, name, options);
}

cursor search_index_view::list(
    client_session const& session,
    bsoncxx::v_noabi::string::view_or_value name,
    options::aggregate const& options) {
    return _get_impl().list(&session, name, options);
}

std::string search_index_view::create_one(bsoncxx::v_noabi::document::view_or_value definition) {
    return create_one(search_index_model(definition));
}

std::string search_index_view::create_one(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value definition) {
    return create_one(session, search_index_model(definition));
}

std::string search_index_view::create_one(
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    return create_one(search_index_model(name, definition));
}

std::string search_index_view::create_one(
    client_session const& session,
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    return create_one(session, search_index_model(name, definition));
}

std::string search_index_view::create_one(search_index_model const& model) {
    return _get_impl().create_one(nullptr, model);
}

std::string search_index_view::create_one(client_session const& session, search_index_model const& model) {
    return _get_impl().create_one(&session, model);
}

std::vector<std::string> search_index_view::create_many(std::vector<search_index_model> const& models) {
    auto response = _get_impl().create_many(nullptr, models);
    return _create_many_helper(response["indexesCreated"].get_array().value);
}

std::vector<std::string> search_index_view::create_many(
    client_session const& session,
    std::vector<search_index_model> const& models) {
    auto response = _get_impl().create_many(&session, models);
    return _create_many_helper(response["indexesCreated"].get_array().value);
}

std::vector<std::string> search_index_view::_create_many_helper(bsoncxx::v_noabi::array::view created_indexes) {
    std::vector<std::string> search_index_names;
    for (auto&& index : created_indexes) {
        search_index_names.push_back(
            bsoncxx::v_noabi::string::to_string(index.get_document().value["name"].get_string().value));
    }
    return search_index_names;
}

void search_index_view::drop_one(bsoncxx::v_noabi::string::view_or_value name) {
    _get_impl().drop_one(nullptr, name);
}

void search_index_view::drop_one(client_session const& session, bsoncxx::v_noabi::string::view_or_value name) {
    _get_impl().drop_one(&session, name);
}

void search_index_view::update_one(
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    _get_impl().update_one(nullptr, name, definition);
}

void search_index_view::update_one(
    client_session const& session,
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    _get_impl().update_one(&session, name, definition);
}

search_index_view::impl const& search_index_view::_get_impl() const {
    if (!_impl) {
        throw mongocxx::v_noabi::logic_error{error_code::k_invalid_search_index_view};
    }
    return *_impl;
}

search_index_view::impl& search_index_view::_get_impl() {
    auto cthis = const_cast<search_index_view const*>(this);
    return const_cast<search_index_view::impl&>(cthis->_get_impl());
}

} // namespace v_noabi
} // namespace mongocxx
