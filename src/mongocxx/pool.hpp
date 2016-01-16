// Copyright 2015 MongoDB Inc.
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

#include <functional>
#include <memory>

#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/options/ssl.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;

///
/// A pool of @c client objects associated with a MongoDB deployment.
///
/// For interoperability with other MongoDB drivers, the minimum and maximum number of connections
/// in the pool is configured using the 'minPoolSize' and 'maxPoolSize' connection string options.
///
/// @see http://docs.mongodb.org/manual/reference/connection-string/#connection-string-options
///
/// @remark When connecting to a replica set, it is @b much more efficient to use a pool as opposed
/// to manually constructing @c client objects. The pool will use a single background thread used
/// to monitor the topology of the replica set that is shared between all the client objects it
/// creates. Each standalone client, however, will start its own background thread, leading to many
/// redundant threads and network operations.
///
/// As a @c client is @b not thread safe, the @c pool should be preferred in multithreaded
/// programs as it can safely be shared across threads.
///
class MONGOCXX_API pool {
   public:
    ///
    /// Creates a pool associated with a connection string.
    ///
    /// @param mongodb_uri
    ///  A MongoDB URI representing the connection parameters
    /// @param ssl_options
    ///  Optional SSL options to use when connecting to the MongoDB deployment.
    pool(const uri& mongodb_uri = mongocxx::uri(),
         stdx::optional<options::ssl> ssl_options = stdx::nullopt);

    ///
    /// Destroys a pool.
    ///
    ~pool();

    ///
    /// An entry is a handle on a @c client object acquired via the pool.
    ///
    /// @note The lifetime of any entry object must be a subset of the pool object
    ///  from which it was acquired.
    ///
    using entry = std::unique_ptr<client, std::function<void(client*)>>;

    ///
    /// Acquires a client from the pool. The calling thread will block until a connection is
    /// available.
    ///
    entry acquire();

    ///
    /// Acquires a client from the pool. This method will return immediately, but may return a
    /// disengaged optional if a client is not available.
    ///
    stdx::optional<entry> try_acquire();

   private:
    MONGOCXX_PRIVATE void _release(client* client);

    class MONGOCXX_PRIVATE impl;
    const std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
