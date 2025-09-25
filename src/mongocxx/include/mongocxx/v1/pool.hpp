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

#include <mongocxx/v1/pool-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

namespace mongocxx {
namespace v1 {

///
/// A thread-safe pool of client objects.
///
/// @important This interface does NOT fully conform to the CMAP specification!
///
/// @see
/// - [`mongoc_client_pool_t` (mongoc)](https://mongoc.org/libmongoc/current/mongoc_client_pool_t.html)
/// - [Connection Monitoring and Pooling (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/connection-monitoring-and-pooling/connection-monitoring-and-pooling/)
/// - [Connection Pool Overview (MongoDB Manual)](https://www.mongodb.com/docs/manual/administration/connection-pool-overview/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class pool {
    ///
    /// Options for @ref mongocxx::v1::pool.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class options {};
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::pool.
///
