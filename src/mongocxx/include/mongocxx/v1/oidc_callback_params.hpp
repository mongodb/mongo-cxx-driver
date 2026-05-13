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

#pragma once

#include <mongocxx/v1/oidc_callback_params-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <cstdint>

namespace mongocxx {
namespace v1 {

///
/// Parameters passed to an OIDC callback.
///
/// @note This type is not user-constructible. The lifetime of associated views is limited to the scope of the OIDC
/// callback function to which this argument is passed.
///
class oidc_callback_params {
    void const* _impl; // mongoc_oidc_callback_params_t const

   public:
    ///
    /// Return the callback API version.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) version() const;

    ///
    /// Return the optional deadline, or nullopt for infinite timeout.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::steady_clock::time_point>) timeout() const;

    ///
    /// Return the optional username, or nullopt if not provided.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) username() const;

    class internal;

   private:
    /* explicit(false) */ oidc_callback_params(void const* impl);
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::oidc_callback_params.
///
