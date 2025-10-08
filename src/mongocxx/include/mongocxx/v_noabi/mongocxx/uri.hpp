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

#include <memory>
#include <string>
#include <vector>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/pool-fwd.hpp>
#include <mongocxx/uri-fwd.hpp>

#include <bsoncxx/document/view.hpp>
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
   public:
    ///
    /// A host.
    ///
    struct host {
        std::string name;    ///< The host name.
        std::uint16_t port;  ///< The port number.
        std::int32_t family; ///< The address family.
    };

    ///
    /// The default URI string: `"mongodb://localhost:27017"`.
    ///
    static MONGOCXX_ABI_EXPORT const std::string k_default_uri;

    ///
    /// Constructs a uri from an optional MongoDB URI string. If no URI string is specified,
    /// uses the default URI string: `"mongodb://localhost:27017"`.
    ///
    /// @see
    /// - https://mongoc.org/libmongoc/current/mongoc_uri_t.html
    ///
    /// @param uri_string
    ///   String representing a MongoDB connection string URI, defaults to k_default_uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    uri(bsoncxx::v_noabi::string::view_or_value uri_string = k_default_uri);

    ///
    /// Move constructs a uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() uri(uri&&) noexcept;

    ///
    /// Move assigns a uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(uri&) operator=(uri&&) noexcept;

    ///
    /// Destroys a uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~uri();

    uri(uri const&) = delete;
    uri& operator=(uri const&) = delete;

    ///
    /// Returns the authentication mechanism from the uri.
    ///
    /// @return A string representing the authentication mechanism.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) auth_mechanism() const;

    ///
    /// Returns the authentication source from the uri.
    ///
    /// @return A string representing the authentication source.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) auth_source() const;

    ///
    /// Returns the hosts from the uri.
    ///
    /// @return A vector of hosts.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<host>) hosts() const;

    ///
    /// Returns the database from the uri.
    ///
    /// @return A string with the name of the database.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) database() const;

    ///
    /// Returns other uri options.
    ///
    /// Note, options are returned in the case they were presented.
    /// The URI mongodb://localhost/?appName=abc will return { "appName": "abc" }
    /// The URI mongodb://localhost/?appname=abc will return { "appname": "abc" }
    ///
    /// @return A document view containing other options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) options() const;

    ///
    /// Returns the password from the uri.
    ///
    /// @return A string containing the supplied password.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) password() const;

    ///
    /// Returns the read concern from the uri.
    ///
    /// @return A read_concern that represents what was specified in the uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::read_concern) read_concern() const;

    ///
    /// Returns the read preference from the uri.
    ///
    /// @return A read_preference that represents what was specified in the uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::read_preference) read_preference() const;

    ///
    /// Returns the replica set specified in the uri.
    ///
    /// @return A string representing the supplied replica set name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) replica_set() const;

    ///
    /// Returns the ssl parameter from the uri.
    ///
    /// @return Boolean that is @c true if ssl is enabled and @c false if not.
    ///
    /// @deprecated The tls() method should be used instead of this method.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bool) ssl() const;

    ///
    /// Returns the tls parameter from the uri.
    ///
    /// @return Boolean that is @c true if tls is enabled and @c false if not.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) tls() const;

    ///
    /// Returns the uri in a string format.
    ///
    /// @return A string with the uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) to_string() const;

    ///
    /// Returns the supplied username from the uri.
    ///
    /// @return A string with the username specified in the uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::string) username() const;

    ///
    /// Returns the write concern specified in the uri.
    ///
    /// @return A write_concern that represents what was specified in the uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(mongocxx::v_noabi::write_concern) write_concern() const;

    ///
    /// Returns the value of the option "appname" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view>)
    appname() const;

    ///
    /// Returns the value of the option "authMechanismProperties" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::document::view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>)
    auth_mechanism_properties() const;

    ///
    /// Returns the value of the option credentials if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::document::view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>)
    credentials();

    ///
    /// Returns the value of the option "srvMaxHosts" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>) srv_max_hosts() const;

    ///
    /// Returns the list of compressors present in the uri or an empty list if "compressors" was not
    /// present or contained no valid compressors.
    ///
    /// @return A std::vector of bsoncxx::v_noabi::stdx::string_view.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<bsoncxx::v_noabi::stdx::string_view>) compressors() const;

    ///
    /// Returns the value of the option "connectTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    connect_timeout_ms() const;

    ///
    /// Returns the value of the option "directConnection" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>) direct_connection() const;

    ///
    /// Returns the value of the option "heartbeatFrequencyMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    heartbeat_frequency_ms() const;

    ///
    /// Returns the value of the option "localThresholdMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    local_threshold_ms() const;

    ///
    /// Returns the value of the option "maxPoolSize" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>) max_pool_size() const;

    ///
    /// Returns the value of the option "retryReads" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>) retry_reads() const;

    ///
    /// Returns the value of the option "retryWrites" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>) retry_writes() const;

    ///
    /// Returns the value of the option "serverSelectionTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    server_selection_timeout_ms() const;

    ///
    /// Returns the value of the option "serverSelectionTryOnce" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>)
    server_selection_try_once() const;

    ///
    /// Sets the value of the option "serverSelectionTryOnce" in the uri.
    ///
    /// @param val The new value to apply to as "serverSelectionTryOnce".
    ///
    /// @throws mongocxx::v_noabi::exception if there is an error setting the option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    server_selection_try_once(bool val);

    ///
    /// Returns the value of the option "socketTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    socket_timeout_ms() const;

    ///
    /// Returns the value of the option "tlsAllowInvalidCertificates" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>)
    tls_allow_invalid_certificates() const;

    ///
    /// Returns the value of the option "tlsAllowInvalidHostnames" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>)
    tls_allow_invalid_hostnames() const;

    ///
    /// Returns the value of the option "tlsCAFile" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view>)
    tls_ca_file() const;

    ///
    /// Returns the value of the option "tlsCertificateKeyFile" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view>)
    tls_certificate_key_file() const;

    ///
    /// Returns the value of the option "tlsCertificateKeyFilePassword" if present in the uri.
    ///
    /// @return An optional bsoncxx::v_noabi::stdx::string_view
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::stdx::string_view>)
    tls_certificate_key_file_password() const;

    ///
    /// Returns the value of the option "tlsDisableCertificateRevocationCheck" if present in the
    /// uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>)
    tls_disable_certificate_revocation_check() const;

    ///
    /// Returns the value of the option "tlsDisableOCSPEndpointCheck" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>)
    tls_disable_ocsp_endpoint_check() const;

    ///
    /// Returns the value of the option "tlsInsecure" if present in the uri.
    ///
    /// @return An optional bool
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool>) tls_insecure() const;

    ///
    /// Returns the value of the option "waitQueueTimeoutMS" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    wait_queue_timeout_ms() const;

    ///
    /// Returns the value of the option "zlibCompressionLevel" if present in the uri.
    ///
    /// @return An optional std::int32_t
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t>)
    zlib_compression_level() const;

   private:
    friend ::mongocxx::v_noabi::client;
    friend ::mongocxx::v_noabi::pool;

    class impl;

    uri(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::uri.
///
