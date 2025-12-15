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

#include <mongocxx/uri-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/detail/type_traits.hpp>

#include <mongocxx/v1/uri.hpp> // IWYU pragma: export

#include <cstdint>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <mongocxx/client-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/pool-fwd.hpp>   // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A MongoDB connection string URI.
///
/// @see
/// - [Connection Strings (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/connection-string/)
///
class uri {
   private:
    v1::uri _uri;

   public:
    using host = v1::uri::host;

    ///
    /// The default URI string: `"mongodb://localhost:27017"`.
    ///
    static MONGOCXX_ABI_EXPORT const std::string k_default_uri;

    ///
    /// Constructs a uri from the provided MongoDB URI string.
    ///
    /// @see
    /// - https://mongoc.org/libmongoc/current/mongoc_uri_t.html
    ///
    /// @{
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() uri(bsoncxx::v_noabi::string::view_or_value uri_string);

    template <
        typename T,
        bsoncxx::detail::enable_if_t<std::is_convertible<T, bsoncxx::v_noabi::string::view_or_value>::value>* = nullptr>
    /* explicit(false) */ uri(T t) : uri{bsoncxx::v_noabi::string::view_or_value{t}} {
        // For backward compatibility: avoid ambiguity with the new v1::uri ctor.
    }
    /// @}

    ///
    /// Constructs a uri from the default MongoDB URI string: `"mongodb://localhost:27017"`.
    ///
    /// @see
    /// - https://mongoc.org/libmongoc/current/mongoc_uri_t.html
    ///
    uri() : uri{bsoncxx::v_noabi::string::view_or_value{k_default_uri}} {}

    ///
    /// Move constructs a uri.
    ///
    uri(uri&& other) noexcept = default;

    ///
    /// Move assigns a uri.
    ///
    uri& operator=(uri&& other) noexcept = default;

    ///
    /// Destroys a uri.
    ///
    ~uri() = default;

    uri(uri const& other) = delete;
    uri& operator=(uri const& other) = delete;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ uri(v1::uri uri) : _uri{std::move(uri)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated views.
    ///
    explicit operator v1::uri() && {
        return std::move(_uri);
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::uri() const& {
        return _uri;
    }

    ///
    /// Returns the authentication mechanism from the uri.
    ///
    /// @return A string representing the authentication mechanism.
    ///
    std::string auth_mechanism() const {
        return std::string{_uri.auth_mechanism()};
    }

    ///
    /// Returns the authentication source from the uri.
    ///
    /// @return A string representing the authentication source.
    ///
    std::string auth_source() const {
        return std::string{_uri.auth_source()};
    }

    ///
    /// Returns the hosts from the uri.
    ///
    /// @return A vector of hosts.
    ///
    std::vector<host> hosts() const {
        return _uri.hosts();
    }

    ///
    /// Returns the database from the uri.
    ///
    /// @return A string with the name of the database.
    ///
    std::string database() const {
        return std::string{_uri.database()};
    }

    ///
    /// Returns other uri options.
    ///
    /// Note, options are returned in the case they were presented.
    /// The URI mongodb://localhost/?appName=abc will return { "appName": "abc" }
    /// The URI mongodb://localhost/?appname=abc will return { "appname": "abc" }
    ///
    /// @return A document view containing other options.
    ///
    bsoncxx::v_noabi::document::view options() const {
        return _uri.options();
    }

    ///
    /// Returns the password from the uri.
    ///
    /// @return A string containing the supplied password.
    ///
    std::string password() const {
        return std::string{_uri.password()};
    }

    ///
    /// Returns the read concern from the uri.
    ///
    /// @return A read_concern that represents what was specified in the uri.
    ///
    v_noabi::read_concern read_concern() const {
        return _uri.read_concern();
    }

    ///
    /// Returns the read preference from the uri.
    ///
    /// @return A read_preference that represents what was specified in the uri.
    ///
    v_noabi::read_preference read_preference() const {
        return _uri.read_preference();
    }

    ///
    /// Returns the replica set specified in the uri.
    ///
    /// @return A string representing the supplied replica set name.
    ///
    std::string replica_set() const {
        return std::string{_uri.replica_set()};
    }

    ///
    /// Returns the ssl parameter from the uri.
    ///
    /// @return Boolean that is @c true if ssl is enabled and @c false if not.
    ///
    /// @deprecated The tls() method should be used instead of this method.
    ///
    bool ssl() const {
        return _uri.tls();
    }

    ///
    /// Returns the tls parameter from the uri.
    ///
    /// @return Boolean that is @c true if tls is enabled and @c false if not.
    ///
    bool tls() const {
        return _uri.tls();
    }

    ///
    /// Returns the uri in a string format.
    ///
    /// @return A string with the uri.
    ///
    std::string to_string() const {
        return std::string{_uri.to_string()};
    }

    ///
    /// Returns the supplied username from the uri.
    ///
    /// @return A string with the username specified in the uri.
    ///
    std::string username() const {
        return std::string{_uri.username()};
    }

    ///
    /// Returns the write concern specified in the uri.
    ///
    /// @return A write_concern that represents what was specified in the uri.
    ///
    v_noabi::write_concern write_concern() const {
        return _uri.write_concern();
    }

    ///
    /// Returns the value of the option "appname" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view> appname() const {
        return _uri.appname();
    }

    ///
    /// Returns the value of the option "authMechanismProperties" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::document::view
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> auth_mechanism_properties() const {
        return _uri.auth_mechanism_properties();
    }

    ///
    /// Returns the value of the option credentials if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::document::view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>) credentials();

    ///
    /// Returns the value of the option "srvMaxHosts" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> srv_max_hosts() const {
        return _uri.srv_max_hosts();
    }

