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

#include <mongocxx/options/client_encryption.hh>

//

#include <mongocxx/v1/client_encryption.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <mongocxx/client.hh>
#include <mongocxx/scoped_bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

client_encryption::client_encryption(v1::client_encryption::options opts)
    : _key_vault_client{},
      _key_vault_namespace{std::move(v1::client_encryption::options::internal::key_vault_namespace(opts))},
      _kms_providers{[&]() -> decltype(_kms_providers) {
          if (auto& opt = v1::client_encryption::options::internal::kms_providers(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _tls_opts{[&]() -> decltype(_tls_opts) {
          if (auto& opt = v1::client_encryption::options::internal::tls_opts(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()} {}

client_encryption::operator v1::client_encryption::options() const {
    using bsoncxx::v_noabi::to_v1;

    v1::client_encryption::options ret;

    // _key_vault_client

    if (_key_vault_namespace) {
        ret.key_vault_namespace(*_key_vault_namespace);
    }
    if (_kms_providers) {
        ret.kms_providers(bsoncxx::v1::document::value{to_v1(*_kms_providers)});
    }
    if (_tls_opts) {
        ret.tls_opts(bsoncxx::v1::document::value{to_v1(*_tls_opts)});
    }

    return ret;
}

client_encryption client_encryption::internal::from_v1(v1::client_encryption::options v) {
    return {std::move(v)};
}

v1::client_encryption::options client_encryption::internal::to_v1(client_encryption const& v) {
    return v1::client_encryption::options{v};
}

client_encryption::internal::opts_ptr_type client_encryption::internal::to_mongoc(client_encryption const& opts) {
    auto ret = opts_ptr_type{libmongoc::client_encryption_opts_new(), opts_deleter{}};
    auto const ptr = ret.get();

    if (auto const opt = opts.key_vault_client()) {
        libmongoc::client_encryption_opts_set_keyvault_client(ptr, v_noabi::client::internal::as_mongoc(**opt));
    }

    if (auto const& opt = opts.key_vault_namespace()) {
        auto const& ns = *opt;
        libmongoc::client_encryption_opts_set_keyvault_namespace(ptr, ns.first.c_str(), ns.second.c_str());
    }

    if (auto const& opt = opts.kms_providers()) {
        libmongoc::client_encryption_opts_set_kms_providers(ptr, to_scoped_bson_view(*opt).bson());
    }

    if (auto const& opt = opts.tls_opts()) {
        libmongoc::client_encryption_opts_set_tls_opts(ptr, to_scoped_bson_view(*opt).bson());
    }

    return ret;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

v_noabi::options::client_encryption from_v1(v1::client_encryption::options v) {
    return v_noabi::options::client_encryption::internal::from_v1(std::move(v));
}

v1::client_encryption::options to_v1(v_noabi::options::client_encryption const& v) {
    return v_noabi::options::client_encryption::internal::to_v1(v);
}

} // namespace v_noabi
} // namespace mongocxx
