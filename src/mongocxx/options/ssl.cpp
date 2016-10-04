// Copyright 2014 MongoDB Inc.
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

#include <mongocxx/options/ssl.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

ssl& ssl::pem_file(bsoncxx::string::view_or_value pem_file) {
    _pem_file = std::move(pem_file);
    return *this;
}

const stdx::optional<bsoncxx::string::view_or_value>& ssl::pem_file() const {
    return _pem_file;
}

ssl& ssl::pem_password(bsoncxx::string::view_or_value pem_password) {
    _pem_password = std::move(pem_password);
    return *this;
}

const stdx::optional<bsoncxx::string::view_or_value>& ssl::pem_password() const {
    return _pem_password;
}

ssl& ssl::ca_file(bsoncxx::string::view_or_value ca_file) {
    _ca_file = std::move(ca_file);
    return *this;
}

const stdx::optional<bsoncxx::string::view_or_value>& ssl::ca_file() const {
    return _ca_file;
}

ssl& ssl::ca_dir(bsoncxx::string::view_or_value ca_dir) {
    _ca_dir = std::move(ca_dir);
    return *this;
}

const stdx::optional<bsoncxx::string::view_or_value>& ssl::ca_dir() const {
    return _ca_dir;
}

ssl& ssl::crl_file(bsoncxx::string::view_or_value crl_file) {
    _crl_file = std::move(crl_file);
    return *this;
}

const stdx::optional<bsoncxx::string::view_or_value>& ssl::crl_file() const {
    return _crl_file;
}

ssl& ssl::allow_invalid_certificates(bool allow_invalid_certificates) {
    _allow_invalid_certificates = allow_invalid_certificates;
    return *this;
}

const stdx::optional<bool>& ssl::allow_invalid_certificates() const {
    return _allow_invalid_certificates;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
