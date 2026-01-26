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

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/apm.hh>
#include <mongocxx/v1/auto_encryption_options.hh>
#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/database.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/pipeline.hh>
#include <mongocxx/v1/server_api.hh>
#include <mongocxx/v1/tls.hh>
#include <mongocxx/v1/uri.hh>

#include <memory>
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

        if (!libmongoc::client_enable_auto_encryption(
                _client, v1::auto_encryption_options::internal::to_mongoc(*opt).get(), &error)) {
            v1::throw_exception(error);
        }
    }

    if (auto const& opt = options::internal::server_api_opts(opts)) {
        bson_error_t error = {};

        if (!libmongoc::client_set_server_api(_client, v1::server_api::internal::to_mongoc(*opt).get(), &error)) {
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

            auto const v = v1::tls::internal::to_mongoc(*opt);
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
    v1::apm::internal::set_apm_callbacks(impl::with(self)._client, _apm);
}

mongoc_client_t* client::internal::release(client& self) {
    return exchange(impl::with(self)._client, nullptr);
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
