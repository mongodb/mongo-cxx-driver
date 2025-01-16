#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/search_index_model.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/search_index_model.hh>

namespace mongocxx {
namespace v_noabi {

search_index_model::search_index_model(bsoncxx::v_noabi::document::view_or_value definition)
    : _impl{bsoncxx::make_unique<impl>(definition)} {}
search_index_model::search_index_model(
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition)
    : _impl{bsoncxx::make_unique<impl>(name, definition)} {}

search_index_model::search_index_model(search_index_model&&) noexcept = default;

search_index_model& search_index_model::operator=(search_index_model&&) noexcept = default;

search_index_model::search_index_model(search_index_model const& other)
    : _impl(bsoncxx::make_unique<impl>(other._get_impl())) {}

search_index_model& search_index_model::operator=(search_index_model const& other) {
    _get_impl() = other._get_impl();
    return *this;
}

search_index_model::~search_index_model() = default;

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> search_index_model::name() const {
    return _get_impl()._name;
}

bsoncxx::v_noabi::document::view search_index_model::definition() const {
    return _get_impl()._definition.view();
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> search_index_model::type() const {
    return _get_impl()._type;
}

search_index_model& search_index_model::type(bsoncxx::v_noabi::string::view_or_value type) {
    _get_impl()._type = type;
    return *this;
}

search_index_model::impl const& search_index_model::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_search_index_model};
    }
    return *_impl;
}

search_index_model::impl& search_index_model::_get_impl() {
    auto cthis = const_cast<search_index_model const*>(this);
    return const_cast<search_index_model::impl&>(cthis->_get_impl());
}

} // namespace v_noabi
} // namespace mongocxx
