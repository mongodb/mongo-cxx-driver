//
// Licensed under the Apache License, Version 2.0 (the "License");
// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/client.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/client_session.hpp>
#include <mongocxx/v1/database.hpp>
#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/server_api.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/apm.hh>
#include <mongocxx/v1/auto_encryption_options.hh>
#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/database.hh>
#include <mongocxx/v1/events/command_failed.hh>
#include <mongocxx/v1/events/command_started.hh>
#include <mongocxx/v1/events/command_succeeded.hh>
#include <mongocxx/v1/events/server_closed.hh>
#include <mongocxx/v1/events/server_description_changed.hh>
#include <mongocxx/v1/events/server_heartbeat_failed.hh>
#include <mongocxx/v1/events/server_heartbeat_started.hh>
#include <mongocxx/v1/events/server_heartbeat_succeeded.hh>
#include <mongocxx/v1/events/server_opening.hh>
#include <mongocxx/v1/events/topology_closed.hh>
#include <mongocxx/v1/events/topology_description_changed.hh>
#include <mongocxx/v1/events/topology_opening.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/pipeline.hh>
#include <mongocxx/v1/tls.hh>
#include <mongocxx/v1/uri.hh>

#include <exception>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <system_error>
#include <vector>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/ssl.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

using code = client::errc;

namespace {

// An APM callback exiting via an exception is documented as being undefined behavior.
// For QoI, terminate the program before allowing the exception to bypass libmongoc code.
template <typename Fn>
void exception_guard(char const* source, Fn fn) noexcept {
    try {
        fn();
    } catch (...) {
        std::cerr << "fatal error: APM callback " << source << " exited via an exception" << std::endl;
        std::terminate();
    }
}

void command_started(mongoc_apm_command_started_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_command_started_get_context(v));
    auto const event = v1::events::command_started::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::command_started(context)(event); });
}

void command_failed(mongoc_apm_command_failed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_command_failed_get_context(v));
    auto const event = v1::events::command_failed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::command_failed(context)(event); });
}

void command_succeeded(mongoc_apm_command_succeeded_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_command_succeeded_get_context(v));
    auto const event = v1::events::command_succeeded::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::command_succeeded(context)(event); });
}

void server_opening(mongoc_apm_server_opening_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_opening_get_context(v));
    auto const event = v1::events::server_opening::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_opening(context)(event); });
}

void server_closed(mongoc_apm_server_closed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_closed_get_context(v));
    auto const event = v1::events::server_closed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_closed(context)(event); });
}

void server_description_changed(mongoc_apm_server_changed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_changed_get_context(v));
    auto const event = v1::events::server_description_changed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_description_changed(context)(event); });
}

void topology_opening(mongoc_apm_topology_opening_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_topology_opening_get_context(v));
    auto const event = v1::events::topology_opening::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::topology_opening(context)(event); });
}

void topology_closed(mongoc_apm_topology_closed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_topology_closed_get_context(v));
    auto const event = v1::events::topology_closed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::topology_closed(context)(event); });
}

void topology_description_changed(mongoc_apm_topology_changed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_topology_changed_get_context(v));
    auto const event = v1::events::topology_description_changed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::topology_description_changed(context)(event); });
}

void server_heartbeat_started(mongoc_apm_server_heartbeat_started_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_heartbeat_started_get_context(v));
    auto const event = v1::events::server_heartbeat_started::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_heartbeat_started(context)(event); });
}

void server_heartbeat_failed(mongoc_apm_server_heartbeat_failed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_heartbeat_failed_get_context(v));
    auto const event = v1::events::server_heartbeat_failed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_heartbeat_failed(context)(event); });
}

void server_heartbeat_succeeded(mongoc_apm_server_heartbeat_succeeded_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_heartbeat_succeeded_get_context(v));
    auto const event = v1::events::server_heartbeat_succeeded::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_heartbeat_succeeded(context)(event); });
}

class apm_callbacks {
   public:
    mongoc_apm_callbacks_t* _callbacks = libmongoc::apm_callbacks_new();

    ~apm_callbacks() {
        libmongoc::apm_callbacks_destroy(_callbacks);
    }

    apm_callbacks(apm_callbacks&& other) = delete;
    apm_callbacks& operator=(apm_callbacks&& other) = delete;
    apm_callbacks(apm_callbacks const& other) = delete;
    apm_callbacks& operator=(apm_callbacks const& other) = delete;

