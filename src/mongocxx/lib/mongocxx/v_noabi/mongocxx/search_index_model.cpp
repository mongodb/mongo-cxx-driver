#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/search_index_model.hpp>

#include <mongocxx/search_index_model.hh>

#include <bsoncxx/private/make_unique.hh>

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

// NOLINTNEXTLINE(cert-oop54-cpp): handled by impl.
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

template <typename Self>
auto search_index_model::_get_impl(Self& self) -> decltype(*self._impl) {
    if (!self._impl) {
        throw logic_error{error_code::k_invalid_search_index_model};
    }
    return *self._impl;
}

search_index_model::impl const& search_index_model::_get_impl() const {
    return _get_impl(*this);
}

search_index_model::impl& search_index_model::_get_impl() {
    return _get_impl(*this);
}

} // namespace v_noabi
} // namespace mongocxx
