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

#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/uri.hh>
#include <mongocxx/v1/write_concern.hh>

#include <bsoncxx/builder/basic/kvp.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/pipeline.hpp>

#include <mongocxx/client.hh>
#include <mongocxx/client_session.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/apm.hh>
#include <mongocxx/options/auto_encryption.hh>
#include <mongocxx/options/change_stream.hh>
#include <mongocxx/options/server_api.hh>
#include <mongocxx/options/tls.hh>
#include <mongocxx/read_concern.hh>
#include <mongocxx/read_preference.hh>
#include <mongocxx/scoped_bson.hh>
#include <mongocxx/uri.hh>
#include <mongocxx/write_concern.hh>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/ssl.hh>

namespace mongocxx {
namespace v_noabi {

using bsoncxx::v_noabi::builder::basic::kvp;

namespace {
class database_names {
   public:
    explicit database_names(char** names) : _names{names} {}

    ~database_names() {
        bson_strfreev(_names);
    }

    database_names(database_names&&) = delete;
    database_names& operator=(database_names&&) = delete;
    database_names(database_names const&) = delete;
    database_names& operator=(database_names const&) = delete;

    char const* operator[](std::size_t const i) const {
        return _names[i];
    }

    bool operator!() const {
        return _names == nullptr;
    }

   private:
    char** _names;
};
} // namespace

client::client() noexcept = default;

client::client(mongocxx::v_noabi::uri const& uri, options::client const& options) {
#if MONGOCXX_SSL_IS_ENABLED()
    if (options.tls_opts()) {
        if (!uri.tls())
            throw exception{error_code::k_invalid_parameter, "cannot set TLS options if 'tls=true' not in URI"};
    }
#else
    if (uri.tls() || options.tls_opts()) {
        throw exception{error_code::k_ssl_not_supported};
    }
#endif
    auto new_client = libmongoc::client_new_from_uri(v_noabi::uri::internal::as_mongoc(uri));
    if (!new_client) {
        // Shouldn't happen after checks above, but future libmongoc's may change behavior.
        throw exception{error_code::k_invalid_parameter, "could not construct client from URI"};
    }

    _impl = bsoncxx::make_unique<impl>(std::move(new_client));

    if (options.apm_opts()) {
        _impl->listeners = *options.apm_opts();
        auto callbacks = options::make_apm_callbacks(_impl->listeners);
        // We cast the APM class to a void* so we can pass it into libmongoc's context.
        // It will be cast back to an APM class in the event handlers.
        auto context = static_cast<void*>(&(_impl->listeners));
        libmongoc::client_set_apm_callbacks(_get_impl().client_t, callbacks.get(), context);
    }

    if (auto const& auto_encryption_opts = options.auto_encryption_opts()) {
        auto const opts = v_noabi::options::auto_encryption::internal::to_mongoc(*auto_encryption_opts);

        bson_error_t error;
        auto r = libmongoc::client_enable_auto_encryption(_get_impl().client_t, opts, &error);

        libmongoc::auto_encryption_opts_destroy(opts);

        if (!r) {
            throw_exception<operation_exception>(error);
        }
    }

    if (options.server_api_opts()) {
        auto const& server_api_opts = *options.server_api_opts();
        auto mongoc_server_api_opts = options::make_server_api(server_api_opts);

        bson_error_t error;
        auto result = libmongoc::client_set_server_api(_get_impl().client_t, mongoc_server_api_opts.get(), &error);

        if (!result) {
            throw_exception<operation_exception>(error);
        }
    }

#if MONGOCXX_SSL_IS_ENABLED()
    if (options.tls_opts()) {
        auto mongoc_opts = options::make_tls_opts(*options.tls_opts());
        _impl->tls_options = std::move(mongoc_opts.second);
        libmongoc::client_set_ssl_opts(_get_impl().client_t, &mongoc_opts.first);
    }
#endif
}

client::client(void* implementation)
    : _impl{bsoncxx::make_unique<impl>(static_cast<::mongoc_client_t*>(implementation))} {}

client::client(client&&) noexcept = default;
client& client::operator=(client&&) noexcept = default;

client::~client() = default;

client::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

void client::read_concern_deprecated(mongocxx::v_noabi::read_concern rc) {
    libmongoc::client_set_read_concern(_get_impl().client_t, v_noabi::read_concern::internal::as_mongoc(rc));
}

void client::read_concern(mongocxx::v_noabi::read_concern rc) {
    return read_concern_deprecated(std::move(rc));
}

mongocxx::v_noabi::read_concern client::read_concern() const {
    return v1::read_concern::internal::make(
        libmongoc::read_concern_copy(libmongoc::client_get_read_concern(_get_impl().client_t)));
}

void client::read_preference_deprecated(mongocxx::v_noabi::read_preference rp) {
    libmongoc::client_set_read_prefs(_get_impl().client_t, v_noabi::read_preference::internal::as_mongoc(rp));
}

void client::read_preference(mongocxx::v_noabi::read_preference rp) {
    return read_preference_deprecated(std::move(rp));
}

mongocxx::v_noabi::read_preference client::read_preference() const {
    return v1::read_preference::internal::make(
        libmongoc::read_prefs_copy(libmongoc::client_get_read_prefs(_get_impl().client_t)));
}

mongocxx::v_noabi::uri client::uri() const {
    return v1::uri::internal::make(libmongoc::uri_copy(libmongoc::client_get_uri(_get_impl().client_t)));
}

void client::write_concern_deprecated(mongocxx::v_noabi::write_concern wc) {
    libmongoc::client_set_write_concern(_get_impl().client_t, v_noabi::write_concern::internal::as_mongoc(wc));
}

void client::write_concern(mongocxx::v_noabi::write_concern wc) {
    return write_concern_deprecated(std::move(wc));
}

mongocxx::v_noabi::write_concern client::write_concern() const {
    return v1::write_concern::internal::make(
        libmongoc::write_concern_copy(libmongoc::client_get_write_concern(_get_impl().client_t)));
}

mongocxx::v_noabi::database client::database(bsoncxx::v_noabi::string::view_or_value name) const& {
    return mongocxx::v_noabi::database(*this, std::move(name));
}

cursor client::list_databases() const {
    return v1::cursor::internal::make(libmongoc::client_find_databases_with_opts(_get_impl().client_t, nullptr));
}

cursor client::list_databases(client_session const& session) const {
    bsoncxx::v_noabi::builder::basic::document options_doc;
    options_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session._get_impl().to_document()});
    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(_get_impl().client_t, to_scoped_bson_view(options_doc)));
}

