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

#include <mongocxx/options/tls-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/tls.hpp> // IWYU pragma: export

#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <string>
#include <utility>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::client::tls_opts.
///
class tls {
   public:
    ///
    /// Default initialization.
    ///
    tls() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() tls(v1::tls v);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::tls() const {
        v1::tls ret;

        if (_pem_file) {
            ret.pem_file(std::string{_pem_file->view()});
        }

        if (_pem_password) {
            ret.pem_password(std::string{_pem_password->view()});
        }

        if (_ca_file) {
            ret.ca_file(std::string{_ca_file->view()});
        }

        if (_ca_dir) {
            ret.ca_dir(std::string{_ca_dir->view()});
        }

        if (_crl_file) {
            ret.crl_file(std::string{_crl_file->view()});
        }

        if (_allow_invalid_certificates) {
            ret.allow_invalid_certificates(*_allow_invalid_certificates);
        }

        return ret;
    }

    ///
    /// The path to the .pem file containing a public key certificate and its associated private
    /// key.
    ///
    /// @param pem_file
    ///   The path to the .pem file.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) pem_file(bsoncxx::v_noabi::string::view_or_value pem_file);

    ///
    /// Retrieves the current path to the .pem file.
    ///
    /// @return The path to the .pem file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const&)
    pem_file() const;

    ///
    /// The pass phrase used to decrypt an encrypted PEM file.
    ///
    /// @param pem_password
    ///   The pass phrase.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&)
    pem_password(bsoncxx::v_noabi::string::view_or_value pem_password);

    ///
    /// Retrieves the current decryption pass phrase.
    ///
    /// @return The pass phrase.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const&)
    pem_password() const;

    ///
    /// The path to the .pem file that contains the root certificate chain from the Certificate
    /// Authority.
    ///
    /// @param ca_file
    ///   The path to the CA file.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) ca_file(bsoncxx::v_noabi::string::view_or_value ca_file);

    ///
    /// Retrieves the current path to the CA file.
    ///
    /// @return The path to the CA file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const&)
    ca_file() const;

    ///
    /// The path to the Certificate Authority directory.
    ///
    /// @param ca_dir
    ///   The path to the CA directory.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) ca_dir(bsoncxx::v_noabi::string::view_or_value ca_dir);

    ///
    /// Retrieves the current path to the CA directory.
    ///
    /// @return The path to the CA directory.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const&)
    ca_dir() const;

    ///
    /// The path to the .pem file that contains revoked certificates.
    ///
    /// @param crl_file
    ///   The path to the PEM file.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) crl_file(bsoncxx::v_noabi::string::view_or_value crl_file);

    ///
    /// Retrieves the current path to the .pem file that contains revoked certificates.
    ///
    /// @return The path to the revoked certificates file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const&)
    crl_file() const;

    ///
    /// If true, the driver will not verify the server's CA file.
    ///
    /// @param allow_invalid_certificates
    ///   Whether or not to check the server's CA file.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) allow_invalid_certificates(bool allow_invalid_certificates);

    ///
    /// Retrieves whether or not the driver will check the server's CA file.
    ///
    /// @return Whether or not the driver will check the server's CA file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool> const&)
    allow_invalid_certificates() const;

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _pem_file;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _pem_password;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _ca_file;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _ca_dir;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _crl_file;
    bsoncxx::v_noabi::stdx::optional<bool> _allow_invalid_certificates;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::tls from_v1(v1::tls v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::tls to_v1(v_noabi::options::tls v) {
    return v1::tls{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::tls.
///
/// @par Includes
/// - @ref mongocxx/v1/tls.hpp
///
