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

#include <mongocxx/v1/oidc_callback.hh>

//

#include <mongocxx/v1/oidc_callback_params.hh>
#include <mongocxx/v1/oidc_credential.hh>

#include <exception>

#include <mongocxx/private/mongoc.hh>

#include <mongoc/mongoc.h>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_oidc_credential_t* MONGOC_CALL oidc_callback_trampoline(mongoc_oidc_callback_params_t* params) noexcept {
    auto const& fn = *static_cast<oidc_callback*>(libmongoc::oidc_callback_params_get_user_data(params));
    auto const cpp_params = oidc_callback_params::internal::make(params);

    try {
        return oidc_credential::internal::to_mongoc(fn(cpp_params));
    } catch (std::exception const& e) {
        MONGOC_ERROR("OIDC callback exited via exception: %s", e.what());
        return nullptr;
    } catch (...) {
        MONGOC_ERROR("OIDC callback exited via unknown exception");
        return nullptr;
    }
}

} // namespace

void set_oidc_callback(mongoc_client_t* client, v1::oidc_callback& fn) {
    auto* mc = libmongoc::oidc_callback_new_with_user_data(oidc_callback_trampoline, &fn);
    libmongoc::client_set_oidc_callback(client, mc);
    libmongoc::oidc_callback_destroy(mc);
}

void set_oidc_callback(mongoc_client_pool_t* pool, v1::oidc_callback& fn) {
    auto* mc = libmongoc::oidc_callback_new_with_user_data(oidc_callback_trampoline, &fn);
    libmongoc::client_pool_set_oidc_callback(pool, mc);
    libmongoc::oidc_callback_destroy(mc);
}

} // namespace v1
} // namespace mongocxx
