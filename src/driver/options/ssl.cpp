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

#include "driver/options/ssl.hpp"

namespace mongo {
namespace driver {
namespace options {

void ssl::pem_file(std::string pem_file) { _pem_file = std::move(pem_file); }

const optional<std::string>& ssl::pem_file() const { return _pem_file; }

void ssl::pem_password(std::string pem_password) { _pem_password = std::move(pem_password); }

const optional<std::string>& ssl::pem_password() const { return _pem_password; }

void ssl::ca_file(std::string ca_file) { _ca_file = std::move(ca_file); }

const optional<std::string>& ssl::ca_file() const { return _ca_file; }

void ssl::ca_dir(std::string ca_dir) { _ca_dir = std::move(ca_dir); }
const optional<std::string>& ssl::ca_dir() const { return _ca_dir; }

void ssl::weak_cert_validation(bool weak_cert_validation) {
    _weak_cert_validation = weak_cert_validation;
}

const optional<bool>& ssl::weak_cert_validation() const { return _weak_cert_validation; }

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
