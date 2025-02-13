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

#pragma once

#include <mongocxx/options/tls.hpp>

//

#include <list>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/ssl.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

#if MONGOCXX_SSL_IS_ENABLED()
inline std::pair<::mongoc_ssl_opt_t, std::list<bsoncxx::v_noabi::string::view_or_value>> make_tls_opts(
    tls const& tls_opts) {
    ::mongoc_ssl_opt_t out{};
    std::list<bsoncxx::v_noabi::string::view_or_value> values;

    if (tls_opts.pem_file()) {
        out.pem_file = values.emplace(values.end(), tls_opts.pem_file()->terminated())->data();
    }
    if (tls_opts.pem_password()) {
        out.pem_pwd = values.emplace(values.end(), tls_opts.pem_password()->terminated())->data();
    }
    if (tls_opts.ca_file()) {
        out.ca_file = values.emplace(values.end(), tls_opts.ca_file()->terminated())->data();
    }
    if (tls_opts.ca_dir()) {
        out.ca_dir = values.emplace(values.end(), tls_opts.ca_dir()->terminated())->data();
    }
    if (tls_opts.crl_file()) {
        out.crl_file = values.emplace(values.end(), tls_opts.crl_file()->terminated())->data();
    }
    if (tls_opts.allow_invalid_certificates()) {
        out.weak_cert_validation = *(tls_opts.allow_invalid_certificates());
    }
    return {out, std::move(values)};
}
#endif

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
