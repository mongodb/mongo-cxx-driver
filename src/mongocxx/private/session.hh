// Copyright 2017-present MongoDB Inc.
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

#include <bsoncxx/private/helpers.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/session.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class session::impl {
   public:
    impl(const class client* client, const options::session& session_options)
        : options(session_options), session_t(nullptr, nullptr), client(client) {
        // Create a mongoc_session_opts_t from session_options.
        std::unique_ptr<mongoc_session_opt_t, decltype(libmongoc::session_opts_destroy)> opt_t{
            libmongoc::session_opts_new(), libmongoc::session_opts_destroy};

        libmongoc::session_opts_set_causal_consistency(opt_t.get(), options.causal_consistency());

        bson_error_t error;
        auto s = libmongoc::client_start_session(client->_get_impl().client_t, opt_t.get(), &error);
        if (!s) {
            throw_exception<exception>(error);
        }

        session_t = unique_session{
            s, [](mongoc_client_session_t* cs) { libmongoc::client_session_destroy(cs); }};
    }

    // Get session id, also known as "logical session id" or "lsid".
    stdx::optional<bsoncxx::document::view> id() const {
        return bsoncxx::helpers::view_from_bson_t(
            libmongoc::client_session_get_lsid(session_t.get()));
    }

    bool has_ended() {
        return session_t == nullptr;
    }

    void end_session() {
        session_t = nullptr;
    }

    using unique_session =
        std::unique_ptr<mongoc_client_session_t,
                        std::function<void MONGOCXX_CALL(mongoc_client_session_t*)>>;

    unique_session session_t;
    const class client* client;
    options::session options;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