    explicit apm_callbacks(v1::apm const& apm) {
        if (v1::apm::internal::command_started(apm)) {
            libmongoc::apm_set_command_started_cb(_callbacks, command_started);
        }

        if (v1::apm::internal::command_failed(apm)) {
            libmongoc::apm_set_command_failed_cb(_callbacks, command_failed);
        }

        if (v1::apm::internal::command_succeeded(apm)) {
            libmongoc::apm_set_command_succeeded_cb(_callbacks, command_succeeded);
        }

        if (v1::apm::internal::server_opening(apm)) {
            libmongoc::apm_set_server_opening_cb(_callbacks, server_opening);
        }

        if (v1::apm::internal::server_closed(apm)) {
            libmongoc::apm_set_server_closed_cb(_callbacks, server_closed);
        }

        if (v1::apm::internal::server_description_changed(apm)) {
            libmongoc::apm_set_server_changed_cb(_callbacks, server_description_changed);
        }

        if (v1::apm::internal::topology_opening(apm)) {
            libmongoc::apm_set_topology_opening_cb(_callbacks, topology_opening);
        }

        if (v1::apm::internal::topology_closed(apm)) {
            libmongoc::apm_set_topology_closed_cb(_callbacks, topology_closed);
        }

        if (v1::apm::internal::topology_description_changed(apm)) {
            libmongoc::apm_set_topology_changed_cb(_callbacks, topology_description_changed);
        }

        if (v1::apm::internal::server_heartbeat_started(apm)) {
            libmongoc::apm_set_server_heartbeat_started_cb(_callbacks, server_heartbeat_started);
        }

        if (v1::apm::internal::server_heartbeat_failed(apm)) {
            libmongoc::apm_set_server_heartbeat_failed_cb(_callbacks, server_heartbeat_failed);
        }

        if (v1::apm::internal::server_heartbeat_succeeded(apm)) {
            libmongoc::apm_set_server_heartbeat_succeeded_cb(_callbacks, server_heartbeat_succeeded);
        }
    }
};

struct mongoc_auto_encryption_opts_deleter {
    void operator()(mongoc_auto_encryption_opts_t* ptr) const noexcept {
        libmongoc::auto_encryption_opts_destroy(ptr);
    }
};

std::unique_ptr<mongoc_auto_encryption_opts_t, mongoc_auto_encryption_opts_deleter> to_mongoc(
    v1::auto_encryption_options const& opts) {
    std::unique_ptr<mongoc_auto_encryption_opts_t, mongoc_auto_encryption_opts_deleter> ret{
        libmongoc::auto_encryption_opts_new()};

    auto const ptr = ret.get();

    if (auto const& opt = opts.key_vault_client()) {
        libmongoc::auto_encryption_opts_set_keyvault_client(ptr, v1::client::internal::as_mongoc(*opt));
    }

    if (auto const& opt = opts.key_vault_pool()) {
        (void)opt; // TODO: v1::pool (CXX-3237)
    }

    if (auto const& opt = opts.key_vault_namespace()) {
        libmongoc::auto_encryption_opts_set_keyvault_namespace(ptr, opt->first.c_str(), opt->second.c_str());
    }

    if (auto const& opt = opts.kms_providers()) {
        libmongoc::auto_encryption_opts_set_kms_providers(ptr, scoped_bson_view{*opt}.bson());
    }

    if (auto const& opt = opts.tls_opts()) {
        libmongoc::auto_encryption_opts_set_tls_opts(ptr, scoped_bson_view{*opt}.bson());
    }

    if (auto const& opt = opts.schema_map()) {
        libmongoc::auto_encryption_opts_set_schema_map(ptr, scoped_bson_view{*opt}.bson());
    }

    if (auto const& opt = opts.encrypted_fields_map()) {
        libmongoc::auto_encryption_opts_set_encrypted_fields_map(ptr, scoped_bson_view{*opt}.bson());
    }

    libmongoc::auto_encryption_opts_set_bypass_auto_encryption(ptr, opts.bypass_auto_encryption());
    libmongoc::auto_encryption_opts_set_bypass_query_analysis(ptr, opts.bypass_query_analysis());

    if (auto const& opt = opts.extra_options()) {
        libmongoc::auto_encryption_opts_set_extra(ptr, scoped_bson_view{*opt}.bson());
    }

    return ret;
}

