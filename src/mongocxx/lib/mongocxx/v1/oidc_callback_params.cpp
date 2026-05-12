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

#include <mongocxx/v1/oidc_callback_params.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <chrono>
#include <cstdint>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_oidc_callback_params_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_oidc_callback_params_t const*>(ptr);
}

} // namespace

oidc_callback_params::oidc_callback_params(void const* impl) : _impl{impl} {}

std::int32_t oidc_callback_params::version() const {
    return libmongoc::oidc_callback_params_get_version(to_mongoc(_impl));
}

bsoncxx::v1::stdx::optional<std::chrono::steady_clock::time_point> oidc_callback_params::timeout() const {
    auto const* p = libmongoc::oidc_callback_params_get_timeout(to_mongoc(_impl));
    // mongoc_oidc_callback_params_get_timeout returns a value comparable to bson_get_monotonic_time.
    if (!p) {
        return bsoncxx::v1::stdx::nullopt;
    }
    auto duration = *p - bson_get_monotonic_time();
    return std::chrono::steady_clock::now() + std::chrono::microseconds(duration);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> oidc_callback_params::username() const {
    auto const* s = libmongoc::oidc_callback_params_get_username(to_mongoc(_impl));
    if (s) {
        return bsoncxx::v1::stdx::string_view{s};
    }
    return bsoncxx::v1::stdx::nullopt;
}

oidc_callback_params oidc_callback_params::internal::make(mongoc_oidc_callback_params_t const* ptr) {
    return oidc_callback_params{ptr};
}

} // namespace v1
} // namespace mongocxx
