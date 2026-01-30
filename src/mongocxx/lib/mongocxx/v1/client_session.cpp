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

#include <mongocxx/v1/client_session.hh>

//

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/transaction_options.hh>

#include <cstdint>
#include <exception>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class client_session::impl {
   public:
    mongoc_client_session_t* _session = nullptr;
    v1::client* _client = nullptr;

    ~impl() {
        libmongoc::client_session_destroy(_session);
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;
    impl(impl const& other) = delete;
    impl& operator=(impl const& other) = delete;

    impl(mongoc_client_session_t* session, v1::client& client) : _session{session}, _client{&client} {}

    static impl const& with(client_session const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(client_session const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(client_session& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(client_session* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

client_session::~client_session() {
    delete impl::with(this);
}

client_session::client_session(client_session&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_session& client_session::operator=(client_session&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::client const& client_session::client() const {
    return *impl::with(this)->_client;
}

client_session::options client_session::opts() const {
    return options::internal::make(
        libmongoc::session_opts_clone(libmongoc::client_session_get_opts(impl::with(this)->_session)));
}

bsoncxx::v1::document::view client_session::id() const {
    return scoped_bson_view{libmongoc::client_session_get_lsid(impl::with(this)->_session)}.view();
}

bsoncxx::v1::document::view client_session::cluster_time() const {
    if (auto const ptr = libmongoc::client_session_get_cluster_time(impl::with(this)->_session)) {
        return scoped_bson_view{ptr}.view();
    }

    return {};
}

bsoncxx::v1::types::b_timestamp client_session::operation_time() const {
    bsoncxx::v1::types::b_timestamp ret = {};
    libmongoc::client_session_get_operation_time(impl::with(this)->_session, &ret.timestamp, &ret.increment);
    return ret;
}

std::uint32_t client_session::server_id() const {
    return libmongoc::client_session_get_server_id(impl::with(this)->_session);
}

client_session::transaction_state client_session::get_transaction_state() const {
    static_assert(MONGOC_TRANSACTION_NONE == static_cast<int>(transaction_state::k_transaction_none), "");
    static_assert(MONGOC_TRANSACTION_STARTING == static_cast<int>(transaction_state::k_transaction_starting), "");
    static_assert(MONGOC_TRANSACTION_IN_PROGRESS == static_cast<int>(transaction_state::k_transaction_in_progress), "");
    static_assert(MONGOC_TRANSACTION_COMMITTED == static_cast<int>(transaction_state::k_transaction_committed), "");
    static_assert(MONGOC_TRANSACTION_ABORTED == static_cast<int>(transaction_state::k_transaction_aborted), "");

    auto const v = libmongoc::client_session_get_transaction_state(impl::with(this)->_session);

    switch (v) {
        case MONGOC_TRANSACTION_NONE:
        case MONGOC_TRANSACTION_STARTING:
        case MONGOC_TRANSACTION_IN_PROGRESS:
        case MONGOC_TRANSACTION_COMMITTED:
        case MONGOC_TRANSACTION_ABORTED:
            return static_cast<transaction_state>(v);

        default:
            MONGOCXX_PRIVATE_UNREACHABLE;
    }
}

bool client_session::get_dirty() const {
    return libmongoc::client_session_get_dirty(impl::with(this)->_session);
}

void client_session::advance_cluster_time(bsoncxx::v1::document::view v) {
    libmongoc::client_session_advance_cluster_time(impl::with(this)->_session, scoped_bson_view{v}.bson());
}

void client_session::advance_operation_time(bsoncxx::v1::types::b_timestamp v) {
    libmongoc::client_session_advance_operation_time(impl::with(this)->_session, v.timestamp, v.increment);
}

void client_session::start_transaction(v1::transaction_options const& opts) {
    bson_error_t error = {};

    if (!libmongoc::client_session_start_transaction(
            impl::with(this)->_session, v1::transaction_options::internal::as_mongoc(opts), &error)) {
        v1::throw_exception(error);
    }
}

void client_session::start_transaction() {
    bson_error_t error = {};

    if (!libmongoc::client_session_start_transaction(impl::with(this)->_session, nullptr, &error)) {
        v1::throw_exception(error);
    }
}

void client_session::commit_transaction() {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::client_session_commit_transaction(impl::with(this)->_session, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }
}

void client_session::abort_transaction() {
    bson_error_t error = {};

    if (!libmongoc::client_session_abort_transaction(impl::with(this)->_session, &error)) {
        v1::throw_exception(error);
    }
}

namespace {

struct with_transaction_ctx {
    v1::client_session* self_ptr;
    client_session::with_transaction_cb const* fn;
    std::exception_ptr eptr;
};

bool with_transaction_cb_impl(mongoc_client_session_t*, void* ctx, bson_t** reply, bson_error_t* error) noexcept {
    auto const cb_ctx = static_cast<with_transaction_ctx*>(ctx);

    try {
        (*cb_ctx->fn)(*cb_ctx->self_ptr);
        return true;
    } catch (v1::exception const& ex) {
        cb_ctx->eptr = std::current_exception();

        // Unused by mongoc, but set anyways for consistency.
        bson_set_error(error, 0u, static_cast<std::uint32_t>(ex.code().value()), "%s", ex.what());

        // `mongoc_client_session_with_transaction` primarily implements its behavior using the `reply` document.
        if (auto const ptr = dynamic_cast<v1::server_error const*>(&ex)) {
            *reply = scoped_bson_view{ptr->raw()}.copy();
        } else if (auto const& reply_opt = v1::exception::internal::get_reply(ex)) {
            *reply = scoped_bson_view{*reply_opt}.copy();
        }

        return false;
    } catch (...) {
        cb_ctx->eptr = std::current_exception();

        // Unused by mongoc, but set anyways for consistency.
        bson_set_error(error, 0u, 0u, "unknown error"); // CDRIVER-3524

        return false;
    }
}

void with_transaction_impl(
    v1::client_session& self,
    client_session::with_transaction_cb const& fn,
    mongoc_transaction_opt_t const* opts) {
    with_transaction_ctx ctx{&self, &fn, nullptr};

    scoped_bson reply;
    bson_error_t error = {};

    if (libmongoc::client_session_with_transaction(
            client_session::internal::as_mongoc(self),
            &with_transaction_cb_impl,
            opts,
            &ctx,
            reply.out_ptr(),
            &error)) {
        return;
    }

    if (ctx.eptr) {
        std::rethrow_exception(ctx.eptr);
    }

    if (reply.view().empty()) {
        v1::throw_exception(error);
    }

    v1::throw_exception(error, std::move(reply).value());
}

} // namespace

void client_session::with_transaction(with_transaction_cb const& fn, v1::transaction_options const& opts) {
    with_transaction_impl(*this, fn, v1::transaction_options::internal::as_mongoc(opts));
}

void client_session::with_transaction(with_transaction_cb const& fn) {
    with_transaction_impl(*this, fn, nullptr);
}

client_session::client_session(void* impl) : _impl{impl} {}

client_session client_session::internal::make(mongoc_client_session_t* session, v1::client& client) {
    return {new impl{session, client}};
}

mongoc_client_session_t const* client_session::internal::as_mongoc(client_session const& self) {
    return impl::with(self)._session;
}

mongoc_client_session_t* client_session::internal::as_mongoc(client_session& self) {
    return impl::with(self)._session;
}

void client_session::internal::append_to(client_session const& self, scoped_bson& out) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!libmongoc::client_session_append(impl::with(self)._session, doc.out_ptr(), &error)) {
        v1::throw_exception(error);
    }

    out += doc;
}

namespace {

mongoc_session_opt_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_session_opt_t*>(ptr);
}

} // namespace

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

client_session::options::~options() {
    libmongoc::session_opts_destroy(to_mongoc(_impl));
}

client_session::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_session::options& client_session::options::operator=(options&& other) noexcept {
    if (this != &other) {
        libmongoc::session_opts_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }

    return *this;
}

client_session::options::options(options const& other) : _impl{libmongoc::session_opts_clone(to_mongoc(other._impl))} {}

client_session::options& client_session::options::operator=(options const& other) {
    if (this != &other) {
        libmongoc::session_opts_destroy(
            to_mongoc(exchange(_impl, libmongoc::session_opts_clone(to_mongoc(other._impl)))));
    }

    return *this;
}

client_session::options::options() : _impl{libmongoc::session_opts_new()} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

client_session::options& client_session::options::causal_consistency(bool v) {
    libmongoc::session_opts_set_causal_consistency(to_mongoc(_impl), v);
    return *this;
}

bool client_session::options::causal_consistency() const {
    return libmongoc::session_opts_get_causal_consistency(to_mongoc(_impl));
}

client_session::options& client_session::options::snapshot(bool v) {
    libmongoc::session_opts_set_snapshot(to_mongoc(_impl), v);
    return *this;
}

bool client_session::options::snapshot() const {
    return libmongoc::session_opts_get_snapshot(to_mongoc(_impl));
}

client_session::options& client_session::options::default_transaction_opts(v1::transaction_options const& v) {
    libmongoc::session_opts_set_default_transaction_opts(
        to_mongoc(_impl), v1::transaction_options::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::transaction_options> client_session::options::default_transaction_opts() const {
    if (auto const ptr = libmongoc::session_opts_get_default_transaction_opts(to_mongoc(_impl))) {
        return v1::transaction_options::internal::make(libmongoc::transaction_opts_clone(ptr));
    }

    return {}; // Never null?
}

client_session::options::options(void* impl) : _impl{impl} {}

client_session::options client_session::options::internal::make(mongoc_session_opt_t* impl) {
    return {impl};
}

mongoc_session_opt_t const* client_session::options::internal::as_mongoc(options const& self) {
    return to_mongoc(self._impl);
}

} // namespace v1
} // namespace mongocxx
