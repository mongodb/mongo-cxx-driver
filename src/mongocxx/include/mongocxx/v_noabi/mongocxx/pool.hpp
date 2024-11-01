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

#include <functional>
#include <memory>

#include <mongocxx/options/auto_encryption-fwd.hpp>
#include <mongocxx/pool-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/options/pool.hpp>
#include <mongocxx/uri.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A pool of @c client objects associated with a MongoDB deployment.
///
/// For interoperability with other MongoDB drivers, the minimum and maximum number of connections
/// in the pool is configured using the 'minPoolSize' and 'maxPoolSize' connection string options.
///
/// @see
/// - https://www.mongodb.com/docs/manual/reference/connection-string/#connection-string-options
///
/// @remark When connecting to a replica set, it is @b much more efficient to use a pool as opposed
/// to manually constructing @c client objects. The pool will use a single background thread per
/// server to monitor the topology of the replica set, all of which are shared between the client
/// objects created by the pool. A standalone client will instead "stop the world" every 60 seconds
/// to check the status of the cluster. Because of this, if multiple threads are available, a
/// connection pool should be used even if the application itself is single-threaded.
///
class pool {
   public:
    ///
    /// Creates a pool associated with a connection string.
    ///
    /// @param mongodb_uri
    ///  A MongoDB URI representing the connection parameters
    /// @param options
    ///  Options to use when connecting to the MongoDB deployment.
    ///
    /// @throws mongocxx::v_noabi::exception if invalid options are provided (whether from the URI
    /// or
    ///  provided client options).
    explicit MONGOCXX_ABI_EXPORT_CDECL() pool(const uri& mongodb_uri = mongocxx::v_noabi::uri(),
                                              const options::pool& options = options::pool());

    ///
    /// Destroys a pool.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~pool();

    pool(pool&&) = delete;
    pool& operator=(pool&&) = delete;

    pool(const pool&) = delete;
    pool& operator=(const pool&) = delete;

    ///
    /// An entry is a handle on a @c client object acquired via the pool. Similar to
    /// std::unique_ptr.
    ///
    /// @note The lifetime of any entry object must be a subset of the pool object
    ///  from which it was acquired.
    ///
    class entry {
       public:
        /// Access a member of the client instance.
        MONGOCXX_ABI_EXPORT_CDECL(client*) operator->() const& noexcept;
        client* operator->() && = delete;

        /// Retrieve a reference to the client.
        MONGOCXX_ABI_EXPORT_CDECL(client&) operator*() const& noexcept;
        client& operator*() && = delete;

        /// Assign nullptr to this entry to release its client to the pool.
        MONGOCXX_ABI_EXPORT_CDECL(entry&) operator=(std::nullptr_t) noexcept;

        /// Return true if this entry has a client acquired from the pool.
        explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const noexcept;

        // Allows the pool_entry["db_name"] syntax to be used to access a database within the
        // entry's underlying client.
        mongocxx::v_noabi::database operator[](
            bsoncxx::v_noabi::string::view_or_value name) const& {
            return (**this)[name];
        }

        mongocxx::v_noabi::database operator[](bsoncxx::v_noabi::string::view_or_value name) && =
            delete;

       private:
        friend ::mongocxx::v_noabi::pool;

        using unique_client =
            std::unique_ptr<client, std::function<void MONGOCXX_ABI_CDECL(client*)>>;

        explicit entry(unique_client);

        unique_client _client;
    };

    ///
    /// Acquires a client from the pool. The calling thread will block until a connection is
    /// available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(entry) acquire();

    ///
    /// Acquires a client from the pool. This method will return immediately, but may return a
    /// disengaged optional if a client is not available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<entry>) try_acquire();

   private:
    friend ::mongocxx::v_noabi::options::auto_encryption;

    void _release(client* client);

    class impl;
    const std::unique_ptr<impl> _impl;
};

}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::pool.
///
