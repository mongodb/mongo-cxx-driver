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

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/oidc_callback_params-fwd.hpp>
#include <mongocxx/v1/oidc_credential-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <functional>

namespace mongocxx {
namespace v1 {

///
/// An OIDC credential callback function.
///
/// The function receives callback parameters and returns an OIDC credential.
/// If the function throws an exception, the exception is caught, an error is logged, and NULL is returned to libmongoc
/// to signal failure.
///
using oidc_callback = std::function<oidc_credential MONGOCXX_ABI_CDECL(oidc_callback_params const&)>;

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::oidc_callback.
///
