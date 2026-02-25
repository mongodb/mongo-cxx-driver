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

#include <mongocxx/options/client.hh>

//

#include <mongocxx/v1/client.hh>

#include <utility>

#include <mongocxx/options/apm.hpp>
#include <mongocxx/options/server_api.hpp>
#include <mongocxx/options/tls.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

client::client(v1::client::options opts)
    : _tls_opts{std::move(v1::client::options::internal::tls_opts(opts))},
      _apm_opts{std::move(v1::client::options::internal::apm_opts(opts))},
      _auto_encrypt_opts{},
      _server_api_opts{std::move(v1::client::options::internal::server_api_opts(opts))} {}

client::operator v1::client::options() const {
    using mongocxx::v_noabi::from_v1;

    v1::client::options ret;

    if (_tls_opts) {
        ret.tls_opts(v_noabi::to_v1(*_tls_opts));
    }

    if (_apm_opts) {
        ret.apm_opts(v_noabi::to_v1(*_apm_opts));
    }

    // _auto_encrypt_opts

    if (_server_api_opts) {
        ret.server_api_opts(v_noabi::to_v1(*_server_api_opts));
    }

    return ret;
}

client client::internal::from_v1(v1::client::options v) {
    return {std::move(v)};
}

v1::client::options client::internal::to_v1(client const& v) {
    return v1::client::options{v};
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

v_noabi::options::client from_v1(v1::client::options v) {
    return v_noabi::options::client::internal::from_v1(std::move(v));
}

v1::client::options to_v1(v_noabi::options::client const& v) {
    return v_noabi::options::client::internal::to_v1(v);
}

} // namespace v_noabi
} // namespace mongocxx
