#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/private/search_index_model.hh>
#include <mongocxx/search_index_model.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

search_index_model::search_index_model(bsoncxx::document::view_or_value definition)
    : _impl{bsoncxx::stdx::make_unique<impl>(definition)} {}
search_index_model::search_index_model(bsoncxx::string::view_or_value name,
                                       bsoncxx::document::view_or_value definition)
    : _impl{bsoncxx::stdx::make_unique<impl>(name, definition)} {}

search_index_model::search_index_model(search_index_model&&) noexcept = default;

search_index_model& search_index_model::operator=(search_index_model&&) noexcept = default;

search_index_model::search_index_model(const search_index_model& other)
    : _impl(bsoncxx::stdx::make_unique<impl>(other._get_impl())) {}

search_index_model& search_index_model::operator=(const search_index_model& other) {
    _get_impl() = other._get_impl();
    return *this;
}

search_index_model::~search_index_model() = default;

bsoncxx::stdx::optional<bsoncxx::string::view_or_value> search_index_model::name() const {
    return _get_impl()._name;
}

bsoncxx::document::view search_index_model::definition() const {
    return _get_impl()._definition.view();
}

const search_index_model::impl& search_index_model::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_search_index_model};
    }
    return *_impl;
}

search_index_model::impl& search_index_model::_get_impl() {
    auto cthis = const_cast<const search_index_model*>(this);
    return const_cast<search_index_model::impl&>(cthis->_get_impl());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
