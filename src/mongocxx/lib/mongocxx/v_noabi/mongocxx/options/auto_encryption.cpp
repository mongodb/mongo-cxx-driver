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

#include <mongocxx/options/auto_encryption.hh>

//

#include <mongocxx/v1/auto_encryption_options.hh>

#include <stdexcept>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/pool.hpp>

#include <mongocxx/client.hh>
#include <mongocxx/pool.hh>
#include <mongocxx/scoped_bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

auto_encryption::auto_encryption(v1::auto_encryption_options opts)
    : _bypass{opts.bypass_auto_encryption()},
      _bypass_query_analysis{opts.bypass_query_analysis()},
      _key_vault_client{},
      _key_vault_pool{},
      _key_vault_namespace{std::move(v1::auto_encryption_options::internal::key_vault_namespace(opts))},
      _kms_providers{[&]() -> decltype(_kms_providers) {
          if (auto& opt = v1::auto_encryption_options::internal::kms_providers(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _tls_opts{[&]() -> decltype(_tls_opts) {
          if (auto& opt = v1::auto_encryption_options::internal::tls_opts(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _schema_map{[&]() -> decltype(_schema_map) {
          if (auto& opt = v1::auto_encryption_options::internal::schema_map(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _encrypted_fields_map{[&]() -> decltype(_encrypted_fields_map) {
          if (auto& opt = v1::auto_encryption_options::internal::encrypted_fields_map(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _extra_options{[&]() -> decltype(_extra_options) {
          if (auto& opt = v1::auto_encryption_options::internal::extra_options(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()} {}

auto_encryption::operator v1::auto_encryption_options() const {
    using bsoncxx::v_noabi::from_v1;

    v1::auto_encryption_options ret;

    ret.bypass_auto_encryption(_bypass);
    ret.bypass_query_analysis(_bypass_query_analysis);

    // _key_vault_client
    // _key_vault_pool

    if (_key_vault_namespace) {
        ret.key_vault_namespace(*_key_vault_namespace);
    }

    if (_kms_providers) {
        ret.kms_providers(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_kms_providers->view())});
    }

    if (_tls_opts) {
        ret.tls_opts(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_tls_opts->view())});
    }

    if (_schema_map) {
        ret.schema_map(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_schema_map->view())});
    }

    if (_encrypted_fields_map) {
        ret.encrypted_fields_map(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_encrypted_fields_map->view())});
    }

    if (_extra_options) {
        ret.extra_options(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_extra_options->view())});
    }

    return ret;
}

auto_encryption auto_encryption::internal::from_v1(v1::auto_encryption_options v) {
    return {std::move(v)};
}

v1::auto_encryption_options auto_encryption::internal::to_v1(auto_encryption const& v) {
    return v1::auto_encryption_options{v};
}

mongoc_auto_encryption_opts_t* auto_encryption::internal::to_mongoc(auto_encryption const& opts) {
    if (opts.key_vault_client() && opts.key_vault_pool()) {
        throw v_noabi::exception{
            error_code::k_invalid_parameter, "cannot set both key vault client and key vault pool, please choose one"};
    }

    auto const ret = libmongoc::auto_encryption_opts_new();

    if (!ret) {
        throw std::logic_error{"could not get object from libmongoc"};
    }

    if (auto const& opt = opts.key_vault_client()) {
        mongoc_client_t* client_t = (*opt)->_get_impl().client_t;
        libmongoc::auto_encryption_opts_set_keyvault_client(ret, client_t);
    }

    if (auto const& opt = opts.key_vault_pool()) {
        mongoc_client_pool_t* pool_t = (*opt)->_impl->client_pool_t;
        libmongoc::auto_encryption_opts_set_keyvault_client_pool(ret, pool_t);
    }

    if (auto const& opt = opts.key_vault_namespace()) {
        auto const& ns = *opt;
        libmongoc::auto_encryption_opts_set_keyvault_namespace(ret, ns.first.c_str(), ns.second.c_str());
    }

    if (auto const& opt = opts.kms_providers()) {
        libmongoc::auto_encryption_opts_set_kms_providers(ret, to_scoped_bson_view(*opt));
    }

    if (auto const& opt = opts.tls_opts()) {
        libmongoc::auto_encryption_opts_set_tls_opts(ret, to_scoped_bson_view(*opt));
    }

    if (auto const& opt = opts.schema_map()) {
        libmongoc::auto_encryption_opts_set_schema_map(ret, to_scoped_bson_view(*opt));
    }

    if (auto const& opt = opts.encrypted_fields_map()) {
        libmongoc::auto_encryption_opts_set_encrypted_fields_map(ret, to_scoped_bson_view(*opt));
    }

    libmongoc::auto_encryption_opts_set_bypass_auto_encryption(ret, opts.bypass_auto_encryption());
    libmongoc::auto_encryption_opts_set_bypass_query_analysis(ret, opts.bypass_query_analysis());

    if (auto const& opt = opts.extra_options()) {
        libmongoc::auto_encryption_opts_set_extra(ret, to_scoped_bson_view(*opt));
    }

    return ret;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

v_noabi::options::auto_encryption from_v1(v1::auto_encryption_options v) {
    return v_noabi::options::auto_encryption::internal::from_v1(std::move(v));
}

v1::auto_encryption_options to_v1(v_noabi::options::auto_encryption const& v) {
    return v_noabi::options::auto_encryption::internal::to_v1(v);
}

} // namespace v_noabi
} // namespace mongocxx
