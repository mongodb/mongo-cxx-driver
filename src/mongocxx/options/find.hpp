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

#include <mongocxx/config/prelude.hpp>

#include <cstdint>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/read_preference.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

enum class cursor_type : std::uint8_t { k_non_tailable, k_tailable, k_tailable_await };

///
/// Class representing the optional arguments to a MongoDB query.
///
class MONGOCXX_API find {

   public:

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
    /// @return Whether partial results from mongos are allowed.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    const stdx::optional<bool>& allow_partial_results() const;

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
    const stdx::optional<std::int32_t>& batch_size() const;

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
    const stdx::optional<std::string>& comment() const;

    ///
    /// Indicates the type of cursor to use for this query.
    ///
    /// @param cursor_type
    ///   The cursor type to set.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    void cursor_type(enum cursor_type cursor_type);

    ///
    /// Gets the current cursor type.
    ///
    /// @return The current cursor type.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    const stdx::optional<enum cursor_type>& cursor_type() const;

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
    const stdx::optional<std::int32_t>& limit() const;

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
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
    const stdx::optional<std::int64_t>& max_time_ms() const;

    ///
    /// Sets the meta-operators modifying the output or behavior of the query.
    ///
    /// @param modifiers
    ///   The query modifiers.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/query-modifier/
    ///
    void modifiers(bsoncxx::document::view modifiers);

    ///
    /// Gets the current query modifiers.
    ///
    /// @return The current query modifiers.
    ///
    const stdx::optional<bsoncxx::document::view>& modifiers() const;

    ///
    /// Sets the cursor flag to prevent cursor from timing out server-side due to a period of
    /// inactivity.
    ///
    /// @param no_cursor_timeout
    ///   When true prevents the cursor from timing out.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    void no_cursor_timeout(bool no_cursor_timeout);

    ///
    /// Gets the current no_cursor_timeout setting.
    ///
    /// @return The current no_cursor_timeout setting.
    ///
    /// @see http://docs.mongodb.org/meta-driver/latest/legacy/mongodb-wire-protocol/#op-query
    ///
    const stdx::optional<bool>& no_cursor_timeout() const;

    void oplog_replay(bool oplog_replay);
    const stdx::optional<bool>& oplog_replay() const;

    ///
    /// Sets a projection which limits the returned fields for all matching documents.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    void projection(bsoncxx::document::view projection);

    ///
    /// Gets the current projection set on this query.
    ///
    /// @return The current projection.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    const stdx::optional<bsoncxx::document::view>& projection() const;

    ///
    /// Sets the read_preference for this operation.
    ///
    /// @param rp
    ///   The new read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    void read_preference(class read_preference rp);

    ///
    /// The current read_preference for this operation.
    ///
    /// @return
    ///   The current read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    const stdx::optional<class read_preference>& read_preference() const;

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
    const stdx::optional<std::int32_t>& skip() const;

    ///
    /// The order in which to return matching documents. If $orderby also exists in the modifiers
    /// document, the sort field takes precedence over $orderby.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.sort/
    ///
    void sort(bsoncxx::document::view ordering);

    ///
    /// Gets the current sort ordering for this query.
    ///
    /// @return The current sort ordering.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.sort/
    ///
    const stdx::optional<bsoncxx::document::view>& sort() const;

   private:
    stdx::optional<bool> _allow_partial_results;
    stdx::optional<std::int32_t> _batch_size;
    stdx::optional<std::string> _comment;
    stdx::optional<enum cursor_type> _cursor_type;
    stdx::optional<std::int32_t> _limit;
    stdx::optional<std::int64_t> _max_time_ms;
    stdx::optional<bsoncxx::document::view> _modifiers;
    stdx::optional<bool> _no_cursor_timeout;
    stdx::optional<bool> _oplog_replay;
    stdx::optional<bsoncxx::document::view> _projection;
    stdx::optional<class read_preference> _read_preference;
    stdx::optional<std::int32_t> _skip;
    stdx::optional<bsoncxx::document::view> _ordering;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
