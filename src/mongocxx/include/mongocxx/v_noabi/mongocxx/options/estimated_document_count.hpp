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

#include <mongocxx/options/estimated_document_count-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/estimated_document_count_options.hpp> // IWYU pragma: export

#include <chrono>
#include <utility>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>
#include <bsoncxx/types/view.hpp>

#include <mongocxx/read_preference.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::collection::estimated_document_count.
///
class estimated_document_count {
   public:
    ///
    /// Default initialization.
    ///
    estimated_document_count() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() estimated_document_count(
        v1::estimated_document_count_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::estimated_document_count_options() const {
        using bsoncxx::v_noabi::to_v1;

        v1::estimated_document_count_options ret;

        if (_max_time) {
            ret.max_time(*_max_time);
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        if (_read_preference) {
            ret.read_preference(to_v1(*_read_preference));
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
    /// - https://www.mongodb.com/docs/manual/reference/command/count/
    ///
    estimated_document_count& max_time(std::chrono::milliseconds max_time) {
        _max_time = max_time;
        return *this;
    }

    ///
    /// The current max_time setting.
    ///
    /// @return The current max time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/count/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_time() const {
        return _max_time;
    }

    ///
    /// Sets the comment for this operation.
    ///
    /// @param comment
    ///   The new comment.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/count/
    ///
    estimated_document_count& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// The current comment for this operation.
    ///
    /// @return The current comment
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/count/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& comment() const {
        return _comment;
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
    /// - https://www.mongodb.com/docs/manual/reference/command/count/
    ///
    estimated_document_count& read_preference(mongocxx::v_noabi::read_preference rp) {
        _read_preference = std::move(rp);
        return *this;
    }

    ///
    /// The current read_preference for this operation.
    ///
    /// @return the current read_preference
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/count/
    ///
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_preference> const& read_preference() const {
        return _read_preference;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _comment;
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_preference> _read_preference;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::estimated_document_count from_v1(v1::estimated_document_count_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::estimated_document_count_options to_v1(v_noabi::options::estimated_document_count const& v) {
    return v1::estimated_document_count_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::estimated_document_count.
///
/// @par Includes
/// - @ref mongocxx/v1/estimated_document_count_options.hpp
///
