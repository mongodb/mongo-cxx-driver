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

#include <mongocxx/v1/pool.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/database.hpp>

#include <mongocxx/v1/apm.hh>
#include <mongocxx/v1/auto_encryption_options.hh>
#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/server_api.hh>
#include <mongocxx/v1/tls.hh>
#include <mongocxx/v1/uri.hh>

#include <cstddef>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/ssl.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

using code = pool::errc;

class pool::impl {
   public:
    mongoc_client_pool_t* _pool;
    v1::apm _apm;

    ~impl() {
        libmongoc::client_pool_destroy(_pool);
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;
    impl(impl const& other) = delete;
    impl& operator=(impl const& other) = delete;

    explicit impl(mongoc_uri_t const* uri)
        : _pool{[&] {
              bson_error_t error = {};

              if (auto const ptr = libmongoc::client_pool_new_with_error(uri, &error)) {
                  return ptr;
              }

              v1::throw_exception(error);
          }()} {}

    explicit impl(mongoc_client_pool_t* pool) : _pool{pool} {}

    static impl const& with(pool const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(pool const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(pool& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(pool* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

pool::~pool() {
    delete impl::with(this);
}

pool::pool(pool&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

pool& pool::operator=(pool&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

pool::pool(v1::uri const& uri, options opts) : pool{uri} {
    auto const _pool = impl::with(this)->_pool;

    auto& client_opts = options::internal::client_opts(opts);

    if (auto& opt = v1::client::options::internal::apm_opts(client_opts)) {
        internal::set_apm(*this, std::move(*opt));
    }

    if (auto const& opt = v1::client::options::internal::auto_encryption_opts(client_opts)) {
        bson_error_t error = {};

        if (!libmongoc::client_pool_enable_auto_encryption(
                _pool, v1::auto_encryption_options::internal::to_mongoc(*opt).get(), &error)) {
            v1::throw_exception(error);
        }
    }

    if (auto const& opt = v1::client::options::internal::server_api_opts(client_opts)) {
        bson_error_t error = {};

        if (!libmongoc::client_pool_set_server_api(_pool, v1::server_api::internal::to_mongoc(*opt).get(), &error)) {
            v1::throw_exception(error);
        }
    }

    {
        auto const tls_enabled = uri.tls();

#if MONGOCXX_SSL_IS_ENABLED()
        if (auto const& opt = v1::client::options::internal::tls_opts(client_opts)) {
            if (!tls_enabled) {
                throw v1::exception::internal::make(v1::client::errc::tls_not_enabled);
            }

            auto const v = v1::tls::internal::to_mongoc(*opt);
            libmongoc::client_pool_set_ssl_opts(_pool, &v);
        }
#else
        if (tls_enabled || v1::client::options::internal::tls_opts(client_opts)) {
            throw v1::exception::internal::make(v1::client::errc::tls_not_supported);
        }
#endif
    }
}

pool::pool(v1::uri const& uri) : _impl{new impl{v1::uri::internal::as_mongoc(uri)}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

pool::pool() : pool{v1::uri{}} {}

pool::operator bool() const {
    return _impl != nullptr;
}

pool::entry pool::acquire() {
    auto const _pool = impl::with(this)->_pool;

    if (auto const ptr = libmongoc::client_pool_pop(_pool)) {
        return entry::internal::make(_pool, ptr);
    }

    throw v1::exception::internal::make(code::wait_queue_timeout);
}

bsoncxx::v1::stdx::optional<pool::entry> pool::try_acquire() {
    auto const _pool = impl::with(this)->_pool;

    if (auto const ptr = libmongoc::client_pool_try_pop(_pool)) {
        return entry::internal::make(_pool, ptr);
    }

    return {};
}

std::error_category const& pool::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::pool";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::wait_queue_timeout:
                    return "failed to acquire a client object due to waitQueueTimeoutMS";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::wait_queue_timeout:
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
                    case code::wait_queue_timeout:
                        return type == condition::runtime_error;

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

pool::pool(void* impl) : _impl{impl} {}

class pool::options::impl {
   public:
    v1::client::options _client_opts;

    explicit impl(v1::client::options client_opts) : _client_opts{std::move(client_opts)} {}

    impl() = default;

    static impl const& with(options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

pool::options::~options() {
    delete impl::with(_impl);
}

pool::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

pool::options& pool::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

pool::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

pool::options& pool::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

pool::options::options(v1::client::options opts) : _impl{new impl{std::move(opts)}} {}

pool::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::client::options pool::options::client_opts() const {
    return impl::with(this)->_client_opts;
}

class pool::entry::impl {
   public:
    mongoc_client_pool_t* _pool;
    v1::client _client;

    ~impl() {
        libmongoc::client_pool_push(_pool, v1::client::internal::release(_client));
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;
    impl(impl const& other) = delete;
    impl& operator=(impl const& other) = delete;

    impl(mongoc_client_pool_t* pool, v1::client client) : _pool{pool}, _client{std::move(client)} {}

    static impl const& with(entry const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(entry const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(entry& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(entry* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

pool::entry::~entry() {
    delete impl::with(_impl);
    _impl = nullptr; // scan-build: warning: Use of memory after it is freed [cplusplus.NewDelete]
}

pool::entry::entry(entry&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

pool::entry& pool::entry::entry::operator=(entry&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

pool::entry::entry(void* impl) : _impl{impl} {}

v1::client* pool::entry::operator->() {
    return &impl::with(this)->_client;
}

v1::client& pool::entry::operator*() {
    return impl::with(this)->_client;
}

pool::entry& pool::entry::operator=(std::nullptr_t) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): owning void* for ABI stability.
    delete impl::with(exchange(_impl, nullptr));
    return *this;
}

pool::entry::operator bool() const {
    return _impl != nullptr;
}

v1::database pool::entry::database(bsoncxx::v1::stdx::string_view name) {
    return impl::with(this)->_client.database(name);
}

v1::database pool::entry::operator[](bsoncxx::v1::stdx::string_view name) {
    return this->database(name);
}

pool pool::internal::make(mongoc_client_pool_t* ptr) {
    return {new impl{ptr}};
}

mongoc_client_pool_t* pool::internal::as_mongoc(pool& self) {
    return impl::with(self)._pool;
}

void pool::internal::set_apm(pool& self, v1::apm v) {
    auto& _apm = impl::with(self)._apm;
    _apm = std::move(v);
    v1::apm::internal::set_apm_callbacks(impl::with(self)._pool, _apm);
}

v1::client::options& pool::options::internal::client_opts(options& self) {
    return impl::with(self)._client_opts;
}

pool::entry pool::entry::internal::make(mongoc_client_pool_t* pool, mongoc_client_t* client) {
    return {new impl{pool, v1::client::internal::make(client)}};
}

} // namespace v1
} // namespace mongocxx
