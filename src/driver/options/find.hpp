// Copyright 2014 MongoDB Inc.
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

#include "driver/config/prelude.hpp"

#include <cstdint>

#include "bson/document.hpp"
#include "driver/base/read_preference.hpp"
#include "stdx/optional.hpp"

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB query
///
class LIBMONGOCXX_EXPORT find {

   public:
    enum class cursor_type: std::uint8_t;

    ///
    /// Sets whether to allow partial results from a mongos if some shards are down (instead of
    /// throwing an error).
    ///
    /// @param allow_partial
    ///   Whether to allow partial results from mongos.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    void allow_partial_results(bool allow_partial);

    ///
    /// Gets the current setting for allowing partial results from mongos.
    ///
    /// @return Whether or not we allow partial results from mongos.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    const optional<bool>& allow_partial_results() const;

    ///
    /// Sets the number of documents to return per batch.
    ///
    /// @param batch_size
    ///   The size of the batches to request.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.batchSize/
    ///
    void batch_size(std::int32_t batch_size);

    ///
    /// The current batch size setting.
    ///
    /// @return The current batch size.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.batchSize/
    ///
    const optional<std::int32_t>& batch_size() const;

    ///
    /// Attaches a comment to the query. If $comment also exists in the modifiers document then
    /// the comment field overwrites $comment.
    ///
    /// @param comment
    ///   The comment to attach to this query.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/comment/
    ///
    void comment(std::string comment);

    ///
    /// Gets the current comment attached to this query.
    ///
    /// @return The comment attached to this query.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/comment/
    ///
    const optional<std::string>& comment() const;

    ///
    /// Indicates the type of cursor to use for this query.
    ///
    /// @param cursor_type
    ///   The cursor type to set.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    void cursor_type(cursor_type cursor_type);

    ///
    /// Gets the current cursor type.
    ///
    /// @return The current cursor type.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    const optional<enum cursor_type>& cursor_type() const;

    ///
    /// Sets maximum number of documents to return.
    ///
    /// @param limit
    ///   The maximum number of documents to return.
    ///
    void limit(std::int32_t limit);

    ///
    /// Gets the current limit.
    ///
    /// @return The current limit.
    ///
    const optional<std::int32_t>& limit() const;

    ///
    /// Sets the maximum amount of time for this operation to run (server side) in milliseconds.
    ///
    /// @param max_time_ms
    ///   The max amount of time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    void max_time_ms(std::int64_t max_time_ms);

    ///
    /// The current max_time_ms setting.
    ///
    /// @return The current max time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    const optional<std::int64_t>& max_time_ms() const;

    ///
    /// Sets the meta-operators modifying the output or behavior of the query.
    ///
    /// @param modifiers
    ///   The query modifiers.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/query-modifier/
    ///
    void modifiers(bson::document::view modifiers);

    ///
    /// Gets the current query modifiers.
    ///
    /// @return The current query modifiers.
    ///
    const optional<bson::document::view>& modifiers() const;

    ///
    /// Sets the cursor flag to prevent cursor from timing out server side due to a period of
    /// inactivity.
    ///
    /// @param no_cursor_timeout
    ///   When true prevents the cursor from timing out.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    void no_cursor_timeout(bool no_cursor_timeout);

    ///
    /// Gets the current no cursor timeout setting.
    ///
    /// @return The current no cursor timeout setting.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    const optional<bool>& no_cursor_timeout() const;

    void oplog_replay(bool oplog_replay);
    const optional<bool>& oplog_replay() const;

    ///
    /// Sets a projection which limits the fields to return for all matching documents.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    void projection(bson::document::view projection);

    ///
    /// Gets the current projection.
    ///
    /// @return The current projection.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    const optional<bson::document::view>& projection() const;

    ///
    /// Sets the read_preference for this operation.
    ///
    /// @param rp
    ///   The new read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    void read_preference(read_preference rp);

    ///
    /// The current read_preference for this operation.
    ///
    /// @return
    ///   The current read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    const optional<class read_preference>& read_preference() const;

    ///
    /// Sets the number of documents to skip before returning results.
    ///
    /// @param skip
    ///   The number of documents to skip.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.skip/
    ///
    void skip(std::int32_t skip);

    ///
    /// Gets the current number of documents to skip.
    ///
    /// @return The number of documents to skip.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.skip/
    ///
    const optional<std::int32_t>& skip() const;

    ///
    /// The order in which to return matching documents. If $orderby also exists in the modifiers
    /// document, the sort field overwrites $orderby.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.sort/
    ///
    void sort(bson::document::view ordering);

    ///
    /// Gets the current sort ordering.
    ///
    /// @return The current sort ordering.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.sort/
    ///
    const optional<bson::document::view>& sort() const;

   private:
    optional<bool> _allow_partial_results;
    optional<std::int32_t> _batch_size;
    optional<std::string> _comment;
    optional<enum cursor_type> _cursor_type;
    optional<std::int32_t> _limit;
    optional<std::int64_t> _max_time_ms;
    optional<bson::document::view> _modifiers;
    optional<bool> _no_cursor_timeout;
    optional<bool> _oplog_replay;
    optional<bson::document::view> _projection;
    optional<class read_preference> _read_preference;
    optional<std::int32_t> _skip;
    optional<bson::document::view> _ordering;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
