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

#include <string>

#include <mongocxx/options/client-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/options/apm.hpp>
#include <mongocxx/options/auto_encryption.hpp>
#include <mongocxx/options/server_api.hpp>
#include <mongocxx/options/tls.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by clients.
///
class client {
   public:
    ///
    /// Sets the SSL-related options.
    ///
    /// @param ssl_opts
    ///   The SSL-related options.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This
    ///   facilitates method chaining.
    ///
    /// @deprecated
    ///   Please use tls_opts instead.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(client&) ssl_opts(tls ssl_opts);

    ///
    /// Sets the TLS-related options.
    ///
    /// @param tls_opts
    ///   The TLS-related options.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) tls_opts(tls tls_opts);

    ///
    /// The current SSL-related options.
    ///
    /// @return The SSL-related options.
    ///
    /// @deprecated Please use tls_opts instead.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<tls> const&) ssl_opts() const;

    ///
    /// The current TLS-related options.
    ///
    /// @return The TLS-related options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<tls> const&) tls_opts() const;

    ///
    /// Sets the automatic encryption options.
    ///
    /// @param auto_encryption_opts
    ///   The options for automatic encryption.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) auto_encryption_opts(auto_encryption auto_encryption_opts);

    ///
    /// Gets the current automatic encryption options.
    ///
    /// @return
    ///   The automatic encryption opts.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<auto_encryption> const&)
    auto_encryption_opts() const;

    ///
    /// Sets the APM-related options.
    ///
    /// @param apm_opts
    ///   The APM-related options.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) apm_opts(apm apm_opts);

    ///
    /// The current APM-related options.
    ///
    /// @return The APM-related options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<apm> const&) apm_opts() const;

    ///
    /// Sets the server API options.
    ///
    /// @param server_api_opts
    ///   The options for server API.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) server_api_opts(server_api server_api_opts);

    ///
    /// Gets the current server API options or returns a disengaged optional if there are no server
    /// API options set.
    ///
    /// @return
    ///   The server API options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<server_api> const&)
    server_api_opts() const;

   private:
    bsoncxx::v_noabi::stdx::optional<tls> _tls_opts;
    bsoncxx::v_noabi::stdx::optional<apm> _apm_opts;
    bsoncxx::v_noabi::stdx::optional<auto_encryption> _auto_encrypt_opts;
    bsoncxx::v_noabi::stdx::optional<server_api> _server_api_opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::client.
///
