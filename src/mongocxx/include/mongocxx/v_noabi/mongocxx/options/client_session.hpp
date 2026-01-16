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

#include <mongocxx/options/client_session-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/client_session.hpp> // IWYU pragma: export

#include <utility>

#include <mongocxx/client_session-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/options/transaction.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by client sessions.
///
class client_session {
   public:
    ///
    /// Default initialization.
    ///
    client_session() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ client_session(v1::client_session::options const& opts)
        : _causal_consistency{opts.causal_consistency()},
          _snapshot{opts.snapshot()},
          _default_transaction_opts{opts.default_transaction_opts()} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::client_session::options() const {
        using mongocxx::v_noabi::to_v1;

        v1::client_session::options ret;

        if (_causal_consistency) {
            ret.causal_consistency(*_causal_consistency);
        }

        if (_snapshot) {
            ret.snapshot(*_snapshot);
        }

        if (_default_transaction_opts) {
            ret.default_transaction_opts(to_v1(*_default_transaction_opts));
        }

        return ret;
    }

    ///
    /// Sets the causal_consistency option.
    ///
    /// If true (the default), each operation in the session will be causally ordered after the
    /// previous read or write operation. Set to false to disable causal consistency.
    ///
    /// Unacknowledged writes are not causally consistent. If you execute a write operation with an
    /// unacknowledged write concern (a mongocxx::v_noabi::write_concern with
    /// mongocxx::v_noabi::write_concern::acknowledge_level of @c k_unacknowledged), the write does
    /// not participate in causal consistency.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-isolation-consistency-recency/#causal-consistency
    ///
    client_session& causal_consistency(bool causal_consistency) noexcept {
        _causal_consistency = causal_consistency;
        return *this;
    }

    ///
    /// Gets the value of the causal_consistency option.
    ///
    ///
    bool causal_consistency() const noexcept {
        return _causal_consistency.value_or(true);
    }

    ///
    /// Sets the read concern "snapshot" (not enabled by default).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/read-concern-snapshot/
    ///
    /// @note Snapshot reads and causal consistency are mutually exclusive: only one or the
    /// other may be active at a time. Attempting to do so will result in an error being thrown
    /// by mongocxx::v_noabi::client::start_session.
    ///
    client_session& snapshot(bool enable_snapshot_reads) noexcept {
        _snapshot = enable_snapshot_reads;
        return *this;
    }

    ///
    /// Gets the value of the snapshot_reads option.
    ///
    bool snapshot() const noexcept {
        return _snapshot.value_or(false);
    }

    ///
    /// Sets the default transaction options.
    ///
    /// @param default_transaction_opts
    ///   The default transaction options.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    client_session& default_transaction_opts(transaction default_transaction_opts) {
        _default_transaction_opts = std::move(default_transaction_opts);
        return *this;
    }

    ///
    /// Gets the current default transaction options.
    ///
    /// @return The default transaction options.
    ///
    bsoncxx::v_noabi::stdx::optional<transaction> const& default_transaction_opts() const {
        return _default_transaction_opts;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _causal_consistency;
    bsoncxx::v_noabi::stdx::optional<bool> _snapshot;
    bsoncxx::v_noabi::stdx::optional<transaction> _default_transaction_opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::client_session from_v1(v1::client_session::options v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::client_session::options to_v1(v_noabi::options::client_session v) {
    return v1::client_session::options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::client_session.
///
/// @par Includes
/// - @ref mongocxx/v1/client_session.hpp
///
