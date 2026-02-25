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

#include <mongocxx/options/client-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/client.hpp> // IWYU pragma: export

#include <string> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

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
    /// Default initialization.
    ///
    client() = default;

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
    MONGOCXX_DEPRECATED client& ssl_opts(tls ssl_opts) {
        return this->tls_opts(std::move(ssl_opts));
    }

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
    client& tls_opts(tls tls_opts) {
        _tls_opts = std::move(tls_opts);
        return *this;
    }

    ///
    /// The current SSL-related options.
    ///
    /// @return The SSL-related options.
    ///
    /// @deprecated Please use tls_opts instead.
    ///
    MONGOCXX_DEPRECATED bsoncxx::v_noabi::stdx::optional<tls> const& ssl_opts() const {
        return this->tls_opts();
    }

    ///
    /// The current TLS-related options.
    ///
    /// @return The TLS-related options.
    ///
    bsoncxx::v_noabi::stdx::optional<tls> const& tls_opts() const {
        return _tls_opts;
    }

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
    client& auto_encryption_opts(auto_encryption auto_encryption_opts) {
        _auto_encrypt_opts = std::move(auto_encryption_opts);
        return *this;
    }

    ///
    /// Gets the current automatic encryption options.
    ///
    /// @return
    ///   The automatic encryption opts.
    ///
    bsoncxx::v_noabi::stdx::optional<auto_encryption> const& auto_encryption_opts() const {
        return _auto_encrypt_opts;
    }

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
    client& apm_opts(apm apm_opts) {
        _apm_opts = std::move(apm_opts);
        return *this;
    }

    ///
    /// The current APM-related options.
    ///
    /// @return The APM-related options.
    ///
    bsoncxx::v_noabi::stdx::optional<apm> const& apm_opts() const {
        return _apm_opts;
    }

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
    client& server_api_opts(server_api server_api_opts) {
        _server_api_opts = std::move(server_api_opts);
        return *this;
    }

    ///
    /// Gets the current server API options or returns a disengaged optional if there are no server
    /// API options set.
    ///
    /// @return
    ///   The server API options.
    ///
    bsoncxx::v_noabi::stdx::optional<server_api> const& server_api_opts() const {
        return _server_api_opts;
    }

    class internal;

   private:
    bsoncxx::v_noabi::stdx::optional<tls> _tls_opts;
    bsoncxx::v_noabi::stdx::optional<apm> _apm_opts;
    bsoncxx::v_noabi::stdx::optional<auto_encryption> _auto_encrypt_opts;
    bsoncxx::v_noabi::stdx::optional<server_api> _server_api_opts;

    /* explicit(false) */ client(v1::client::options opts);

    explicit operator v1::client::options() const;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
/// @important The `auto_encryption_opts` field in the resulting object is unset when not explicitly provided as an
/// argument to this conversion function.
///
/// @{
MONGOCXX_ABI_EXPORT_CDECL(v_noabi::options::client) from_v1(v1::client::options v);

inline v_noabi::options::client from_v1(v1::client::options v, v_noabi::options::auto_encryption opts) {
    auto ret = from_v1(std::move(v));
    ret.auto_encryption_opts(std::move(opts));
    return ret;
}
/// @}
///

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
/// @important The `auto_encryption_opts` field in the resulting object is unset when not explicitly provided as an
/// argument to this conversion function.
///
/// @{
MONGOCXX_ABI_EXPORT_CDECL(v1::client::options) to_v1(v_noabi::options::client const& v);

inline v1::client::options to_v1(v_noabi::options::client const& v, v1::auto_encryption_options opts) {
    auto ret = to_v1(v);
    ret.auto_encryption_opts(std::move(opts));
    return ret;
}
/// @}
///

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::client.
///
/// @par Includes
/// - @ref mongocxx/v1/client.hpp
///