struct mongoc_server_api_deleter {
    void operator()(mongoc_server_api_t* ptr) const noexcept {
        libmongoc::server_api_destroy(ptr);
    }
};

std::unique_ptr<mongoc_server_api_t, mongoc_server_api_deleter> to_mongoc(v1::server_api const& api) {
    mongoc_server_api_version_t version = {};

    if (!libmongoc::server_api_version_from_string(
            v1::server_api::version_to_string(api.get_version()).c_str(), &version)) {
        // Invariant: enforced by `v1::server_api::errc::invalid_version`.
        MONGOCXX_PRIVATE_UNREACHABLE;
    }

    std::unique_ptr<mongoc_server_api_t, mongoc_server_api_deleter> ret{libmongoc::server_api_new(version)};

    auto const ptr = ret.get();

    if (auto const opt = api.strict()) {
        libmongoc::server_api_strict(ptr, *opt);
    }

    if (auto const opt = api.deprecation_errors()) {
        libmongoc::server_api_deprecation_errors(ptr, *opt);
    }

    return ret;
}

#if MONGOCXX_SSL_IS_ENABLED()
mongoc_ssl_opt_t to_mongoc(v1::tls const& opts) {
    mongoc_ssl_opt_t ret = {};

    if (auto const& opt = v1::tls::internal::pem_file(opts)) {
        ret.pem_file = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::pem_password(opts)) {
        ret.pem_pwd = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::ca_file(opts)) {
        ret.ca_file = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::ca_dir(opts)) {
        ret.ca_dir = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::crl_file(opts)) {
        ret.crl_file = opt->c_str();
    }

    ret.weak_cert_validation = opts.allow_invalid_certificates().value_or(false);

    return ret;
}
#endif

} // namespace

class client::impl {
   public:
    mongoc_client_t* _client;
    v1::apm _apm;

