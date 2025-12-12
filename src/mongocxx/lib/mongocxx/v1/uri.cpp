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

#include <mongocxx/v1/uri.hh>

//

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/write_concern.hh>

#include <cstdint>
#include <string>
#include <system_error>
#include <vector>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

#include <bson/error.h>

namespace mongocxx {
namespace v1 {

using code = uri::errc;

namespace {

mongoc_uri_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_uri_t*>(ptr);
}

template <typename T>
bsoncxx::v1::stdx::optional<T> get_field(mongoc_uri_t const* uri, char const* field);

template <>
bsoncxx::v1::stdx::optional<std::int32_t> get_field(mongoc_uri_t const* uri, char const* field) {
    auto const options = libmongoc::uri_get_options(uri); // Never null.

    bson_iter_t iter = {};
    if (bson_iter_init_find_case(&iter, options, field) && bson_iter_type(&iter) == BSON_TYPE_INT32) {
        return bson_iter_int32(&iter);
    }

    return {};
}

template <>
bsoncxx::v1::stdx::optional<bool> get_field(mongoc_uri_t const* uri, char const* field) {
    auto const options = libmongoc::uri_get_options(uri); // Never null.

    bson_iter_t iter = {};
    if (bson_iter_init_find_case(&iter, options, field) && bson_iter_type(&iter) == BSON_TYPE_BOOL) {
        return bson_iter_bool(&iter);
    }

    return {};
}

template <>
bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> get_field(mongoc_uri_t const* uri, char const* field) {
    auto const options = libmongoc::uri_get_options(uri); // Never null.

    bson_iter_t iter = {};
    if (bson_iter_init_find_case(&iter, options, field) && bson_iter_type(&iter) == BSON_TYPE_UTF8) {
        std::uint32_t len = {};
        auto const data = bson_iter_utf8(&iter, &len);
        return bsoncxx::v1::stdx::string_view{data, len};
    }

    return {};
}

template <typename T>
bsoncxx::v1::stdx::optional<T> get_credentials_field(mongoc_uri_t const* uri, char const* field);

template <>
bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> get_credentials_field(
    mongoc_uri_t const* uri,
    char const* field) {
    if (auto const creds = libmongoc::uri_get_credentials(uri)) {
        bson_iter_t iter = {};
        if (bson_iter_init_find_case(&iter, creds, field) && bson_iter_type(&iter) == BSON_TYPE_DOCUMENT) {
            std::uint8_t const* data = {};
            std::uint32_t len = {};
            bson_iter_document(&iter, &len, &data);
            return bsoncxx::v1::document::view{data, len};
        }
    }

    return {};
}

} // namespace

uri::~uri() {
    libmongoc::uri_destroy(to_mongoc(_impl));
}

uri::uri(uri&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

uri& uri::operator=(uri&& other) noexcept {
    if (this != &other) {
        libmongoc::uri_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }

    return *this;
}

uri::uri(uri const& other) : _impl{libmongoc::uri_copy(to_mongoc(other._impl))} {}

uri& uri::operator=(uri const& other) {
    if (this != &other) {
        libmongoc::uri_destroy(to_mongoc(exchange(_impl, libmongoc::uri_copy(to_mongoc(other._impl)))));
    }

    return *this;
}

uri::uri() : _impl{libmongoc::uri_new_with_error(k_default_uri, nullptr)} {}

uri::uri(bsoncxx::v1::stdx::string_view v)
    : _impl{[&]() -> void* {
          bson_error_t error = {};
          if (mongoc_uri_t* ret = libmongoc::uri_new_with_error(std::string{v}.c_str(), &error)) {
              return ret;
          }
          throw_exception(error);
      }()} {}

bsoncxx::v1::stdx::string_view uri::auth_mechanism() const {
    if (auto const v = libmongoc::uri_get_auth_mechanism(to_mongoc(_impl))) {
        return v;
    }
    return {};
}

bsoncxx::v1::stdx::string_view uri::auth_source() const {
    if (auto const v = libmongoc::uri_get_auth_source(to_mongoc(_impl))) {
        return v;
    }
    return {}; // Never null?
}

std::vector<uri::host> uri::hosts() const {
    std::vector<host> ret;

    for (auto iter = libmongoc::uri_get_hosts(to_mongoc(_impl)); iter != nullptr; iter = iter->next) {
        ret.push_back(host{iter->host, iter->port, iter->family});
    }

    return ret;
}

bsoncxx::v1::stdx::string_view uri::database() const {
    if (auto const v = libmongoc::uri_get_database(to_mongoc(_impl))) {
        return v;
    }
    return {};
}

bsoncxx::v1::document::view uri::options() const {
    return scoped_bson_view{libmongoc::uri_get_options(to_mongoc(_impl))}.view();
}

bsoncxx::v1::stdx::string_view uri::password() const {
    if (auto const v = libmongoc::uri_get_password(to_mongoc(_impl))) {
        return v;
    }
    return {};
}

v1::read_concern uri::read_concern() const {
    return v1::read_concern::internal::make(
        libmongoc::read_concern_copy(libmongoc::uri_get_read_concern(to_mongoc(_impl))));
}

v1::read_preference uri::read_preference() const {
    return v1::read_preference::internal::make(
        libmongoc::read_prefs_copy(libmongoc::uri_get_read_prefs_t(to_mongoc(_impl))));
}

bsoncxx::v1::stdx::string_view uri::replica_set() const {
    if (auto const v = libmongoc::uri_get_replica_set(to_mongoc(_impl))) {
        return v;
    }
    return {};
}

bool uri::tls() const {
    return libmongoc::uri_get_tls(to_mongoc(_impl));
}

bsoncxx::v1::stdx::string_view uri::to_string() const {
    return libmongoc::uri_get_string(to_mongoc(_impl));
}

bsoncxx::v1::stdx::string_view uri::username() const {
    if (auto const v = libmongoc::uri_get_username(to_mongoc(_impl))) {
        return v;
    }
    return {};
}

v1::write_concern uri::write_concern() const {
    return v1::write_concern::internal::make(
        libmongoc::write_concern_copy(libmongoc::uri_get_write_concern(to_mongoc(_impl))));
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> uri::appname() const {
    if (auto const v = libmongoc::uri_get_appname(to_mongoc(_impl))) {
        return v;
    }
    return {};
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> uri::auth_mechanism_properties() const {
    return get_credentials_field<bsoncxx::v1::document::view>(to_mongoc(_impl), MONGOC_URI_AUTHMECHANISMPROPERTIES);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> uri::credentials() const {
    if (auto const v = libmongoc::uri_get_credentials(to_mongoc(_impl))) {
        return scoped_bson_view{v}.view();
    }
    return {}; // Never null?
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::srv_max_hosts() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_SRVMAXHOSTS);
}

std::vector<bsoncxx::v1::stdx::string_view> uri::compressors() const {
    if (auto const doc = scoped_bson_view{libmongoc::uri_get_compressors(to_mongoc(_impl))}.view()) {
        std::vector<bsoncxx::v1::stdx::string_view> ret;

        for (auto const e : doc) {
            ret.push_back(e.key());
        }

        return ret;
    }

    return {};
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::connect_timeout_ms() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_CONNECTTIMEOUTMS);
}

bsoncxx::v1::stdx::optional<bool> uri::direct_connection() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_DIRECTCONNECTION);
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::heartbeat_frequency_ms() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_HEARTBEATFREQUENCYMS);
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::local_threshold_ms() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_LOCALTHRESHOLDMS);
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::max_pool_size() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_MAXPOOLSIZE);
}

