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

#include <mongocxx/options/tls.hh>

//

#include <mongocxx/v1/tls.hh>

#include <utility>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/private/ssl.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

tls::tls(v1::tls v)
    : _pem_file{std::move(v1::tls::internal::pem_file(v))},
      _pem_password{std::move(v1::tls::internal::pem_password(v))},
      _ca_file{std::move(v1::tls::internal::ca_file(v))},
      _ca_dir{std::move(v1::tls::internal::ca_dir(v))},
      _crl_file{std::move(v1::tls::internal::crl_file(v))},
      _allow_invalid_certificates{v.allow_invalid_certificates()} {}

#if MONGOCXX_SSL_IS_ENABLED()
tls::internal::to_mongoc_type tls::internal::to_mongoc(tls const& opts) {
    to_mongoc_type ret = {};

    auto& opt = ret.opt;

    if (opts._pem_file) {
        opt.pem_file = ret.string_owner.emplace(ret.string_owner.end(), opts._pem_file->terminated())->data();
    }

    if (opts._pem_password) {
        opt.pem_pwd = ret.string_owner.emplace(ret.string_owner.end(), opts._pem_password->terminated())->data();
    }

    if (opts._ca_file) {
        opt.ca_file = ret.string_owner.emplace(ret.string_owner.end(), opts._ca_file->terminated())->data();
    }

    if (opts._ca_dir) {
        opt.ca_dir = ret.string_owner.emplace(ret.string_owner.end(), opts._ca_dir->terminated())->data();
    }

    if (opts._crl_file) {
        opt.crl_file = ret.string_owner.emplace(ret.string_owner.end(), opts._crl_file->terminated())->data();
    }

    opt.weak_cert_validation = opts._allow_invalid_certificates.value_or(false);

    return ret;
}
#endif

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
