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

#pragma once

#include <mongo/driver/config/prelude.hpp>

#include <string>

#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB driver client (SSL)
///
class LIBMONGOCXX_API ssl {

   public:
    void pem_file(std::string pem_file);
    const stdx::optional<std::string>& pem_file() const;

    void pem_password(std::string pem_password);
    const stdx::optional<std::string>& pem_password() const;

    void ca_file(std::string ca_file);
    const stdx::optional<std::string>& ca_file() const;

    void ca_dir(std::string ca_file);
    const stdx::optional<std::string>& ca_dir() const;

    void weak_cert_validation(bool weak_cert_validation);
    const stdx::optional<bool>& weak_cert_validation() const;

   private:
    stdx::optional<std::string> _pem_file;
    stdx::optional<std::string> _pem_password;
    stdx::optional<std::string> _ca_file;
    stdx::optional<std::string> _ca_dir;
    stdx::optional<bool> _weak_cert_validation;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
