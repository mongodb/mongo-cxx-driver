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

#include <mongocxx/v1/client_session-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/client-fwd.hpp>
#include <mongocxx/v1/transaction-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <functional>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB client session.
///
/// @see
/// - [Read Isolation, Consistency, and Recency (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/read-isolation-consistency-recency/)
/// - [Server Sessions (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/server-sessions/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_session {
   private:
    class impl;
    void* _impl;

   public:
    class options;

    ///
    /// The state of an associated transaction.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class transaction_state {
        ///
        /// No associated transaction.
        ///
        k_transaction_none,

        ///
        /// An associated transaction has started.
        ///
        k_transaction_starting,

        ///
        /// The associated transaction is in progress.
        ///
        k_transaction_in_progress,

        ///
        /// The associated transaction was committed.
        ///
        k_transaction_committed,

        ///
        /// The associated transaction was aborted.
        ///
        k_transaction_aborted,
    };

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views and references.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~client_session();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client_session(client_session&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_session&) operator=(client_session&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    client_session(client_session const&) = delete;

    ///
    /// This class is not copyable.
    ///
    client_session& operator=(client_session const&) = delete;

    ///
    /// Return the associated client object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::client const&) client() const;

    ///
    /// Return the options used to create this client session.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options) opts() const;

    ///
    /// Return the current "lsid" (logical session ID).
    ///
    /// @returns Empty when one is not available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) id() const;

    ///
    /// Return this session's current "clusterTime" value.
    ///
    /// @returns Empty when not available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) cluster_time() const;

    ///
    /// Return this session's current "operationTime" value.
    ///
    /// @returns Default-initialized when not available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::b_timestamp) operation_time() const;

    ///
    /// Return the ID of the current pinned server.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::uint32_t) server_id() const;

    ///
    /// Return the current transaction state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction_state) get_transaction_state() const;

    ///
    /// Return true when the current session is dirty.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) get_dirty() const;

    ///
    /// Advance the "clusterTime" value for this session.
    ///
    /// @param v A "clusterTime" value obtained by a prior call to @ref cluster_time.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) advance_cluster_time(bsoncxx::v1::document::view v);

    ///
    /// Advance the "operationTime" value for this session.
    ///
    /// @param v An "operationTime" value obtained by a prior call to @ref operation_time.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) advance_operation_time(bsoncxx::v1::types::b_timestamp v);

    ///
    /// Start a new transaction using `opts` instead of default transaction options.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) start_transaction(v1::transaction opts);

    ///
    /// Start a new transaction.
    ///
    /// Default transaction options are applied to the new transaction.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) start_transaction();

    ///
    /// Commit the current transaction.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) commit_transaction();

    ///
    /// Abort the current transaction.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) abort_transaction();

    ///
    /// The callback function to invoke within a standalone transaction.
    ///
    /// `this` is passed to the callback function and must be forwarded to operations within the callback function.
    ///
    /// The callback function may be invoked multiple times as part of retry attempt behavior.
    ///
    /// @warning The callback function **MUST** allow any @ref mongocxx::v1::server_error exceptions thrown by an
    /// operation within the callback function to propagate out of the callback function for correct retry attempt
    /// behavior. The callback function **MUST NOT** attempt to manually retry operations.
    ///
    using with_transaction_cb = std::function<void MONGOCXX_ABI_CDECL(client_session&)>;

    ///
    /// Start and commit a new standalone transaction using `fn`.
    ///
    /// The behavior is approximately equivalent to:
    /// ```cpp
    /// this->start_transaction(opts);
    /// while (/* unspecified */) {
    ///     try {
    ///         fn();
    ///         this->commit_transaction(); // Success.
    ///     } catch (/* unspecified */) {
    ///        if (/* unspecified */) {
    ///            continue; // Retry attempt.
    ///        } else {
    ///            this->abort_transaction();
    ///            throw; // Failure.
    ///        }
    ///     }
    /// }
    /// ```
    ///
    /// Any user-defined exceptions thrown by `fn` will be caught and rethrown after the standalone transaction has been
    /// aborted.
    ///
    /// @warning `fn` **MUST** allow any @ref mongocxx::v1::server_error exceptions thrown by an operation within `fn`
    /// to propagate out of `fn` for correct retry attempt behavior. `fn` **MUST NOT** attempt to manually retry
    /// operations.
    ///
    /// @note This operation uses an internal, non-configurable time limit of 120 seconds. The transaction may be
    /// retried until this time limit is exceeded.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and all retry attempts have failed.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) with_transaction(with_transaction_cb const& fn, v1::transaction const& opts);

    ///
    /// Equivalent to @ref with_transaction(with_transaction_cb const& fn, v1::transaction const& opts) with a
    /// default-initialized @ref v1::transaction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) with_transaction(with_transaction_cb const& fn);
};

///
/// Options for @ref mongocxx::v1::client_session.
///
/// Supported fields include:
/// - `causal_consistency` ("causalConsistency")
/// - `snapshot` ("snapshot")
/// - `default_transaction_opts` ("defaultTransactionOptions")
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_session::options {
   private:
    void* _impl; // mongoc_session_opt_t

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
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
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "causalConsistency" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) causal_consistency(bool v);

    ///
    /// Return the current "causalConsistency" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) causal_consistency() const;

    ///
    /// Set the "snapshot" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) snapshot(bool v);

    ///
    /// Return the current "snapshot" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) snapshot() const;

    ///
    /// Set the "defaultTransactionOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) default_transaction_opts(v1::transaction v);

    ///
    /// Return the current "defaultTransactionOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::transaction>) default_transaction_opts() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::client_session.
///
