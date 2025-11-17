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

#include <mongocxx/read_concern-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/read_concern.hpp> // IWYU pragma: export

#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <mongocxx/client-fwd.hpp>              // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/collection-fwd.hpp>          // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/database-fwd.hpp>            // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/options/transaction-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/uri-fwd.hpp>                 // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/options/transaction.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Controls the consistency and isolation properties of data read from replica sets and sharded
/// clusters.
///
/// Read concern can be set at the client, database, or collection level. The read concern can also
/// be provided via connection string, and will be parsed and set on the client constructed for the
/// URI.
///
/// @see
/// - [Read Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/read-concern/)
///
class read_concern {
   private:
    v1::read_concern _rc;

   public:
    ///
    /// A class to represent the read concern level for read operations.
    ///
    /// @see
    /// - [Read Concern Levels (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/read-concern/#read-concern-levels)
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/#read-concern)
    ///
    enum class level {
        k_local,          ///< Represent read concern level "local".
        k_majority,       ///< Represent read concern level "majority".
        k_linearizable,   ///< Represent read concern level "linearizable".
        k_server_default, ///< Represent the server's default read concern level.
        k_unknown,        ///< Represent an unknown read concern level.
        k_available,      ///< Represent read concern level "available".
        k_snapshot        ///< Represent read concern level "snapshot".
    };

    ///
    /// Constructs a new read_concern with default acknowledge_level of k_server_default.
    ///
    /// The k_server_default acknowledge level has an empty acknowledge_string. Queries that
    /// run with this read_concern will use the server's default read_concern instead of
    /// specifying one.
    ///
    read_concern() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ read_concern(v1::read_concern rc) : _rc{std::move(rc)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated iterators and views.
    ///
    explicit operator v1::read_concern() && {
        return std::move(_rc);
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::read_concern() const& {
        return _rc;
    }

    ///
    /// Sets the read concern level.
    ///
    /// @param rc_level
    ///   Either k_local, k_majority, k_linearizable, or k_server_default.
    ///
    /// @throws
    ///   mongocxx::v_noabi::exception if rc_level is not k_local, k_majority, k_linearizable, or
    ///   k_server_default.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) acknowledge_level(level rc_level);

    ///
    /// Gets the current read concern level.
    ///
    /// If this was set with acknowledge_string to anything other than "local", "majority",
    /// "linearizable", or an empty string, this will return k_unknown.
    ///
    /// @return The read concern level.
    ///
    level acknowledge_level() const {
        return static_cast<level>(_rc.acknowledge_level());
    }

    ///
    /// Sets the read concern string. Any valid read concern string (e.g. "local",
    /// "majority", "linearizable", "") may be passed in.  For forward-compatibility
    /// with read concern levels introduced in the future, no validation is performed on
    /// this string.
    ///
    /// @param rc_string
    ///   The read concern string.
    ///
    void acknowledge_string(bsoncxx::v1::stdx::string_view rc_string) {
        _rc.acknowledge_string(rc_string);
    }

    ///
    /// Gets the current read concern string.
    ///
    /// If the read concern level was set with acknowledge_level, this will return either "local",
    /// "majority", "linearizable", or an empty string for k_server_default.
    ///
    /// @return The read concern string.
    ///
    bsoncxx::v1::stdx::string_view acknowledge_string() const {
        auto ret = _rc.acknowledge_string();
        if (ret.empty()) {
            ret = "";
        }
        return ret;
    }

    ///
    /// Gets the document form of this read_concern.
    ///
    /// @return
    ///   Document representation of this read_concern.
    ///
    bsoncxx::v_noabi::document::value to_document() const {
        return bsoncxx::v_noabi::from_v1(_rc.to_document());
    }

    ///
    /// @relates mongocxx::v_noabi::read_concern
    ///
    /// Compares two read_concern objects for (in)-equality.
    ///
    /// @{
    friend bool operator==(read_concern const& lhs, read_concern const& rhs) {
        return lhs.acknowledge_level() == rhs.acknowledge_level();
    }

    friend bool operator!=(read_concern const& lhs, read_concern const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///

    class internal;
};

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::read_concern from_v1(v1::read_concern v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::read_concern to_v1(v_noabi::read_concern v) {
    return v1::read_concern{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::read_concern.
///
/// @par Includes
/// - @ref mongocxx/v1/read_concern.hpp
///
