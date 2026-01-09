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

#include <mongocxx/client.hh>

//

#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/uri.hh>
#include <mongocxx/v1/write_concern.hh>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/change_stream.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/apm.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/client_session.hpp>
#include <mongocxx/pipeline.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/auto_encryption.hh>
#include <mongocxx/options/change_stream.hh>
#include <mongocxx/options/server_api.hh>
#include <mongocxx/options/tls.hh>
#include <mongocxx/read_concern.hh>
#include <mongocxx/read_preference.hh>
#include <mongocxx/scoped_bson.hh>
#include <mongocxx/uri.hh>
#include <mongocxx/write_concern.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/ssl.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

template <typename Client>
Client& check_moved_from(Client& client) {
    if (!client) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_client_object};
    }
    return client;
}

} // namespace

client::client(v_noabi::uri const& uri, options::client const& options) {
#if MONGOCXX_SSL_IS_ENABLED()
    if (options.tls_opts()) {
        if (!uri.tls())
            throw v_noabi::exception{
                v_noabi::error_code::k_invalid_parameter, "cannot set TLS options if 'tls=true' not in URI"};
    }
#else
    if (uri.tls() || options.tls_opts()) {
        throw v_noabi::exception{v_noabi::error_code::k_ssl_not_supported};
    }
#endif

    auto const ptr = libmongoc::client_new_from_uri(v_noabi::uri::internal::as_mongoc(uri));

    if (!ptr) {
        // Shouldn't happen after checks above, but future libmongoc's may change behavior.
        throw v_noabi::exception{v_noabi::error_code::k_invalid_parameter, "could not construct client from URI"};
    }

    _client = v1::client::internal::make(ptr); // Ownership transfer.

    if (auto& opts = options.apm_opts()) {
        v1::client::internal::set_apm(_client, v_noabi::to_v1(*opts));
    }

    if (auto const& opts = options.auto_encryption_opts()) {
        bson_error_t error = {};

        if (!libmongoc::client_enable_auto_encryption(
                ptr, v_noabi::options::auto_encryption::internal::to_mongoc(*opts).get(), &error)) {
            v_noabi::throw_exception<operation_exception>(error);
        }
    }

    if (auto const& opts = options.server_api_opts()) {
        bson_error_t error = {};

        if (!libmongoc::client_set_server_api(
                ptr, v_noabi::options::server_api::internal::to_mongoc(*opts).get(), &error)) {
            v_noabi::throw_exception<operation_exception>(error);
        }
    }

#if MONGOCXX_SSL_IS_ENABLED()
    if (auto const& opts = options.tls_opts()) {
        auto const v = v_noabi::options::tls::internal::to_mongoc(*opts);
        libmongoc::client_set_ssl_opts(ptr, &v.opt);
    }
#endif
}

void client::read_concern_deprecated(v_noabi::read_concern rc) {
    libmongoc::client_set_read_concern(
        v1::client::internal::as_mongoc(check_moved_from(_client)), v_noabi::read_concern::internal::as_mongoc(rc));
}

void client::read_concern(v_noabi::read_concern rc) {
    return read_concern_deprecated(std::move(rc));
}

v_noabi::read_concern client::read_concern() const {
    return v1::read_concern::internal::make(
        libmongoc::read_concern_copy(
            libmongoc::client_get_read_concern(v1::client::internal::as_mongoc(check_moved_from(_client)))));
}

void client::read_preference_deprecated(v_noabi::read_preference rp) {
    libmongoc::client_set_read_prefs(
        v1::client::internal::as_mongoc(check_moved_from(_client)), v_noabi::read_preference::internal::as_mongoc(rp));
}

void client::read_preference(v_noabi::read_preference rp) {
    return read_preference_deprecated(std::move(rp));
}

v_noabi::read_preference client::read_preference() const {
    return v1::read_preference::internal::make(
        libmongoc::read_prefs_copy(
            libmongoc::client_get_read_prefs(v1::client::internal::as_mongoc(check_moved_from(_client)))));
}