    ///
    /// Returns the list of compressors present in the uri or an empty list if "compressors" was not
    /// present or contained no valid compressors.
    ///
    /// @return A std::vector of bsoncxx::v_noabi::stdx::string_view.
    ///
    std::vector<bsoncxx::v_noabi::stdx::string_view> compressors() const {
        return _uri.compressors();
    }

    ///
    /// Returns the value of the option "connectTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> connect_timeout_ms() const {
        return _uri.connect_timeout_ms();
    }

    ///
    /// Returns the value of the option "directConnection" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> direct_connection() const {
        return _uri.direct_connection();
    }

    ///
    /// Returns the value of the option "heartbeatFrequencyMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> heartbeat_frequency_ms() const {
        return _uri.heartbeat_frequency_ms();
    }

    ///
    /// Returns the value of the option "localThresholdMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> local_threshold_ms() const {
        return _uri.local_threshold_ms();
    }

    ///
    /// Returns the value of the option "maxPoolSize" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> max_pool_size() const {
        return _uri.max_pool_size();
    }

    ///
    /// Returns the value of the option "retryReads" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> retry_reads() const {
        return _uri.retry_reads();
    }

    ///
    /// Returns the value of the option "retryWrites" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> retry_writes() const {
        return _uri.retry_writes();
    }

    ///
    /// Returns the value of the option "serverSelectionTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> server_selection_timeout_ms() const {
        return _uri.server_selection_timeout_ms();
    }

    ///
    /// Returns the value of the option "serverSelectionTryOnce" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> server_selection_try_once() const {
        return _uri.server_selection_try_once();
    }

    ///
    /// Sets the value of the option "serverSelectionTryOnce" in the uri.
    ///
    /// @param val The new value to apply to as "serverSelectionTryOnce".
    ///
    /// @throws mongocxx::v_noabi::exception if there is an error setting the option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) server_selection_try_once(bool val);

    ///
    /// Returns the value of the option "socketTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> socket_timeout_ms() const {
        return _uri.socket_timeout_ms();
    }

    ///
    /// Returns the value of the option "tlsAllowInvalidCertificates" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> tls_allow_invalid_certificates() const {
        return _uri.tls_allow_invalid_certificates();
    }

    ///
    /// Returns the value of the option "tlsAllowInvalidHostnames" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> tls_allow_invalid_hostnames() const {
        return _uri.tls_allow_invalid_hostnames();
    }

    ///
    /// Returns the value of the option "tlsCAFile" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view> tls_ca_file() const {
        return _uri.tls_ca_file();
    }

    ///
    /// Returns the value of the option "tlsCertificateKeyFile" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view> tls_certificate_key_file() const {
        return _uri.tls_certificate_key_file();
    }

    ///
    /// Returns the value of the option "tlsCertificateKeyFilePassword" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view> tls_certificate_key_file_password() const {
        return _uri.tls_certificate_key_file_password();
    }

    ///
    /// Returns the value of the option "tlsDisableCertificateRevocationCheck" if present in the
    /// uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> tls_disable_certificate_revocation_check() const {
        return _uri.tls_disable_certificate_revocation_check();
    }

    ///
    /// Returns the value of the option "tlsDisableOCSPEndpointCheck" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> tls_disable_ocsp_endpoint_check() const {
        return _uri.tls_disable_ocsp_endpoint_check();
    }

    ///
    /// Returns the value of the option "tlsInsecure" if present in the uri.
    ///
    /// @return An optional bool
    ///
    bsoncxx::v_noabi::stdx::optional<bool> tls_insecure() const {
        return _uri.tls_insecure();
    }

    ///
    /// Returns the value of the option "waitQueueTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> wait_queue_timeout_ms() const {
        return _uri.wait_queue_timeout_ms();
    }

    ///
    /// Returns the value of the option "zlibCompressionLevel" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> zlib_compression_level() const {
        return _uri.zlib_compression_level();
    }

    class internal;
};

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::uri from_v1(v1::uri v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::uri to_v1(v_noabi::uri v) {
    return v1::uri{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::uri.
///
/// @par Includes
/// - @ref mongocxx/v1/uri.hpp
///
