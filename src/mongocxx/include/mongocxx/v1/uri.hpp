///
/// A MongoDB connection string.
///
/// @see
/// - [Connection Strings (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/connection-string/)
///
/// @attention This feature is experimental! It is not ready for use!
///// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/uri-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/read_concern-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB connection string.
///
/// Supported fields include:
/// - `appname` ("appName")
/// - `auth_mechanism_properties` ("authMechanismProperties")
/// - `auth_mechanism` ("authMechanism")
/// - `auth_source` ("authSource")
/// - `compressors`
/// - `connect_timeout_ms` ("connectTimeoutMS")
/// - `database`
/// - `direct_connection` ("directConnection")
/// - `heartbeat_frequency_ms` ("heartbeatFrequencyMS")
/// - `hosts`
/// - `local_threshold_ms` ("localThresholdMS")
/// - `max_pool_size` ("maxPoolSize")
/// - `password`
/// - `read_concern` ("readConcern")
/// - `read_preference` ("readPreference")
/// - `replica_set` ("replicaSet")
/// - `retry_reads` ("retryReads")
/// - `retry_writes` ("retryWrites")
/// - `server_selection_timeout_ms` ("serverSelectionTimeoutMS")
/// - `server_selection_try_once` ("serverSelectionTryOnce")
/// - `socket_timeout_ms` ("socketTimeoutMS")
/// - `srv_max_hosts` ("srvMaxHosts")
/// - `tls_allow_invalid_certificates` ("tlsAllowInvalidCertificates")
/// - `tls_allow_invalid_hostnames` ("tlsAllowInvalidHostnames")
/// - `tls_ca_file` ("tlsCAFile")
/// - `tls_certificate_key_file_password` ("tlsCertificateKeyFilePassword")
/// - `tls_certificate_key_file` ("tlsCertificateKeyFile")
/// - `tls_disable_certificate_revocation_check`
/// - `tls_disable_ocsp_endpoint_check`
/// - `tls_insecure` ("tlsInsecure")
/// - `tls`
/// - `username`
/// - `wait_queue_timeout_ms` ("waitQueueTimeoutMS")
/// - `write_concern` ("writeConcern")
/// - `zlib_compression_level` ("zlibCompressionLevel")
///
/// @see
/// - [Connection Strings (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/connection-string/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class uri {
   private:
    void* _impl; // mongoc_uri_t

   public:
    ///
    /// A host identifier.
    ///
    struct host {
        std::string name;    ///< The host name.
        std::uint16_t port;  ///< The port number.
        std::int32_t family; ///< The address family.
    };

    ///
    /// The default connection string.
    ///
    static constexpr auto k_default_uri = "mongodb://localhost:27017";

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~uri();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() uri(uri&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(uri&) operator=(uri&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() uri(uri const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(uri&) operator=(uri const& other);

    ///
    /// Equivalent to @ref uri(bsoncxx::v1::stdx::string_view v) with @ref k_default_uri.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() uri();

    ///
    /// Initialize with the given connection string.
    ///
    /// @par Postconditions:
    /// - All supported options specified by the connection string are set accordingly.
    /// - `this->options()` returns a document containing all options specified by the connection string.
    /// - All other supported options are "unset" or zero-initialized.
    ///
    /// @throws mongocxx::v1::exception if a client-side error is encountered.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() uri(bsoncxx::v1::stdx::string_view v);

    ///
    /// Return the "authMechanism" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) auth_mechanism() const;

    ///
    /// Return the "authSource" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) auth_source() const;

    ///
    /// Return the host(s) identifiers specified by the connection string.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<host>) hosts() const;

    ///
    /// Return the database name specified by the connection string.
    ///
    /// @returns Empty when no database name was specified.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) database() const;

    ///
    /// Return all URI options that were specified by the connection string.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) options() const;

    ///
    /// Return the password specified by the connection string.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) password() const;

    ///
    /// Return the "readConcern" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::read_concern) read_concern() const;

    ///
    /// Return the "readPreference" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::read_preference) read_preference() const;

    ///
    /// Return the "replicaSet" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) replica_set() const;

    ///
    /// Return the "tls" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) tls() const;

    ///
    /// Return this URI as a connection string.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) to_string() const;

    ///
    /// Return the username specified by the connection string.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) username() const;

    ///
    /// Return the "writeConcern" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::write_concern) write_concern() const;

    ///
    /// Return the "appName" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) appname() const;

    ///
    /// Return the "authMechanismProperties" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) auth_mechanism_properties()
        const;

    ///
    /// Return the mongoc "credentials" field containing "authMechanism" and related options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) credentials() const;

    ///
    /// Return the "srvMaxHosts" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) srv_max_hosts() const;

    ///
    /// Return the "compressors" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<bsoncxx::v1::stdx::string_view>) compressors() const;

    ///
    /// Return the "connectTimeoutMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) connect_timeout_ms() const;

    ///
    /// Return the "directConnection" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) direct_connection() const;

    ///
    /// Return the "heartbeatFrequencyMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) heartbeat_frequency_ms() const;

    ///
    /// Return the "localThresholdMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) local_threshold_ms() const;

    ///
    /// Return the "maxPoolSize" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) max_pool_size() const;

    ///
    /// Return the "retryReads" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) retry_reads() const;

    ///
    /// Return the "retryWrites" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) retry_writes() const;

    ///
    /// Return the "serverSelectionTimeoutMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) server_selection_timeout_ms() const;

    ///
    /// Return the "serverSelectionTryOnce" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) server_selection_try_once() const;

    ///
    /// Set the "serverSelectionTryOnce" option.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::uri::errc::set_failure if mongoc failed to set this URI
    /// option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(uri&) server_selection_try_once(bool val);

    ///
    /// Return the "socketTimeoutMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) socket_timeout_ms() const;

    ///
    /// Return the "tlsAllowInvalidCertificates" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) tls_allow_invalid_certificates() const;

    ///
    /// Return the "tlsAllowInvalidHostnames" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) tls_allow_invalid_hostnames() const;

    ///
    /// Return the "tlsCAFile" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) tls_ca_file() const;

    ///
    /// Return the "tlsCertificateKeyFile" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) tls_certificate_key_file()
        const;

    ///
    /// Return the "tlsCertificateKeyFilePassword" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>)
    tls_certificate_key_file_password() const;

    ///
    /// Return the (mongoc-specific) "tlsDisableCertificateRevocationCheck" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) tls_disable_certificate_revocation_check() const;

    ///
    /// Return the (mongoc-specific) "tlsDisableOCSPEndpointCheck" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) tls_disable_ocsp_endpoint_check() const;

    ///
    /// Return the "tlsInsecure" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) tls_insecure() const;

    ///
    /// Return the "waitQueueTimeoutMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) wait_queue_timeout_ms() const;

    ///
    /// Return the "zlibCompressionLevel" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) zlib_compression_level() const;

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::uri.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,        ///< Zero.
        set_failure, ///< Failed to set the requested URI option.
    };

    ///
    /// The error category for @ref mongocxx::v1::uri::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }

    class internal;

   private:
    // MSVC incorrectly picks this ctor given `char const*` arguments.
    explicit uri(void* impl);
};

BSONCXX_PRIVATE_INLINE_CXX17 constexpr char const* uri::k_default_uri;

} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::uri::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::uri.
///
