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

#include <mongocxx/client_session.hh>

//

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/client_session.hh>

#include <cstdint>
#include <exception>
#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/helpers.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/client_session.hpp>

#include <mongocxx/client.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/transaction.hh>
#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

client_session::client_session(
    v1::client_session session,
    v_noabi::client& client,
    v_noabi::options::client_session options)
    : _session{std::move(session)}, _client{&client}, _options{std::move(options)} {}

void client_session::start_transaction(
    bsoncxx::v_noabi::stdx::optional<options::transaction> const& transaction_opts) try {
    transaction_opts ? _session.start_transaction(v_noabi::options::transaction::internal::as_v1(*transaction_opts))
                     : _session.start_transaction();
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

void client_session::commit_transaction() try { _session.commit_transaction(); } catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

void client_session::abort_transaction() try { _session.abort_transaction(); } catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

namespace {

struct with_transaction_ctx {
    client_session* self_ptr;
    client_session::with_transaction_cb cb;
    std::exception_ptr eptr;
};

// The callback we pass into libmongoc is a wrapped version of the
// user callback. Before giving control back to libmongoc, we convert
// any exception the user callback emits into an error_t and reply object;
// libmongoc uses these to determine whether to retry.
bool with_transaction_impl(mongoc_client_session_t*, void* ctx_vp, bson_t** reply, bson_error_t* error) noexcept {
    auto const ctx = static_cast<with_transaction_ctx*>(ctx_vp);

    try {
        ctx->cb(ctx->self_ptr);
        return true;
    } catch (v_noabi::operation_exception const& ex) {
        bson_set_error(error, 0u, static_cast<std::uint32_t>(ex.code().value()), "%s", ex.what());

        if (auto const& raw = ex.raw_server_error()) {
            *reply = to_scoped_bson_view(*raw).copy();
        }

        return false;
    } catch (v1::exception const& ex) {
        bson_set_error(error, 0u, static_cast<std::uint32_t>(ex.code().value()), "%s", ex.what());

        if (auto const ptr = dynamic_cast<v1::server_error const*>(&ex)) {
            *reply = scoped_bson_view{ptr->raw()}.copy();
        } else if (auto const& reply_opt = v1::exception::internal::get_reply(ex)) {
            *reply = scoped_bson_view{*reply_opt}.copy();
        }

        return false;
    } catch (...) {
        ctx->eptr = std::current_exception();
        bson_set_error(error, 0u, 0u, "unknown error"); // CDRIVER-3524
        return false;
    }
}

} // namespace

void client_session::with_transaction(with_transaction_cb cb, v_noabi::options::transaction opts) try {
    with_transaction_ctx ctx{this, std::move(cb), nullptr};

    bson_error_t error = {};

    scoped_bson reply;

    if (!libmongoc::client_session_with_transaction(
            v1::client_session::internal::as_mongoc(_session),
            &with_transaction_impl,
            v_noabi::options::transaction::internal::as_mongoc(opts),
            &ctx,
            reply.out_ptr(),
            &error)) {
        if (ctx.eptr) {
            std::rethrow_exception(ctx.eptr);
        }

        v_noabi::throw_exception<v_noabi::operation_exception>(
            bsoncxx::v_noabi::from_v1(std::move(reply).value()), error);
    }
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

client_session client_session::internal::make(
    mongoc_client_session_t* session,
    v_noabi::client& client,
    v_noabi::options::client_session opts) {
    return {
        v1::client_session::internal::make(session, v_noabi::client::internal::as_v1(client)), client, std::move(opts)};
}

mongoc_client_session_t const* client_session::internal::as_mongoc(client_session const& self) {
    return v1::client_session::internal::as_mongoc(self._session);
}

void client_session::internal::append_to(client_session const& self, scoped_bson& out) {
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(self._session, out, error)) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_session, error.message};
    }
}

void client_session::internal::append_to(client_session const& self, bsoncxx::builder::basic::document& builder) {
    scoped_bson doc;
    bson_error_t error = {};

    if (!v1::client_session::internal::append_to(self._session, doc, error)) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_session, error.message};
    }

    builder.append(bsoncxx::builder::basic::concatenate(bsoncxx::v_noabi::from_v1(doc.view())));
}

} // namespace v_noabi
} // namespace mongocxx
