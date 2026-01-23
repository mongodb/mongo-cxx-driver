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

#include <mongocxx/options/aggregate-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/aggregate_options.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint>
#include <utility>

#include <mongocxx/collection-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/database-fwd.hpp>   // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/builder/basic/document.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/hint.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by MongoDB aggregation operations.
///
class aggregate {
   public:
    ///
    /// Default initialization.
    ///
    aggregate() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() aggregate(v1::aggregate_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::aggregate_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::aggregate_options ret;

        if (_allow_disk_use) {
            ret.allow_disk_use(*_allow_disk_use);
        }

        if (_batch_size) {
            ret.batch_size(*_batch_size);
        }

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_max_time) {
            ret.max_time(*_max_time);
        }

        if (_read_preference) {
            ret.read_preference(to_v1(*_read_preference));
        }

        if (_bypass_document_validation) {
            ret.bypass_document_validation(*_bypass_document_validation);
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_read_concern) {
            ret.read_concern(to_v1(*_read_concern));
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        return ret;
    }

    ///
    /// Enables writing to temporary files. When set to @c true, aggregation stages can write data
    /// to the _tmp subdirectory in the dbPath directory. The server-side default is @c false.
    ///
    /// @param allow_disk_use
    ///   Whether or not to allow disk use.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    aggregate& allow_disk_use(bool allow_disk_use) {
        _allow_disk_use = allow_disk_use;
        return *this;
    }

    ///
    /// Retrieves the current allow_disk_use setting.
    ///
    /// @return Whether disk use is allowed.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& allow_disk_use() const {
        return _allow_disk_use;
    }

    ///
    /// Sets the number of documents to return per batch.
    ///
    /// @param batch_size
    ///   The size of the batches to request.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    aggregate& batch_size(std::int32_t batch_size) {
        _batch_size = batch_size;
        return *this;
    }

    ///
    /// The current batch size setting.
    ///
    /// @return The current batch size.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
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
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    aggregate& collation(bsoncxx::v_noabi::document::view_or_value collation) {
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
    /// Sets the variable mapping for this operation.
    ///
    /// @param let
    ///   The new variable mapping.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    aggregate& let(bsoncxx::v_noabi::document::view_or_value let) {
        _let = std::move(let);
        return *this;
    }

    ///
    /// Retrieves the current variable mapping for this operation.
    ///
    /// @return
    ///   The current variable mapping.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& let() const {
        return _let;
    }

    ///
    /// Sets the maximum amount of time for this operation to run server-side in milliseconds.
    ///
    /// @param max_time
    ///   The max amount of time (in milliseconds).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    aggregate& max_time(std::chrono::milliseconds max_time) {
        _max_time = max_time;
        return *this;
    }

    ///
    /// The current max_time setting.
    ///
    /// @return
    ///   The current max time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_time() const {
        return _max_time;
    }

    ///
    /// Sets the read_preference for this operation.
    ///
    /// @param rp the new read_preference
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    aggregate& read_preference(v_noabi::read_preference rp) {
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

    ///
    /// Sets whether the $out stage should bypass document validation.
    ///
    /// @param bypass_document_validation whether or not to bypass validation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    aggregate& bypass_document_validation(bool bypass_document_validation) {
        _bypass_document_validation = bypass_document_validation;
        return *this;
    }

    ///
    /// The current bypass_document_validation setting.
    ///
    /// @return the current bypass_document_validation setting
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& bypass_document_validation() const {
        return _bypass_document_validation;
    }

    ///
    /// Sets the index to use for this operation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
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
    aggregate& hint(v_noabi::hint index_hint) {
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
    /// Sets the write concern to use for this operation. Only has an effect if $out is a part of
    /// the pipeline.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    /// @param write_concern
    ///   Object representing the write_concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    aggregate& write_concern(v_noabi::write_concern write_concern) {
        _write_concern = std::move(write_concern);
        return *this;
    }

    ///
    /// Gets the current write concern.
    ///
    /// @return
    ///   The current write concern, if it is set.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern() const {
        return _write_concern;
    }

    ///
    /// Sets the read concern to use for this operation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    /// @param read_concern
    ///   Object representing the read_concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.
    ///
    aggregate& read_concern(v_noabi::read_concern read_concern) {
        _read_concern = std::move(read_concern);
        return *this;
    }

    ///
    /// Gets the current read concern.
    ///
    /// @return
    ///   The current read concern, if it is set.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_concern> const& read_concern() const {
        return _read_concern;
    }

    ///
    /// Sets the comment to use for this operation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    /// @param comment
    ///   Object representing the comment.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.
    ///
    aggregate& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// Gets the current comment.
    ///
    /// @return
    ///   The current comment, if it is set.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& comment() const {
        return _comment;
    }

    class internal;

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _allow_disk_use;
    bsoncxx::v_noabi::stdx::optional<std::int32_t> _batch_size;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _let;
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_preference> _read_preference;
    bsoncxx::v_noabi::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> _write_concern;
    bsoncxx::v_noabi::stdx::optional<v_noabi::read_concern> _read_concern;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _comment;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::aggregate from_v1(v1::aggregate_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::aggregate_options to_v1(v_noabi::options::aggregate const& v) {
    return v1::aggregate_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::aggregate.
///
/// @par Includes
/// - @ref mongocxx/v1/aggregate_options.hpp
///
