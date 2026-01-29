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

#include <mongocxx/pool-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/pool.hpp> // IWYU pragma: export

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

#include <mongocxx/options/auto_encryption-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/options/pool.hpp>
#include <mongocxx/uri.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A pool of reusable client objects connected to the same MongoDB topology.
///
/// @important This class does NOT implement [Connection Monitoring and Pooling (MongoDB
/// Specifications)](https://specifications.readthedocs.io/en/latest/connection-monitoring-and-pooling/connection-monitoring-and-pooling/).
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
   private:
    v1::pool _pool;

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
    explicit MONGOCXX_ABI_EXPORT_CDECL() pool(
        v_noabi::uri const& mongodb_uri = {},
        v_noabi::options::pool const& options = {});

    ///
    /// Destroys a pool.
    ///
    ~pool() = default;

    pool(pool&&) = delete;
    pool& operator=(pool&&) = delete;

    pool(pool const&) = delete;
    pool& operator=(pool const&) = delete;

    ///
    /// An owning handle to a client obtained from a pool.
    ///
    /// Returns the client back to its original pool on destruction.
    ///
    /// @note The lifetime of any entry object must be a subset of the pool object
    ///  from which it was acquired.
    ///
    class entry {
       public:
        /// Access a member of the client instance.
        v_noabi::client* operator->() const& noexcept {
            return _client.get();
        }
        v_noabi::client* operator->() && = delete;

        /// Retrieve a reference to the client.
        v_noabi::client& operator*() const& noexcept {
            return *_client;
        }
        v_noabi::client& operator*() && = delete;

        /// Assign nullptr to this entry to release its client to the pool.
        entry& operator=(std::nullptr_t) noexcept {
            _client.reset();
            return *this;
        }

        /// Return true if this entry has a client acquired from the pool.
        explicit operator bool() const noexcept {
            return _client != nullptr;
        }

        // Allows the pool_entry["db_name"] syntax to be used to access a database within the
        // entry's underlying client.
        mongocxx::v_noabi::database operator[](bsoncxx::v_noabi::string::view_or_value name) const& {
            return (*_client)[name];
        }
        mongocxx::v_noabi::database operator[](bsoncxx::v_noabi::string::view_or_value name) && = delete;

        class internal;

       private:
        using ptr_type = std::unique_ptr<v_noabi::client, std::function<void MONGOCXX_ABI_CDECL(v_noabi::client*)>>;

        ptr_type _client;

        /* explicit(false) */ entry(ptr_type client) : _client{std::move(client)} {}
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

    class internal;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::pool.
///
/// @par Includes
/// - @ref mongocxx/v1/pool.hpp
///