cursor client::list_databases(bsoncxx::v_noabi::document::view_or_value const opts) const {
    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(_get_impl().client_t, to_scoped_bson_view(opts)));
}

cursor client::list_databases(client_session const& session, bsoncxx::v_noabi::document::view_or_value const opts)
    const {
    bsoncxx::v_noabi::builder::basic::document options_doc;
    options_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session._get_impl().to_document()});
    options_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{opts});
    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(_get_impl().client_t, to_scoped_bson_view(options_doc)));
}

std::vector<std::string> client::list_database_names(bsoncxx::v_noabi::document::view_or_value filter) const {
    bsoncxx::v_noabi::builder::basic::document options_builder;

    options_builder.append(kvp("filter", filter));

    bson_error_t error;

    database_names const names(
        libmongoc::client_get_database_names_with_opts(
            _get_impl().client_t, to_scoped_bson_view(options_builder), &error));

    if (!names) {
        throw_exception<operation_exception>(error);
    }

    std::vector<std::string> _names;
    for (std::size_t i = 0u; names[i]; ++i) {
        _names.emplace_back(names[i]);
    }

    return _names;
}

std::vector<std::string> client::list_database_names(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value const filter) const {
    bsoncxx::v_noabi::builder::basic::document options_builder;

    options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session._get_impl().to_document()});
    options_builder.append(kvp("filter", filter));

    bson_error_t error;

    database_names const names(
        libmongoc::client_get_database_names_with_opts(
            _get_impl().client_t, to_scoped_bson_view(options_builder), &error));

    if (!names) {
        throw_exception<operation_exception>(error);
    }

    std::vector<std::string> res;
    for (std::size_t i = 0u; names[i]; ++i) {
        res.emplace_back(names[i]);
    }

    return res;
}

mongocxx::v_noabi::client_session client::start_session(mongocxx::v_noabi::options::client_session const& options) {
    return client_session(this, options);
}

void client::reset() {
    libmongoc::client_reset(_get_impl().client_t);
}

change_stream client::watch(options::change_stream const& options) {
    return watch(pipeline{}, options);
}

change_stream client::watch(client_session const& session, options::change_stream const& options) {
    return _watch(&session, pipeline{}, options);
}

change_stream client::watch(pipeline const& pipe, options::change_stream const& options) {
    return _watch(nullptr, pipe, options);
}

change_stream
client::watch(client_session const& session, pipeline const& pipe, options::change_stream const& options) {
    return _watch(&session, pipe, options);
}

change_stream
client::_watch(client_session const* session, pipeline const& pipe, options::change_stream const& options) {
    bsoncxx::v_noabi::builder::basic::document container;
    container.append(bsoncxx::v_noabi::builder::basic::kvp("pipeline", pipe.view_array()));

    bsoncxx::v_noabi::builder::basic::document options_builder;
    options_builder.append(
        bsoncxx::v_noabi::builder::concatenate(v_noabi::options::change_stream::internal::to_document(options)));
    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    return v1::change_stream::internal::make(
        libmongoc::client_watch(
            _get_impl().client_t, to_scoped_bson_view(container), to_scoped_bson_view(options_builder)));
}

template <typename Self>
auto client::_get_impl(Self& self) -> decltype(*self._impl) {
    if (!self._impl) {
        throw logic_error{error_code::k_invalid_client_object};
    }
    return *self._impl;
}

client::impl const& client::_get_impl() const {
    return _get_impl(*this);
}

client::impl& client::_get_impl() {
    return _get_impl(*this);
}

} // namespace v_noabi
} // namespace mongocxx
