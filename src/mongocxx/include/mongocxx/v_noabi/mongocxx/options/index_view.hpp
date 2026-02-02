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

#include <mongocxx/options/index_view-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/indexes.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by MongoDB index view operations.
///
class index_view {
   public:
    index_view() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /// @note Only `CreateIndexOptions` fields are converted. All other fields are unset.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() index_view(v1::indexes::options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @note Only `CreateIndexOptions` fields are converted. All other fields are unset.
    ///
    explicit operator v1::indexes::options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::indexes::options ret;

        if (_max_time) {
            ret.max_time(*_max_time);
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_commit_quorum) {
            auto const v = _commit_quorum->view()["commitQuorum"];

            if (v.type() == bsoncxx::v_noabi::type::k_int32) {
                ret.commit_quorum(v.get_int32().value);
            }

            if (v.type() == bsoncxx::v_noabi::type::k_string) {
                ret.commit_quorum(v.get_string().value);
            }
        }

        return ret;
    }

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
    ///
    /// @param max_time
    ///   The max amount of time (in milliseconds).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    index_view& max_time(std::chrono::milliseconds max_time) {
        _max_time = max_time;
        return *this;
    }

    ///
    /// The current max_time setting.
    ///
    /// @return
    ///   The current max allowed running time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_time() const {
        return _max_time;
    }

    ///
    /// Sets the write concern for this operation.
    ///
    /// @param write_concern
    ///   Object representing the write concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    index_view& write_concern(mongocxx::v_noabi::write_concern write_concern) {
        _write_concern = std::move(write_concern);
        return *this;
    }

    ///
    /// Gets the current write concern.
    ///
    /// @return
    ///   The current write concern.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern() const {
        return _write_concern;
    }

    ///
    /// Sets the commit quorum for this operation.
    ///
    /// This option may only be used with MongoDB version 4.4 or later.
    ///
    /// @param commit_quorum
    ///   Integer representing the minimum number of data-bearing voting replica set members (i.e.
    ///   commit quorum), including the primary, that must report a successful index build before
    ///   the primary marks the indexes as ready. A value of @c 0 disables quorum-voting behavior.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/createIndexes
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index_view&) commit_quorum(std::int32_t commit_quorum);

    ///
    /// Sets the commit quorum for this operation.
    ///
    /// This option may only be used with MongoDB version 4.4 or later.
    ///
    /// @param commit_quorum
    ///   String representing the minimum number of data-bearing voting replica set members (i.e.
    ///   commit quorum), including the primary, that must report a successful index build before
    ///   the primary marks the indexes as ready.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/createIndexes
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index_view&) commit_quorum(std::string commit_quorum);

    ///
    /// Gets the current commitQuorum setting.
    ///
    /// This option may only be used with MongoDB version 4.4 or later.
    ///
    /// @return
    ///   The current commitQuorum setting.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/createIndexes
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> const commit_quorum() const {
        return _commit_quorum;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> _write_concern;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> _commit_quorum;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

// Ambiguous whether `v1::indexes::options` should be converted to `v_noabi::options::index_view` or
// `v_noabi::options::index`. Require users to explicitly cast to the expected type instead.
//
// v_noabi::options::index_view from_v1(v1::indexes::options v);

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
/// @note "IndexOptions" fields are ignored.
///
inline v1::indexes::options to_v1(v_noabi::options::index_view v) {
    return v1::indexes::options{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::index_view.
///
/// @par Includes
/// - @ref mongocxx/v1/indexes.hpp
///
