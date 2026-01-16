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

#include <mongocxx/client_session-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/client_session.hpp> // IWYU pragma: export

#include <cstdint>
#include <functional>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/bulk_write-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/client-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>        // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/database-fwd.hpp>          // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/index_view-fwd.hpp>        // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/search_index_view-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/options/client_session.hpp>
#include <mongocxx/options/transaction.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Supports MongoDB client session operations.
///
/// Use a session for a sequence of operations, optionally with either causal consistency
/// or snapshots.
///
/// Note that client_session is not thread-safe. See
/// https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/thread-safety/ for more details.
///
/// @see
/// - https://www.mongodb.com/docs/manual/core/read-isolation-consistency-recency/#causal-consistency
///
class client_session {
   private:
    v1::client_session _session;
    v_noabi::client* _client;
    v_noabi::options::client_session _options;

   public:
    using transaction_state = v1::client_session::transaction_state;

    ///
    /// Gets the client that started this session.
    ///
    v_noabi::client const& client() const noexcept {
        return *_client;
    }

    ///
    /// Gets the options this session was created with.
    ///
    options::client_session const& options() const noexcept {
        return _options;
    }

    ///
    /// Get the server-side "logical session ID" associated with this session, as a BSON document.
    /// This view is invalid after the session is destroyed.
    ///
    bsoncxx::v_noabi::document::view id() const noexcept {
        return _session.id();
    }

    ///
    /// Get the session's clusterTime, as a BSON document. This is an opaque value suitable for
    /// passing to advance_cluster_time(). The document is empty if the session has
    /// not been used for any operation and you have not called advance_cluster_time().
    /// This view is invalid after the session is destroyed.
    ///
    bsoncxx::v_noabi::document::view cluster_time() const noexcept {
        return _session.cluster_time();
    }

    ///
    /// Get the session's operationTime, as a BSON timestamp. This is an opaque value suitable for
    /// passing to advance_operation_time(). The timestamp is zero if the session has not been used
    /// for any operation and you have not called advance_operation_time().
    ///
    bsoncxx::v_noabi::types::b_timestamp operation_time() const noexcept {
        return bsoncxx::v_noabi::from_v1(_session.operation_time());
    }

    ///
    /// Get the server_id the session is pinned to. The server_id is zero if the session is not
    /// pinned to a server.
    ///
    std::uint32_t server_id() const noexcept {
        return _session.server_id();
    }

    ///
    /// Returns the current transaction state for this session.
    ///
    transaction_state get_transaction_state() const noexcept {
        return _session.get_transaction_state();
    }

    ///
    /// Returns whether or not this session is dirty.
    ///
    bool get_dirty() const noexcept {
        return _session.get_dirty();
    }

    ///
    /// Advance the cluster time for a session. Has an effect only if the new cluster time is
    /// greater than the session's current cluster time.
    ///
    /// Use advance_operation_time() and advance_cluster_time() to copy the operationTime and
    /// clusterTime from another session, ensuring subsequent operations in this session are
    /// causally consistent with the last operation in the other session.
    ///
    void advance_cluster_time(bsoncxx::v_noabi::document::view const& cluster_time) {
        _session.advance_cluster_time(bsoncxx::v_noabi::to_v1(cluster_time));
    }

    ///
    /// Advance the session's operation time, expressed as a BSON timestamp. Has an effect only if
    /// the new operation time is greater than the session's current operation time.
    ///
    /// Use advance_operation_time() and advance_cluster_time() to copy the operationTime and
    /// clusterTime from another session, ensuring subsequent operations in this session are
    /// causally consistent with the last operation in the other session.
    ///
    void advance_operation_time(bsoncxx::v_noabi::types::b_timestamp const& operation_time) {
        _session.advance_operation_time(bsoncxx::v_noabi::to_v1(operation_time));
    }

    ///
    /// Starts a transaction on the current client session.
    ///
    /// @param transaction_opts (optional)
    ///    The options to use in the transaction.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the options are misconfigured, if there
    /// are network or other transient failures, or if there are other errors such as a session with
    /// a transaction already in progress.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) start_transaction(
        bsoncxx::v_noabi::stdx::optional<v_noabi::options::transaction> const& transaction_opts = {});

    ///
    /// Commits a transaction on the current client session.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the options are misconfigured, if there
    /// are network or other transient failures, or if there are other errors such as a session with
    /// no transaction in progress.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) commit_transaction();

    ///
    /// Aborts a transaction on the current client session.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the options are misconfigured or if there
    /// are other errors such as a session with no transaction in progress.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) abort_transaction();

    ///
    /// Represents a callback invoked within a transaction.
    ///
    using with_transaction_cb = std::function<void MONGOCXX_ABI_CDECL(client_session*)>;

    ///
    /// Helper to run a user-provided callback within a transaction.
    ///
    /// This method will start a new transaction on this client session,
    /// run the callback, then commit the transaction. If it cannot commit
    /// the transaction, the entire sequence may be retried, and the callback
    /// may be run multiple times.
    ///
    /// This method has an internal non-adjustable time limit of 120 seconds,
    /// including all retries.
    ///
    /// If the user callback invokes driver methods that run operations against the
    /// server which could throw an operation_exception, the user callback MUST allow
    /// those exceptions to propagate up the stack so they can be caught and processed
    /// by the with_transaction() helper.
    ///
    /// For example, a callback that invokes collection::insert_one may encounter a
    /// "duplicate key" error with accompanying server-side transaction abort. If this
    /// error were not seen by the with_transaction() helper, the entire transaction
    /// would retry repeatedly until the overall time limit expires.
    ///
    /// @param cb
    ///   The callback to run inside of a transaction.
    /// @param opts (optional)
    ///   The options to use to run the transaction.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if there are errors completing the
    /// transaction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) with_transaction(with_transaction_cb cb, v_noabi::options::transaction opts = {});

    class internal;

   private:
    client_session(v1::client_session session, v_noabi::client& client, v_noabi::options::client_session options);
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::client_session.
///
/// @par Includes
/// - @ref mongocxx/v1/client_session.hpp
///
