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

#include <mongocxx/v1/data_key_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class data_key_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _master_key;
    std::vector<std::string> _key_alt_names;
    bsoncxx::v1::stdx::optional<key_material_type> _key_material;

    static impl const& with(data_key_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(data_key_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(data_key_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(data_key_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

data_key_options::~data_key_options() {
    delete impl::with(this);
}

data_key_options::data_key_options(data_key_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

data_key_options& data_key_options::operator=(data_key_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

data_key_options::data_key_options(data_key_options const& other) : _impl{new impl{impl::with(other)}} {}

data_key_options& data_key_options::operator=(data_key_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

data_key_options::data_key_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

data_key_options& data_key_options::master_key(bsoncxx::v1::document::value master_key) {
    impl::with(this)->_master_key = std::move(master_key);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> data_key_options::master_key() const {
    return impl::with(this)->_master_key;
}

data_key_options& data_key_options::key_alt_names(std::vector<std::string> key_alt_names) {
    impl::with(this)->_key_alt_names = std::move(key_alt_names);
    return *this;
}

std::vector<std::string> data_key_options::key_alt_names() const {
    return impl::with(this)->_key_alt_names;
}

data_key_options& data_key_options::key_material(key_material_type key_material) {
    impl::with(this)->_key_material = std::move(key_material);
    return *this;
}

bsoncxx::v1::stdx::optional<data_key_options::key_material_type> data_key_options::key_material() const {
    return impl::with(this)->_key_material;
}

std::unique_ptr<mongoc_client_encryption_datakey_opts_t, data_key_options::internal::deleter_type>
data_key_options::internal::to_mongoc(data_key_options const& self) {
    std::unique_ptr<mongoc_client_encryption_datakey_opts_t, data_key_options::internal::deleter_type> ret{
        libmongoc::client_encryption_datakey_opts_new()};

    auto const opts = ret.get();
    auto& _impl = impl::with(self);

    if (auto const& opt = _impl._master_key) {
        libmongoc::client_encryption_datakey_opts_set_masterkey(opts, scoped_bson_view{opt->view()}.bson());
    }

    if (!_impl._key_alt_names.empty()) {
        std::vector<char*> names;

        names.reserve(_impl._key_alt_names.size());
        std::transform(
            _impl._key_alt_names.begin(),
            _impl._key_alt_names.end(),
            std::back_inserter(names),
            [](std::string const& name) {
                // mongoc_client_encryption_datakey_opts_set_keyaltnames() deep-copies the elements of `keyaltnames`
                // without modification.
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                return const_cast<char*>(name.c_str());
            });

        libmongoc::client_encryption_datakey_opts_set_keyaltnames(
            opts, names.data(), static_cast<std::uint32_t>(_impl._key_alt_names.size()));
    }

    if (auto const& opt = _impl._key_material) {
        libmongoc::client_encryption_datakey_opts_set_keymaterial(
            opts, opt->data(), static_cast<std::uint32_t>(opt->size()));
    }

    return ret;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& data_key_options::internal::master_key(
    data_key_options& self) {
    return impl::with(self)._master_key;
}

std::vector<std::string>& data_key_options::internal::key_alt_names(data_key_options& self) {
    return impl::with(self)._key_alt_names;
}

bsoncxx::v1::stdx::optional<data_key_options::key_material_type>& data_key_options::internal::key_material(
    data_key_options& self) {
    return impl::with(self)._key_material;
}

} // namespace v1
} // namespace mongocxx
