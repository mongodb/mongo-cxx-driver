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

#include <mongocxx/v1/pool-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/database-fwd.hpp>
#include <mongocxx/v1/uri-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/client.hpp> // IWYU pragma: export
#include <mongocxx/v1/config/export.hpp>

#include <cstddef>
#include <system_error>

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
   private:
    class impl;
    void* _impl;

   public:
    class options;
    class entry;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all entries and clients associated with this pool.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~pool();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() pool(pool&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pool&) operator=(pool&& other) noexcept;

    ///
    /// This class is immovable.
    ///
    pool(pool const& other) = delete;

    ///
    /// This class is immovable.
    ///
    pool& operator=(pool const& other) = delete;

    ///
    /// Initialize this pool with the given URI.
    ///
    /// @note No connection is attempted until a client object is acquired from this pool.
    ///
    /// @throws mongocxx::v1::exception if a client-side error is encountered.
    ///
    /// @{
    explicit MONGOCXX_ABI_EXPORT_CDECL() pool(v1::uri const& uri, options opts);

    explicit MONGOCXX_ABI_EXPORT_CDECL() pool(v1::uri const& uri);
    /// @}
    ///

    ///
    /// Initialize this pool with default URI options.
    ///
    /// @important No connection to the MongoDB server(s) is attempted until the first client object is acquired.
    ///
    /// @throws mongocxx::v1::exception if a client-side error is encountered.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() pool();

    ///
    /// Return a client object associated with this pool.
    ///
    /// This function blocks the current thread until a client object is available or "waitQueueTimeoutMS" is
    /// triggered.
    ///
    /// @note Connection to the MongoDB server(s) is attempted in a background "monitoring" thread when the first client
    /// object is acquired. Server-side errors will only be encountered during or after the first command is executed.
    ///
    /// @returns A handle to a client object connected to the MongoDB server(s).
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::pool::errc::wait_queue_timeout if a client object could
    /// not be acquired within "waitQueueTimeoutMS".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(entry) acquire();

    ///
    /// Return a client object associated with this pool.
    ///
    /// @note The first client object acquired from a pool blocks the current thread until the initial connection to the
    /// MongoDB server(s) is established.
    ///
    /// @note Connection to the MongoDB server(s) is attempted in a background "monitoring" thread when the first client
    /// object is acquired. Server-side errors will only be encountered during or after the first command is executed.
    ///
    /// @returns Empty when a client object is not immediately available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<entry>) try_acquire();

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::pool.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,               ///< Zero.
        wait_queue_timeout, ///< Failed to acquire a client object due to "waitQueueTimeoutMS".
    };

    ///
    /// The error category for @ref mongocxx::v1::pool::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }
};

///
/// Options for @ref mongocxx::v1::pool.
///
/// @attention This feature is experimental! It is not ready for use!
///
class pool::options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

    ///
    /// Initialize with client options to apply to a pool.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(v1::client::options opts);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `this->client_opts()` is default-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Return the current client options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::client::options) client_opts() const;
};

///
/// A handle to a client object owned by an associated pool.
///
/// @attention This feature is experimental! It is not ready for use!
///
class pool::entry {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object, releasing the managed client object back to the associated pool.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~entry();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() entry(entry&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(entry&) operator=(entry&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    entry(entry const& other) = delete;

    ///
    /// This class is not copyable.
    ///
    entry& operator=(entry const& other) = delete;

    ///
    /// Access the managed client.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client*) operator->();

    ///
    /// Access the managed client.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client&) operator*();

    ///
    /// Explicitly release the managed client object back to the associated pool.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(entry&) operator=(std::nullptr_t);

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Equivalent to `(*this)->database(name)`.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::database) database(bsoncxx::v1::stdx::string_view name);
    MONGOCXX_ABI_EXPORT_CDECL(v1::database) operator[](bsoncxx::v1::stdx::string_view name);
    /// @}
    ///
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::pool.
///
/// @par Includes
/// - @ref mongocxx/v1/client.hpp
///
