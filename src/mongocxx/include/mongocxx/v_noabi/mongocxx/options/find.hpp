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

#include <mongocxx/options/find-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/v1/cursor.hpp>
#include <mongocxx/v1/find_options.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint>
#include <utility>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/read_preference.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by MongoDB find operations.
///
class find {
   public:
    ///
    /// Default initialization.
    ///
    find() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() find(v1::find_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @note The `comment` field is initialized with `this->comment_option()` (BSON type value) when set; otherwise, by
    /// `this->comment()` (`std::string`) when set; otherwise, it is unset.
    ///
    explicit operator v1::find_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::find_options ret;

        if (_allow_disk_use) {
            ret.allow_disk_use(*_allow_disk_use);
        }

        if (_allow_partial_results) {
            ret.allow_partial_results(*_allow_partial_results);
        }

        if (_batch_size) {
            ret.batch_size(*_batch_size);
        }

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_cursor_type) {
            ret.cursor_type(static_cast<v1::cursor::type>(*_cursor_type));
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_comment_option) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment_option->view())});
        } else if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        if (_limit) {
            ret.limit(*_limit);
        }

        if (_max) {
            ret.max(bsoncxx::v1::document::value{to_v1(_max->view())});
        }

        if (_max_await_time) {
            ret.max_await_time(*_max_await_time);
        }

        if (_max_time) {
            ret.max_time(*_max_time);
        }

        if (_min) {
            ret.min(bsoncxx::v1::document::value{to_v1(_min->view())});
        }

        if (_no_cursor_timeout) {
            ret.no_cursor_timeout(*_no_cursor_timeout);
        }

        if (_projection) {
            ret.projection(bsoncxx::v1::document::value{to_v1(_projection->view())});
        }

        if (_read_preference) {
            ret.read_preference(to_v1(*_read_preference));
        }

        if (_return_key) {
            ret.return_key(*_return_key);
        }

        if (_show_record_id) {
            ret.show_record_id(*_show_record_id);
        }

        if (_skip) {
            ret.skip(*_skip);
        }

        if (_ordering) {
            ret.sort(bsoncxx::v1::document::value{to_v1(_ordering->view())});
        }

        return ret;
    }

    ///
    /// Enables writing to temporary files on the server. When set to true, the server
    /// can write temporary data to disk while executing the find operation.
    ///
    /// This option is sent only if the caller explicitly provides a value. The default
    /// is to not send a value.
    ///
    /// This option may only be used with MongoDB version 4.4 or later.
    ///
    /// @param allow_disk_use
    ///   Whether to allow writing temporary files on the server.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& allow_disk_use(bool allow_disk_use) {
        _allow_disk_use = allow_disk_use;
        return *this;
    }

    ///
    /// Gets the current setting for allowing disk use on the server.
    ///
    /// This option may only be used with MongoDB version 4.4 or later.
    ///
    /// @return Whether disk use on the server is allowed.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& allow_disk_use() const {
        return _allow_disk_use;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& allow_partial_results(bool allow_partial) {
        _allow_partial_results = allow_partial;
        return *this;
    }

    ///
    /// Gets the current setting for allowing partial results from mongos.
    ///
    /// @return Whether partial results from mongos are allowed.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& allow_partial_results() const {
        return _allow_partial_results;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& batch_size(std::int32_t batch_size) {
        _batch_size = batch_size;
        return *this;
    }

    ///
    /// The current batch size setting.
    ///
    /// @return The current batch size.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> const& batch_size() const {
        return _batch_size;
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
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& collation(bsoncxx::v_noabi::document::view_or_value collation) {
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
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& collation() const {
        return _collation;
    }

    ///
    /// Attaches a comment to the query. If $comment also exists in the modifiers document then
    /// the comment field overwrites $comment.
    ///
    /// @deprecated use comment_option instead.
    ///
    /// @param comment
    ///   The comment to attach to this query.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& comment(bsoncxx::v_noabi::string::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// Gets the current comment attached to this query.
    ///
    /// @deprecated use comment_option instead.
    ///
    /// @return The comment attached to this query.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& comment() const {
        return _comment;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& cursor_type(cursor::type cursor_type) {
        _cursor_type = cursor_type;
        return *this;
    }

    ///
    /// Gets the current cursor type.
    ///
    /// @return The current cursor type.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<cursor::type> const& cursor_type() const {
        return _cursor_type;
    }

    ///
    /// Sets the index to use for this operation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
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
    find& hint(v_noabi::hint index_hint) {
        _hint = std::move(index_hint);
        return *this;
    }

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> const& hint() const {
        return _hint;
    }

    ///
    /// Set the value of the let option.
    ///
    /// @param let
    ///   The new let option.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& let(bsoncxx::v_noabi::document::view_or_value let) {
        _let = std::move(let);
        return *this;
    }

    ///
    /// Gets the current value of the let option.
    ///
    /// @return
    ///  The current let option.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const let() const {
        return _let;
    }

    ///
    /// Set the value of the comment option.
    ///
    /// @note Not to be confused with the $comment query modifier.
    ///
    /// @param comment
    ///   The new comment option.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& comment_option(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment_option = std::move(comment);
        return *this;
    }

    ///
    /// Gets the current value of the comment option.
    ///
    /// @note Not to be confused with the $comment query modifier.
    ///
    /// @return
    ///  The current comment option.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& comment_option() const {
        return _comment_option;
    }

    ///
    /// Sets maximum number of documents to return.
    ///
    /// @param limit
    ///   The maximum number of documents to return.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& limit(std::int64_t limit) {
        _limit = limit;
        return *this;
    }

    ///
    /// Gets the current limit.
    ///
    /// @return The current limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<std::int64_t> const& limit() const {
        return _limit;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& max(bsoncxx::v_noabi::document::view_or_value max) {
        _max = std::move(max);
        return *this;
    }

    ///
    /// Sets the current exclusive upper bound for a specific index.
    ///
    /// @return The exclusive upper bound for a specific index.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& max() const {
        return _max;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& max_await_time(std::chrono::milliseconds max_await_time) {
        _max_await_time = max_await_time;
        return *this;
    }

    ///
    /// The maximum amount of time for the server to wait on new documents to satisfy a tailable
    /// cursor query.
    ///
    /// @return The current max await time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_await_time() const {
        return _max_await_time;
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
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& max_time(std::chrono::milliseconds max_time) {
        _max_time = max_time;
        return *this;
    }

    ///
    /// The current max_time_ms setting.
    ///
    /// @return The current max time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_time() const {
        return _max_time;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& min(bsoncxx::v_noabi::document::view_or_value min) {
        _min = std::move(min);
        return *this;
    }

    ///
    /// Sets the current inclusive lower bound for a specific index.
    ///
    /// @return The inclusive lower bound for a specific index.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& min() const {
        return _min;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& no_cursor_timeout(bool no_cursor_timeout) {
        _no_cursor_timeout = no_cursor_timeout;
        return *this;
    }

    ///
    /// Gets the current no_cursor_timeout setting.
    ///
    /// @return The current no_cursor_timeout setting.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& no_cursor_timeout() const {
        return _no_cursor_timeout;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& projection(bsoncxx::v_noabi::document::view_or_value projection) {
        _projection = std::move(projection);
        return *this;
    }

    ///
    /// Gets the current projection set on this query.
    ///
    /// @return The current projection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& projection() const {
        return _projection;
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
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& read_preference(v_noabi::read_preference rp) {
        _read_preference = std::move(rp);
        return *this;
    }

    ///
    /// The current read_preference for this operation.
    ///
    /// @return
    ///   The current read_preference.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_preference> const& read_preference() const {
        return _read_preference;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& return_key(bool return_key) {
        _return_key = return_key;
        return *this;
    }

    ///
    /// Gets the current setting for returning the index keys associated with the query results,
    /// instead of the actual query results themselves.
    ///
    /// @return
    ///   Whether index keys associated with the query results are returned, instead of the actual
    ///   query results themselves.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& return_key() const {
        return _return_key;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& show_record_id(bool show_record_id) {
        _show_record_id = show_record_id;
        return *this;
    }

    ///
    /// Gets the current setting for whether the record identifier is returned for each document in
    /// the query results.
    ///
    /// @return
    ///   Whether the record identifier is included.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& show_record_id() const {
        return _show_record_id;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& skip(std::int64_t skip) {
        _skip = skip;
        return *this;
    }

    ///
    /// Gets the current number of documents to skip.
    ///
    /// @return The number of documents to skip.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<std::int64_t> const& skip() const {
        return _skip;
    }

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
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    find& sort(bsoncxx::v_noabi::document::view_or_value ordering) {
        _ordering = std::move(ordering);
        return *this;
    }

    ///
    /// Gets the current sort ordering for this query.
    ///
    /// @return The current sort ordering.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/find/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& sort() const {
        return _ordering;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _allow_disk_use;
    bsoncxx::v_noabi::stdx::optional<bool> _allow_partial_results;
    bsoncxx::v_noabi::stdx::optional<std::int32_t> _batch_size;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _comment;
    bsoncxx::v_noabi::stdx::optional<cursor::type> _cursor_type;
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _let;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _comment_option;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _limit;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _max;
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_await_time;
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _min;
    bsoncxx::v_noabi::stdx::optional<bool> _no_cursor_timeout;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _projection;
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_preference> _read_preference;
    bsoncxx::v_noabi::stdx::optional<bool> _return_key;
    bsoncxx::v_noabi::stdx::optional<bool> _show_record_id;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _skip;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _ordering;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::find from_v1(v1::find_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::find_options to_v1(v_noabi::options::find const& v) {
    return v1::find_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::find.
///
/// @par Includes
/// - @ref mongocxx/v1/find_options.hpp
///
