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

#include <chrono>
#include <cstdint>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/read_preference.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

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
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& allow_partial_results(bool allow_partial);

    ///
    /// Gets the current setting for allowing partial results from mongos.
    ///
    /// @return Whether partial results from mongos are allowed.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bool>& allow_partial_results() const;

    ///
    /// Sets the number of documents to return per batch.
    ///
    /// @param batch_size
    ///   The size of the batches to request.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& batch_size(std::int32_t batch_size);

    ///
    /// The current batch size setting.
    ///
    /// @return The current batch size.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<std::int32_t>& batch_size() const;

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
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& collation(bsoncxx::document::view_or_value collation);

    ///
    /// Retrieves the current collation for this operation.
    ///
    /// @return
    ///   The current collation.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& collation() const;

    ///
    /// Attaches a comment to the query. If $comment also exists in the modifiers document then
    /// the comment field overwrites $comment.
    ///
    /// @param comment
    ///   The comment to attach to this query.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& comment(bsoncxx::string::view_or_value comment);

    ///
    /// Gets the current comment attached to this query.
    ///
    /// @return The comment attached to this query.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bsoncxx::string::view_or_value>& comment() const;

    ///
    /// Indicates the type of cursor to use for this query.
    ///
    /// @param cursor_type
    ///   The cursor type to set.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& cursor_type(cursor::type cursor_type);

    ///
    /// Gets the current cursor type.
    ///
    /// @return The current cursor type.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<cursor::type>& cursor_type() const;

    ///
    /// Sets the index to use for this operation.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    /// @note if the server already has a cached shape for this query, it may
    /// ignore a hint.
    ///
    /// @param index_hint
    ///   Object representing the index to use.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    find& hint(class hint index_hint);

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<class hint>& hint() const;

    ///
    /// Sets maximum number of documents to return.
    ///
    /// @param limit
    ///   The maximum number of documents to return.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& limit(std::int64_t limit);

    ///
    /// Gets the current limit.
    ///
    /// @return The current limit.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<std::int64_t>& limit() const;

    ///
    /// Gets the current exclusive upper bound for a specific index.
    ///
    /// @param max
    ///   The exclusive upper bound for a specific index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& max(bsoncxx::document::view_or_value max);

    ///
    /// Sets the current exclusive upper bound for a specific index.
    ///
    /// @return The exclusive upper bound for a specific index.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& max() const;

    ///
    /// The maximum amount of time for the server to wait on new documents to satisfy a tailable
    /// cursor query. This only applies to a TAILABLE_AWAIT cursor. When the cursor is not a
    /// TAILABLE_AWAIT cursor, this option is ignored. The default on the server is to wait for one
    /// second.
    ///
    /// @note On servers < 3.2, this option is ignored.
    ///
    /// @param max_await_time
    ///   The max amount of time (in milliseconds) to wait for new documents.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& max_await_time(std::chrono::milliseconds max_await_time);

    ///
    /// The maximum amount of time for the server to wait on new documents to satisfy a tailable
    /// cursor query.
    ///
    /// @return The current max await time (in milliseconds).
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<std::chrono::milliseconds>& max_await_time() const;

    ///
    /// Sets the maximum number of documents or index keys to scan when executing the query.
    ///
    /// @param max
    ///   The maximum number of documents or index keys to scan.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& max_scan(std::int64_t max);

    ///
    /// Gets the current setting for the maximum number of documents to scan when executing the
    /// query.
    ///
    /// @return The current setting for the maximum number of documents or index keys to scan.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<std::int64_t>& max_scan() const;

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
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& max_time(std::chrono::milliseconds max_time);

    ///
    /// The current max_time_ms setting.
    ///
    /// @return The current max time (in milliseconds).
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<std::chrono::milliseconds>& max_time() const;

    ///
    /// Gets the current inclusive lower bound for a specific index.
    ///
    /// @param min
    ///   The inclusive lower bound for a specific index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& min(bsoncxx::document::view_or_value min);

    ///
    /// Sets the current inclusive lower bound for a specific index.
    ///
    /// @return The inclusive lower bound for a specific index.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& min() const;

    ///
    /// Sets the meta-operators modifying the output or behavior of the query.
    ///
    /// @param modifiers
    ///   The query modifiers.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @deprecated
    ///   The modifiers option has been deprecated, and has been replaced by new top-level options
    ///   that have been introduced. For example, users should call find::snapshot() instead of
    ///   calling find::modifiers() with a document containing a "$snapshot" field.
    ///
    /// @see https://docs.mongodb.com/master/reference/operator/query-modifier/
    ///
    MONGOCXX_DEPRECATED find& modifiers(bsoncxx::document::view_or_value modifiers);
    find& modifiers_deprecated(bsoncxx::document::view_or_value modifiers);

    ///
    /// Gets the current query modifiers.
    ///
    /// @return The current query modifiers.
    ///
    /// @deprecated
    ///   The modifiers option has been deprecated, and has been replaced by new top-level options
    ///   that have been introduced. For example, users should call find::snapshot() instead of
    ///   calling find::modifiers() with a document containing a "$snapshot" field.
    ///
    MONGOCXX_DEPRECATED const stdx::optional<bsoncxx::document::view_or_value>& modifiers() const;
    const stdx::optional<bsoncxx::document::view_or_value>& modifiers_deprecated() const;

    ///
    /// Sets the cursor flag to prevent cursor from timing out server-side due to a period of
    /// inactivity.
    ///
    /// @param no_cursor_timeout
    ///   When true prevents the cursor from timing out.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& no_cursor_timeout(bool no_cursor_timeout);

    ///
    /// Gets the current no_cursor_timeout setting.
    ///
    /// @return The current no_cursor_timeout setting.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bool>& no_cursor_timeout() const;

    ///
    /// Sets a projection which limits the returned fields for all matching documents.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& projection(bsoncxx::document::view_or_value projection);

    ///
    /// Gets the current projection set on this query.
    ///
    /// @return The current projection.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& projection() const;

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
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& read_preference(class read_preference rp);

    ///
    /// The current read_preference for this operation.
    ///
    /// @return
    ///   The current read_preference.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<class read_preference>& read_preference() const;

    ///
    /// Sets whether to return the index keys associated with the query results, instead of the
    /// actual query results themselves.
    ///
    /// @param return_key
    ///   Whether to return the index keys associated with the query results, instead of the actual
    ///   query results themselves.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& return_key(bool return_key);

    ///
    /// Gets the current setting for returning the index keys associated with the query results,
    /// instead of the actual query results themselves.
    ///
    /// @return
    ///   Whether index keys associated with the query results are returned, instead of the actual
    ///   query results themselves.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bool>& return_key() const;

    ///
    /// Sets whether to include the record identifier for each document in the query results.
    ///
    /// @param show_record_id
    ///   Whether to include the record identifier.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& show_record_id(bool show_record_id);

    ///
    /// Gets the current setting for whether the record identifier is returned for each document in
    /// the query results.
    ///
    /// @return
    ///   Whether the record identifier is included.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bool>& show_record_id() const;

    ///
    /// Sets the number of documents to skip before returning results.
    ///
    /// @param skip
    ///   The number of documents to skip.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& skip(std::int64_t skip);

    ///
    /// Gets the current number of documents to skip.
    ///
    /// @return The number of documents to skip.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<std::int64_t>& skip() const;

    ///
    /// Sets whether snapshot mode should be used.
    ///
    /// @param snapshot
    ///   Whether to enable snapshot mode.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& snapshot(bool snapshot);

    ///
    /// Gets the current setting for whether snapshot mode is being used.
    ///
    /// @return Whether snapshot mode is being used.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bool>& snapshot() const;

    ///
    /// The order in which to return matching documents. If $orderby also exists in the modifiers
    /// document, the sort field takes precedence over $orderby.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    find& sort(bsoncxx::document::view_or_value ordering);

    ///
    /// Gets the current sort ordering for this query.
    ///
    /// @return The current sort ordering.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/find/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& sort() const;

    ///
    /// Clears the modifiers member. Internal-only.
    ///
    MONGOCXX_PRIVATE find& modifiers_clear();

   private:
    stdx::optional<bool> _allow_partial_results;
    stdx::optional<std::int32_t> _batch_size;
    stdx::optional<bsoncxx::document::view_or_value> _collation;
    stdx::optional<bsoncxx::string::view_or_value> _comment;
    stdx::optional<cursor::type> _cursor_type;
    stdx::optional<class hint> _hint;
    stdx::optional<std::int64_t> _limit;
    stdx::optional<bsoncxx::document::view_or_value> _max;
    stdx::optional<std::chrono::milliseconds> _max_await_time;
    stdx::optional<std::int64_t> _max_scan;
    stdx::optional<std::chrono::milliseconds> _max_time;
    stdx::optional<bsoncxx::document::view_or_value> _min;
    stdx::optional<bsoncxx::document::view_or_value> _modifiers;
    stdx::optional<bool> _no_cursor_timeout;
    stdx::optional<bsoncxx::document::view_or_value> _projection;
    stdx::optional<class read_preference> _read_preference;
    stdx::optional<bool> _return_key;
    stdx::optional<bool> _show_record_id;
    stdx::optional<std::int64_t> _skip;
    stdx::optional<bool> _snapshot;
    stdx::optional<bsoncxx::document::view_or_value> _ordering;

    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const find&, const find&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const find&, const find&);
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
