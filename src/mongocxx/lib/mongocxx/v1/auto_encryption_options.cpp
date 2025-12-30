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

#include <mongocxx/v1/auto_encryption_options.hh>

//

#include <mongocxx/v1/client-fwd.hpp>
#include <mongocxx/v1/pool-fwd.hpp>

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class auto_encryption_options::impl {
   public:
    v1::client* _key_vault_client = nullptr;
    v1::pool* _key_vault_pool = nullptr;
    bsoncxx::v1::stdx::optional<ns_pair> _key_vault_namespace;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _kms_providers;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _tls_opts;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _schema_map;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _encrypted_fields_map;
    bool _bypass_auto_encryption = false;
    bool _bypass_query_analysis = false;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _extra_options;

    static impl const& with(auto_encryption_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(auto_encryption_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(auto_encryption_options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(auto_encryption_options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

auto_encryption_options::~auto_encryption_options() {
    delete impl::with(_impl);
}

auto_encryption_options::auto_encryption_options(auto_encryption_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

auto_encryption_options& auto_encryption_options::operator=(auto_encryption_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

auto_encryption_options::auto_encryption_options(auto_encryption_options const& other)
    : _impl{new impl{impl::with(other)}} {}

auto_encryption_options& auto_encryption_options::operator=(auto_encryption_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

auto_encryption_options::auto_encryption_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

auto_encryption_options& auto_encryption_options::key_vault_client(v1::client* v) {
    impl::with(this)->_key_vault_client = v;
    return *this;
}

v1::client* auto_encryption_options::key_vault_client() const {
    return impl::with(this)->_key_vault_client;
}

auto_encryption_options& auto_encryption_options::key_vault_pool(v1::pool* v) {
    impl::with(this)->_key_vault_pool = v;
    return *this;
}

v1::pool* auto_encryption_options::key_vault_pool() const {
    return impl::with(this)->_key_vault_pool;
}

auto_encryption_options& auto_encryption_options::key_vault_namespace(ns_pair v) {
    impl::with(this)->_key_vault_namespace = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<auto_encryption_options::ns_pair> auto_encryption_options::key_vault_namespace() const {
    return impl::with(this)->_key_vault_namespace;
}

auto_encryption_options& auto_encryption_options::kms_providers(bsoncxx::v1::document::value v) {
    impl::with(this)->_kms_providers = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> auto_encryption_options::kms_providers() const {
    return impl::with(this)->_kms_providers;
}

auto_encryption_options& auto_encryption_options::tls_opts(bsoncxx::v1::document::value v) {
    impl::with(this)->_tls_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> auto_encryption_options::tls_opts() const {
    return impl::with(this)->_tls_opts;
}

auto_encryption_options& auto_encryption_options::schema_map(bsoncxx::v1::document::value v) {
    impl::with(this)->_schema_map = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> auto_encryption_options::schema_map() const {
    return impl::with(this)->_schema_map;
}

auto_encryption_options& auto_encryption_options::encrypted_fields_map(bsoncxx::v1::document::value v) {
    impl::with(this)->_encrypted_fields_map = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> auto_encryption_options::encrypted_fields_map() const {
    return impl::with(this)->_encrypted_fields_map;
}

auto_encryption_options& auto_encryption_options::bypass_auto_encryption(bool v) {
    impl::with(this)->_bypass_auto_encryption = v;
    return *this;
}

bool auto_encryption_options::bypass_auto_encryption() const {
    return impl::with(this)->_bypass_auto_encryption;
}

auto_encryption_options& auto_encryption_options::bypass_query_analysis(bool v) {
    impl::with(this)->_bypass_query_analysis = v;
    return *this;
}

bool auto_encryption_options::bypass_query_analysis() const {
    return impl::with(this)->_bypass_query_analysis;
}

auto_encryption_options& auto_encryption_options::extra_options(bsoncxx::v1::document::value v) {
    impl::with(this)->_extra_options = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> auto_encryption_options::extra_options() const {
    return impl::with(this)->_extra_options;
}

bsoncxx::v1::stdx::optional<auto_encryption_options::ns_pair>& auto_encryption_options::internal::key_vault_namespace(
    auto_encryption_options& self) {
    return impl::with(self)._key_vault_namespace;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& auto_encryption_options::internal::kms_providers(
    auto_encryption_options& self) {
    return impl::with(self)._kms_providers;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& auto_encryption_options::internal::tls_opts(
    auto_encryption_options& self) {
    return impl::with(self)._tls_opts;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& auto_encryption_options::internal::schema_map(
    auto_encryption_options& self) {
    return impl::with(self)._schema_map;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& auto_encryption_options::internal::encrypted_fields_map(
    auto_encryption_options& self) {
    return impl::with(self)._encrypted_fields_map;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& auto_encryption_options::internal::extra_options(
    auto_encryption_options& self) {
    return impl::with(self)._extra_options;
}

} // namespace v1
} // namespace mongocxx