v_noabi::uri client::uri() const {
    return v1::uri::internal::make(
        libmongoc::uri_copy(libmongoc::client_get_uri(v1::client::internal::as_mongoc(check_moved_from(_client)))));
}

void client::write_concern_deprecated(v_noabi::write_concern wc) {
    libmongoc::client_set_write_concern(
        v1::client::internal::as_mongoc(check_moved_from(_client)), v_noabi::write_concern::internal::as_mongoc(wc));
}

void client::write_concern(v_noabi::write_concern wc) {
    return write_concern_deprecated(std::move(wc));
}

v_noabi::write_concern client::write_concern() const {
    return v1::write_concern::internal::make(
        libmongoc::write_concern_copy(
            libmongoc::client_get_write_concern(v1::client::internal::as_mongoc(check_moved_from(_client)))));
}

v_noabi::database client::database(bsoncxx::v_noabi::string::view_or_value name) const& {
    // Backward compatibility: `database()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    return v_noabi::database(v1::client::internal::as_mongoc(c), std::move(name));
}

cursor client::list_databases() const {
    // Backward compatibility: `list_databases()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(v1::client::internal::as_mongoc(c), nullptr));
}

cursor client::list_databases(client_session const& session) const {
    // Backward compatibility: `list_databases()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    bsoncxx::v_noabi::builder::basic::document options_doc;
    options_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session._get_impl().to_document()});

    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(
            v1::client::internal::as_mongoc(c), to_scoped_bson_view(options_doc)));
}

cursor client::list_databases(bsoncxx::v_noabi::document::view_or_value const opts) const {
    // Backward compatibility: `list_databases()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(v1::client::internal::as_mongoc(c), to_scoped_bson_view(opts)));
}

cursor client::list_databases(client_session const& session, bsoncxx::v_noabi::document::view_or_value const opts)
    const {
    // Backward compatibility: `list_databases()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    bsoncxx::v_noabi::builder::basic::document options_doc;
    options_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session._get_impl().to_document()});
    options_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{opts});
    return v1::cursor::internal::make(
        libmongoc::client_find_databases_with_opts(
            v1::client::internal::as_mongoc(c), to_scoped_bson_view(options_doc)));
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

    v_noabi::throw_exception<v_noabi::operation_exception>(error);
}

} // namespace

std::vector<std::string> client::list_database_names(bsoncxx::v_noabi::document::view_or_value filter) const {
    // Backward compatibility: `list_database_names()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    scoped_bson const options{BCON_NEW("filter", BCON_DOCUMENT(to_scoped_bson_view(filter).bson()))};

    return list_database_names_impl(v1::client::internal::as_mongoc(c), options.bson());
}

std::vector<std::string> client::list_database_names(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value const filter) const {
    // Backward compatibility: `list_databases()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v1::client&>(check_moved_from(_client));

    namespace builder = bsoncxx::v_noabi::builder::basic;

    scoped_bson options;

    options += to_scoped_bson(session._get_impl().to_document());
    options += scoped_bson{BCON_NEW("filter", BCON_DOCUMENT(to_scoped_bson_view(filter).bson()))};

    return list_database_names_impl(v1::client::internal::as_mongoc(c), options.bson());
}

v_noabi::client_session client::start_session(v_noabi::options::client_session const& options) {
    return client_session(this, options);
}

void client::reset() {
    libmongoc::client_reset(v1::client::internal::as_mongoc(check_moved_from(_client)));
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
            v1::client::internal::as_mongoc(check_moved_from(_client)),
            to_scoped_bson_view(container),
            to_scoped_bson_view(options_builder)));
}

void client::internal::disown(client& self) {
    v1::client::internal::disown(self._client);
}

mongoc_client_t* client::internal::as_mongoc(client& self) {
    return v1::client::internal::as_mongoc(self._client);
}

} // namespace v_noabi
} // namespace mongocxx
