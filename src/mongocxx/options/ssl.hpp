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

#include <mongocxx/config/prelude.hpp>

#include <string>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a MongoDB driver client (SSL)
///
class MONGOCXX_API ssl {
   public:
    ///
    /// The path to the .pem file containing a public key certificate and its associated private
    /// key.
    ///
    /// @param pem_file
    ///   The path to the .pem file.
    ///
    void pem_file(std::string pem_file);

    ///
    /// Retrieves the current path to the .pem file.
    ///
    /// @return The path to the .pem file.
    ///
    const stdx::optional<std::string>& pem_file() const;

    ///
    /// The pass phrase used to decrypt an encrypted PEM file.
    ///
    /// @param pem_password
    ///   The pass phrase.
    ///
    void pem_password(std::string pem_password);

    ///
    /// Retrieves the current decryption pass phrase.
    ///
    /// @return The pass phrase.
    ///
    const stdx::optional<std::string>& pem_password() const;

    ///
    /// The path to the .pem file that contains the root certificate chain from the Certificate
    /// Authority.
    ///
    /// @param ca_file
    ///   The path to the CA file.
    ///
    void ca_file(std::string ca_file);

    ///
    /// Retrieves the current path to the CA file.
    ///
    /// @return The path to the CA file.
    ///
    const stdx::optional<std::string>& ca_file() const;

    ///
    /// The path to the Certificate Authority directory.
    ///
    /// @param ca_dir
    ///   The path to the CA directory.
    ///
    void ca_dir(std::string ca_dir);

    ///
    /// Retrieves the current path to the CA directory.
    ///
    /// @return The path to the CA directory.
    ///
    const stdx::optional<std::string>& ca_dir() const;

    ///
    /// The path to the .pem file that contains revoked certificates.
    ///
    /// @param crl_file
    ///   The path to the PEM file.
    ///
    void crl_file(std::string crl_file);

    ///
    /// Retrieves the current path to the .pem file that contains revoked certificates.
    ///
    /// @return The path to the revoked certificates file.
    ///
    const stdx::optional<std::string>& crl_file() const;

    ///
    /// If false, the driver will not verify the server's CA file.
    ///
    /// @param allow_invalid_certificates
    ///   Whether or not to check the server's CA file.
    ///
    void allow_invalid_certificates(bool allow_invalid_certificates);

    ///
    /// Retrieves whether or not the driver will check the server's CA file.
    ///
    /// @return Whether or not the driver will check the server's CA file.
    ///
    const stdx::optional<bool>& allow_invalid_certificates() const;

   private:
    stdx::optional<std::string> _pem_file;
    stdx::optional<std::string> _pem_password;
    stdx::optional<std::string> _ca_file;
    stdx::optional<std::string> _ca_dir;
    stdx::optional<std::string> _crl_file;
    stdx::optional<bool> _allow_invalid_certificates;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
