// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <mongocxx/search_index_model.hh>

//

#include <utility>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {

class search_index_model::impl {
   public:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _name;
    bsoncxx::v_noabi::document::view_or_value _definition;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _type;

    impl(bsoncxx::v_noabi::string::view_or_value name, bsoncxx::v_noabi::document::view_or_value definition)
        : _name(name), _definition(definition.view()) {}

    explicit impl(bsoncxx::v_noabi::document::view_or_value definition) : _definition(definition.view()) {}

    template <typename Self>
    static auto with(Self& self) -> decltype(*self._impl) {
        if (!self._impl) {
            throw v_noabi::logic_error{v_noabi::error_code::k_invalid_search_index_model};
        }
        return *self._impl;
    }

    template <typename Self>
    static auto with(Self* self) -> decltype(self->_impl.get()) {
        if (!self->_impl) {
            throw v_noabi::logic_error{v_noabi::error_code::k_invalid_search_index_model};
        }
        return self->_impl.get();
    }
};

search_index_model::search_index_model(bsoncxx::v_noabi::document::view_or_value definition)
    : _impl{bsoncxx::make_unique<impl>(std::move(definition))} {}

search_index_model::search_index_model(
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition)
    : _impl{bsoncxx::make_unique<impl>(std::move(name), std::move(definition))} {}

search_index_model::search_index_model(search_index_model&& other) noexcept = default;

search_index_model& search_index_model::operator=(search_index_model&& other) noexcept = default;

search_index_model::search_index_model(search_index_model const& other)
    : _impl(bsoncxx::make_unique<impl>(impl::with(other))) {}

// NOLINTNEXTLINE(cert-oop54-cpp): handled by impl.
search_index_model& search_index_model::operator=(search_index_model const& other) {
    *impl::with(this) = impl::with(other);
    return *this;
}

search_index_model::~search_index_model() = default;

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> search_index_model::name() const {
    return impl::with(this)->_name;
}

bsoncxx::v_noabi::document::view search_index_model::definition() const {
    return impl::with(this)->_definition.view();
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> search_index_model::type() const {
    return impl::with(this)->_type;
}

search_index_model& search_index_model::type(bsoncxx::v_noabi::string::view_or_value type) {
    impl::with(this)->_type = type;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& search_index_model::internal::name(
    search_index_model const& self) {
    return impl::with(self)._name;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& search_index_model::internal::type(
    search_index_model const& self) {
    return impl::with(self)._type;
}

} // namespace v_noabi
} // namespace mongocxx
