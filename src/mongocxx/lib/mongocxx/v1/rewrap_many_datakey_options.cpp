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

#include <mongocxx/v1/rewrap_many_datakey_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <string>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class rewrap_many_datakey_options::impl {
   public:
    std::string _provider;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _master_key;

    static impl const& with(rewrap_many_datakey_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(rewrap_many_datakey_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(rewrap_many_datakey_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(rewrap_many_datakey_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

rewrap_many_datakey_options::~rewrap_many_datakey_options() {
    delete impl::with(this);
}

rewrap_many_datakey_options::rewrap_many_datakey_options(rewrap_many_datakey_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

rewrap_many_datakey_options& rewrap_many_datakey_options::operator=(rewrap_many_datakey_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

rewrap_many_datakey_options::rewrap_many_datakey_options(rewrap_many_datakey_options const& other)
    : _impl{new impl{impl::with(other)}} {}

rewrap_many_datakey_options& rewrap_many_datakey_options::operator=(rewrap_many_datakey_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

rewrap_many_datakey_options::rewrap_many_datakey_options() : _impl{new impl{}} {}

rewrap_many_datakey_options& rewrap_many_datakey_options::provider(std::string provider) {
    impl::with(this)->_provider = std::move(provider);
    return *this;
}

bsoncxx::v1::stdx::string_view rewrap_many_datakey_options::provider() const {
    return impl::with(this)->_provider;
}

rewrap_many_datakey_options& rewrap_many_datakey_options::master_key(bsoncxx::v1::document::value master_key) {
    impl::with(this)->_master_key = std::move(master_key);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> rewrap_many_datakey_options::master_key() const {
    return impl::with(this)->_master_key;
}

std::string& rewrap_many_datakey_options::internal::provider(rewrap_many_datakey_options& self) {
    return impl::with(self)._provider;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& rewrap_many_datakey_options::internal::master_key(
    rewrap_many_datakey_options& self) {
    return impl::with(self)._master_key;
}

} // namespace v1
} // namespace mongocxx
