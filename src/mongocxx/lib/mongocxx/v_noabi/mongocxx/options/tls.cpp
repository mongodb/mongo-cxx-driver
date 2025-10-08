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

#include <mongocxx/options/tls.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

tls& tls::pem_file(bsoncxx::v_noabi::string::view_or_value pem_file) {
    _pem_file = std::move(pem_file);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& tls::pem_file() const {
    return _pem_file;
}

tls& tls::pem_password(bsoncxx::v_noabi::string::view_or_value pem_password) {
    _pem_password = std::move(pem_password);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& tls::pem_password() const {
    return _pem_password;
}

tls& tls::ca_file(bsoncxx::v_noabi::string::view_or_value ca_file) {
    _ca_file = std::move(ca_file);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& tls::ca_file() const {
    return _ca_file;
}

tls& tls::ca_dir(bsoncxx::v_noabi::string::view_or_value ca_dir) {
    _ca_dir = std::move(ca_dir);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& tls::ca_dir() const {
    return _ca_dir;
}

tls& tls::crl_file(bsoncxx::v_noabi::string::view_or_value crl_file) {
    _crl_file = std::move(crl_file);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& tls::crl_file() const {
    return _crl_file;
}

tls& tls::allow_invalid_certificates(bool allow_invalid_certificates) {
    _allow_invalid_certificates = allow_invalid_certificates;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bool> const& tls::allow_invalid_certificates() const {
    return _allow_invalid_certificates;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
