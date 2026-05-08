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

#include <mongocxx/client_bulk_write-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/client_bulk_write.hpp> // IWYU pragma: export

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

using client_bulk_write = v1::client_bulk_write;

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {

using v_noabi::client_bulk_write;

} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::client_bulk_write.
///
/// @par Includes
/// - @ref mongocxx/v1/client_bulk_write.hpp
///