    ~impl() {
        libmongoc::client_destroy(_client);
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;
    impl(impl const& other) noexcept = delete;
    impl& operator=(impl const& other) noexcept = delete;

    explicit impl(mongoc_uri_t const* uri)
        : _client{[&] {
              bson_error_t error = {};

              if (auto const ret = libmongoc::client_new_from_uri_with_error(uri, &error)) {
                  return ret;
              }

              v1::throw_exception(error);
          }()} {}

    explicit impl(mongoc_client_t* client) : _client{client} {}

    static impl const& with(client const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(client const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(client& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(client* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

client::~client() {
    delete impl::with(this);
}

client::client(client&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client& client::operator=(client&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

client::client(v1::uri uri, options opts) : client{new impl{v1::uri::internal::as_mongoc(uri)}} {
    auto const& _client = impl::with(this)->_client;

    if (auto& opt = options::internal::apm_opts(opts)) {
        internal::set_apm(*this, std::move(*opt));
    }

    if (auto const& opt = options::internal::auto_encryption_opts(opts)) {
        bson_error_t error = {};

        if (!libmongoc::client_enable_auto_encryption(_client, to_mongoc(*opt).get(), &error)) {
            v1::throw_exception(error);
        }
    }

    if (auto const& opt = options::internal::server_api_opts(opts)) {
        bson_error_t error = {};

        if (!libmongoc::client_set_server_api(_client, to_mongoc(*opt).get(), &error)) {
            v1::throw_exception(error);
        }
    }

    {
        auto const tls_enabled = uri.tls();

#if MONGOCXX_SSL_IS_ENABLED()
        if (auto const& opt = options::internal::tls_opts(opts)) {
            if (!tls_enabled) {
                throw v1::exception::internal::make(errc::tls_not_enabled);
            }

            auto const v = to_mongoc(*opt);
            libmongoc::client_set_ssl_opts(_client, &v);
        }
#else
        if (tls_enabled || options::internal::tls_opts(opts)) {
            throw v1::exception::internal::make(errc::tls_not_supported);
        }
#endif
    }
}

client::client(v1::uri uri) : client{std::move(uri), {}} {}

client::client() : _impl{nullptr} {}

client::operator bool() const {
    return _impl != nullptr;
}

v1::uri client::uri() const {
    return v1::uri::internal::make(libmongoc::uri_copy(libmongoc::client_get_uri(impl::with(this)->_client)));
}

v1::database client::database(bsoncxx::v1::stdx::string_view name) {
    auto const _client = impl::with(this)->_client;

    return v1::database::internal::make(libmongoc::client_get_database(_client, std::string{name}.c_str()), _client);
}

v1::database client::operator[](bsoncxx::v1::stdx::string_view name) {
    return this->database(name);
}

namespace {

v1::cursor list_databases_impl(mongoc_client_t* client, bson_t const* opts) {
    return v1::cursor::internal::make(libmongoc::client_find_databases_with_opts(client, opts));
}

} // namespace

v1::cursor client::list_databases() {
    return list_databases_impl(impl::with(this)->_client, nullptr);
}

v1::cursor client::list_databases(v1::client_session const& session) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    return list_databases_impl(impl::with(this)->_client, doc.bson());
}

v1::cursor client::list_databases(bsoncxx::v1::document::view opts) {
    return list_databases_impl(impl::with(this)->_client, scoped_bson_view{opts}.bson());
}

v1::cursor client::list_databases(v1::client_session const& session, bsoncxx::v1::document::view opts) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    doc += opts;

    return list_databases_impl(impl::with(this)->_client, doc.bson());
}

namespace {

std::vector<std::string> list_database_names_impl(mongoc_client_t* client, bson_t const* opts) {
    struct names_deleter {
        void operator()(char** ptr) const noexcept {
            bson_strfreev(ptr);
        }
    };

    using names_type = std::unique_ptr<char*, names_deleter>;

    bson_error_t error = {};

    if (auto const names = names_type{libmongoc::client_get_database_names_with_opts(client, opts, &error)}) {
        std::vector<std::string> ret;

        for (char const* const* iter = names.get(); *iter != nullptr; ++iter) {
            ret.emplace_back(*iter);
        }

        return ret;
    }

    v1::throw_exception(error);
}

} // namespace

std::vector<std::string> client::list_database_names(bsoncxx::v1::document::view filter) {
    scoped_bson const opts{BCON_NEW("filter", BCON_DOCUMENT(scoped_bson_view{filter}.bson()))};
    return list_database_names_impl(impl::with(this)->_client, opts.bson());
}

std::vector<std::string> client::list_database_names(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    doc += scoped_bson{BCON_NEW("filter", BCON_DOCUMENT(scoped_bson_view{filter}.bson()))};
    return list_database_names_impl(impl::with(this)->_client, doc.bson());
}

std::vector<std::string> client::list_database_names() {
    return list_database_names_impl(impl::with(this)->_client, nullptr);
}

std::vector<std::string> client::list_database_names(v1::client_session const& session) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    return list_database_names_impl(impl::with(this)->_client, doc.bson());
}

namespace {

v1::client_session start_session_impl(v1::client& client, mongoc_session_opt_t const* opts) {
    bson_error_t error = {};

    if (auto const ptr = libmongoc::client_start_session(v1::client::internal::as_mongoc(client), opts, &error)) {
        return v1::client_session::internal::make(ptr, client);
    }

    v1::throw_exception(error);
}

} // namespace

v1::client_session client::start_session(v1::client_session::options const& opts) {
    return start_session_impl(*this, v1::client_session::options::internal::as_mongoc(opts));
}

v1::client_session client::start_session() {
    return start_session_impl(*this, nullptr);
}

namespace {

v1::change_stream watch_impl(mongoc_client_t* client, bsoncxx::v1::array::view pipeline, bson_t const* opts) {
    return v1::change_stream::internal::make(libmongoc::client_watch(client, scoped_bson_view{pipeline}.bson(), opts));
}

} // namespace

v1::change_stream client::watch(v1::change_stream::options const& opts) {
    return watch_impl(
        impl::with(this)->_client,
        bsoncxx::v1::array::view{},
        scoped_bson{v1::change_stream::options::internal::to_document(opts)}.bson());
}

v1::change_stream client::watch() {
    return watch_impl(impl::with(this)->_client, bsoncxx::v1::array::view{}, nullptr);
}

v1::change_stream client::watch(v1::client_session const& session, v1::change_stream::options const& opts) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    doc += v1::change_stream::options::internal::to_document(opts);
    return watch_impl(impl::with(this)->_client, bsoncxx::v1::array::view{}, doc.bson());
}

v1::change_stream client::watch(v1::client_session const& session) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    return watch_impl(impl::with(this)->_client, bsoncxx::v1::array::view{}, doc.bson());
}

v1::change_stream client::watch(v1::pipeline const& pipeline, v1::change_stream::options const& opts) {
    return watch_impl(
        impl::with(this)->_client,
        pipeline.view_array(),
        scoped_bson{v1::change_stream::options::internal::to_document(opts)}.bson());
}

v1::change_stream
client::watch(v1::client_session const& session, v1::pipeline const& pipeline, v1::change_stream::options const& opts) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    doc += v1::change_stream::options::internal::to_document(opts);
    return watch_impl(impl::with(this)->_client, pipeline.view_array(), doc.bson());
}

v1::change_stream client::watch(v1::pipeline const& pipeline) {
    return watch_impl(impl::with(this)->_client, pipeline.view_array(), nullptr);
}

v1::change_stream client::watch(v1::client_session const& session, v1::pipeline const& pipeline) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(session, doc, error)) {
        v1::throw_exception(error);
    }