bsoncxx::v1::stdx::optional<bool> uri::retry_reads() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_RETRYREADS);
}

bsoncxx::v1::stdx::optional<bool> uri::retry_writes() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_RETRYWRITES);
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::server_selection_timeout_ms() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_SERVERSELECTIONTIMEOUTMS);
}

bsoncxx::v1::stdx::optional<bool> uri::server_selection_try_once() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_SERVERSELECTIONTRYONCE);
}

uri& uri::server_selection_try_once(bool val) {
    if (!libmongoc::uri_set_option_as_bool(to_mongoc(_impl), MONGOC_URI_SERVERSELECTIONTRYONCE, val)) {
        throw v1::exception::internal::make(code::set_failure, MONGOC_URI_SERVERSELECTIONTRYONCE);
    }
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::socket_timeout_ms() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_SOCKETTIMEOUTMS);
}

bsoncxx::v1::stdx::optional<bool> uri::tls_allow_invalid_certificates() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_TLSALLOWINVALIDCERTIFICATES);
}

bsoncxx::v1::stdx::optional<bool> uri::tls_allow_invalid_hostnames() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_TLSALLOWINVALIDHOSTNAMES);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> uri::tls_ca_file() const {
    return get_field<bsoncxx::v1::stdx::string_view>(to_mongoc(_impl), MONGOC_URI_TLSCAFILE);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> uri::tls_certificate_key_file() const {
    return get_field<bsoncxx::v1::stdx::string_view>(to_mongoc(_impl), MONGOC_URI_TLSCERTIFICATEKEYFILE);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> uri::tls_certificate_key_file_password() const {
    return get_field<bsoncxx::v1::stdx::string_view>(to_mongoc(_impl), MONGOC_URI_TLSCERTIFICATEKEYFILEPASSWORD);
}

bsoncxx::v1::stdx::optional<bool> uri::tls_disable_certificate_revocation_check() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_TLSDISABLECERTIFICATEREVOCATIONCHECK);
}

bsoncxx::v1::stdx::optional<bool> uri::tls_disable_ocsp_endpoint_check() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_TLSDISABLEOCSPENDPOINTCHECK);
}

bsoncxx::v1::stdx::optional<bool> uri::tls_insecure() const {
    return get_field<bool>(to_mongoc(_impl), MONGOC_URI_TLSINSECURE);
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::wait_queue_timeout_ms() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_WAITQUEUETIMEOUTMS);
}

bsoncxx::v1::stdx::optional<std::int32_t> uri::zlib_compression_level() const {
    return get_field<std::int32_t>(to_mongoc(_impl), MONGOC_URI_ZLIBCOMPRESSIONLEVEL);
}

std::error_category const& uri::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::uri";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::set_failure:
                    return "could not set the requested URI option";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::set_failure:
                        return source == condition::mongocxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::set_failure:
                        return type == condition::invalid_argument;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

uri::uri(void* impl) : _impl{impl} {}

uri uri::internal::make(mongoc_uri_t* uri) {
    return v1::uri{uri};
}

mongoc_uri_t const* uri::internal::as_mongoc(uri const& self) {
    return to_mongoc(self._impl);
}

} // namespace v1
} // namespace mongocxx
