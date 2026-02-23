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

#include <mongocxx/options/count-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/count_options.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint>
#include <string> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/hint.hpp>
#include <mongocxx/read_preference.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::collection::count_documents.
///
class count {
   public:
    ///
    /// Default initialization.
    ///
    count() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() count(v1::count_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::count_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::count_options ret;

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        if (_limit) {
            ret.limit(*_limit);
        }

        if (_max_time) {
            ret.max_time(*_max_time);
        }

        if (_skip) {
            ret.skip(*_skip);
        }

        if (_read_preference) {
            ret.read_preference(to_v1(*_read_preference));
        }

        return ret;
    }

    ///
    /// Sets the collation for this operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& collation(bsoncxx::v_noabi::document::view_or_value collation) {
        _collation = std::move(collation);
        return *this;
    }

    ///
    /// Retrieves the current collation for this operation.
    ///
    /// @return
    ///   The current collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& collation() const {
        return _collation;
    }

    ///
    /// Sets the index to use for this operation.
    ///
    /// @param index_hint
    ///   Object representing the index to use.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& hint(v_noabi::hint index_hint) {
        _hint = std::move(index_hint);
        return *this;
    }

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> const& hint() const {
        return _hint;
    }

    ///
    /// Set the value of the comment option.
    ///
    /// @param comment
    ///   The new comment option.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// Gets the current value of the comment option.
    ///
    /// @return The current comment option.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& comment() const {
        return _comment;
    }

    ///
    /// Sets the maximum number of documents to count.
    ///
    /// @param limit
    ///  The max number of documents to count.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& limit(std::int64_t limit) {
        _limit = limit;
        return *this;
    }

    ///
    /// Gets the current limit.
    ///
    /// @return The current limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<std::int64_t> const& limit() const {
        return _limit;
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
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& max_time(std::chrono::milliseconds max_time) {
        _max_time = max_time;
        return *this;
    }

    ///
    /// The current max_time setting.
    ///
    /// @return The current max time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_time() const {
        return _max_time;
    }

    ///
    /// Sets the number of documents to skip before counting documents.
    ///
    /// @param skip
    ///   The number of documents to skip.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& skip(std::int64_t skip) {
        _skip = skip;
        return *this;
    }

    ///
    /// Gets the current number of documents to skip.
    ///
    /// @return The number of documents to skip.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<std::int64_t> const& skip() const {
        return _skip;
    }

    ///
    /// Sets the read_preference for this operation.
    ///
    /// @param rp
    ///   The new read_preference.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    count& read_preference(v_noabi::read_preference rp) {
        _read_preference = std::move(rp);
        return *this;
    }

    ///
    /// The current read_preference for this operation.
    ///
    /// @return the current read_preference
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_preference> const& read_preference() const {
        return _read_preference;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _comment;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _limit;
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _skip;
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_preference> _read_preference;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::count from_v1(v1::count_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::count_options to_v1(v_noabi::options::count const& v) {
    return v1::count_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::count.
///
/// @par Includes
/// - @ref mongocxx/v1/count_options.hpp
///