    return watch_impl(impl::with(this)->_client, pipeline.view_array(), doc.bson());
}

void client::reset() {
    libmongoc::client_reset(impl::with(this)->_client);
}

std::error_category const& client::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::client";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::tls_not_enabled:
                    return "TLS is not enabled by the URI option";
                case code::tls_not_supported:
                    return "TLS is not supported by the mongoc library";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::tls_not_enabled:
                    case code::tls_not_supported:
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
                    case code::tls_not_enabled:
                    case code::tls_not_supported:
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

client::client(void* impl) : _impl{impl} {}

client client::internal::make(mongoc_client_t* client) {
    return {new impl{client}};
}

void client::internal::set_apm(client& self, v1::apm v) {
    auto& _apm = impl::with(self)._apm;
    _apm = std::move(v);
    libmongoc::client_set_apm_callbacks(impl::with(self)._client, apm_callbacks{_apm}._callbacks, &_apm);
}

void client::internal::disown(client& self) {
    impl::with(self)._client = nullptr;
}

mongoc_client_t const* client::internal::as_mongoc(client const& self) {
    return impl::with(self)._client;
}

mongoc_client_t* client::internal::as_mongoc(client& self) {
    return impl::with(self)._client;
}

class client::options::impl {
   public:
    bsoncxx::v1::stdx::optional<v1::tls> _tls_opts;
    bsoncxx::v1::stdx::optional<v1::auto_encryption_options> _auto_encryption_opts;
    bsoncxx::v1::stdx::optional<v1::apm> _apm_opts;
    bsoncxx::v1::stdx::optional<v1::server_api> _server_api_opts;

    static impl const& with(options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

client::options::~options() {
    delete impl::with(this);
}

client::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client::options& client::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

client::options& client::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

client::options& client::options::tls_opts(v1::tls v) {
    impl::with(this)->_tls_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::tls> client::options::tls_opts() const {
    return impl::with(this)->_tls_opts;
}

client::options& client::options::auto_encryption_opts(v1::auto_encryption_options v) {
    impl::with(this)->_auto_encryption_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::auto_encryption_options> client::options::auto_encryption_opts() const {
    return impl::with(this)->_auto_encryption_opts;
}

client::options& client::options::apm_opts(v1::apm v) {
    impl::with(this)->_apm_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::apm> client::options::apm_opts() const {
    return impl::with(this)->_apm_opts;
}

client::options& client::options::server_api_opts(v1::server_api v) {
    impl::with(this)->_server_api_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::server_api> client::options::server_api_opts() const {
    return impl::with(this)->_server_api_opts;
}

bsoncxx::v1::stdx::optional<v1::tls>& client::options::internal::tls_opts(options& self) {
    return impl::with(self)._tls_opts;
}

bsoncxx::v1::stdx::optional<v1::auto_encryption_options>& client::options::internal::auto_encryption_opts(
    options& self) {
    return impl::with(self)._auto_encryption_opts;
}

bsoncxx::v1::stdx::optional<v1::apm>& client::options::internal::apm_opts(options& self) {
    return impl::with(self)._apm_opts;
}

bsoncxx::v1::stdx::optional<v1::server_api>& client::options::internal::server_api_opts(options& self) {
    return impl::with(self)._server_api_opts;
}

} // namespace v1
} // namespace mongocxx
