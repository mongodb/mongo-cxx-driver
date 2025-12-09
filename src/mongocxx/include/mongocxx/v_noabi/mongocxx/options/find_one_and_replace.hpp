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

#include <mongocxx/options/find_one_and_replace-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/find_one_and_replace_options.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/hint.hpp>
#include <mongocxx/options/find_one_common_options.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::collection.
///
class find_one_and_replace {
   public:
    ///
    /// Default initialization.
    ///
    find_one_and_replace() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() find_one_and_replace(v1::find_one_and_replace_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::find_one_and_replace_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::find_one_and_replace_options ret;

        if (_bypass_document_validation) {
            ret.bypass_document_validation(*_bypass_document_validation);
        }

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(*_comment)});
        }

        if (_max_time) {
            ret.max_time(*_max_time);
        }

        if (_projection) {
            ret.projection(bsoncxx::v1::document::value{to_v1(_projection->view())});
        }

        if (_return_document) {
            ret.return_document(*_return_document);
        }

        if (_ordering) {
            ret.sort(bsoncxx::v1::document::value{to_v1(_ordering->view())});
        }

        if (_upsert) {
            ret.upsert(*_upsert);
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        return ret;
    }

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
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    find_one_and_replace& collation(bsoncxx::v_noabi::document::view_or_value collation) {
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
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& collation() const {
        return _collation;
    }

    ///
    /// Whether or not to bypass document validation for this operation.
    ///
    /// @note
    ///   On servers >= 3.2, the server applies validation by default. On servers < 3.2, this option
    ///   is ignored.
    ///
    /// @param bypass_document_validation
    ///   Whether or not to bypass document validation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    find_one_and_replace& bypass_document_validation(bool bypass_document_validation) {
        _bypass_document_validation = bypass_document_validation;
        return *this;
    }

    ///
    /// The current setting for bypassing document validation.
    ///
    /// @return the current bypass document validation setting.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& bypass_document_validation() const {
        return _bypass_document_validation;
    }

    ///
    /// Sets the index to use for this operation.
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
    find_one_and_replace& hint(v_noabi::hint index_hint) {
        _hint = std::move(index_hint);
        return *this;
    }

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
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
    find_one_and_replace& let(bsoncxx::v_noabi::document::view_or_value let) {
        _let = std::move(let);
        return *this;
    }

    ///
    /// Gets the current value of the let option.
    ///
    /// @return
    ///  The current let option.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const let() const {
        return _let;
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
    find_one_and_replace& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// Gets the current value of the comment option.
    ///
    /// @return
    ///  The current comment option.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const comment() const {
        return _comment;
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
    find_one_and_replace& max_time(std::chrono::milliseconds max_time) {
        _max_time = max_time;
        return *this;
    }

    ///
    /// The current max_time setting.
    ///
    /// @return the current max allowed running time (in milliseconds).
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& max_time() const {
        return _max_time;
    }

    ///
    /// Sets a projection, which limits the fields to return.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    find_one_and_replace& projection(bsoncxx::v_noabi::document::view_or_value projection) {
        _projection = std::move(projection);
        return *this;
    }

    ///
    /// Gets the current projection for this operation.
    ///
    /// @return The current projection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& projection() const {
        return _projection;
    }

    ///
    /// Set the desired version of the replaced document to return, either the original
    /// document, or the replacement. By default, the original document is returned.
    ///
    /// @param return_document
    ///   Version of document to return, either original or replaced.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    /// - @ref mongocxx::v_noabi::options::return_document
    ///
    find_one_and_replace& return_document(return_document return_document) {
        _return_document = return_document;
        return *this;
    }

    ///
    /// Which version of the replaced document to return.
    ///
    /// @return Version of document to return, either original or replacement.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    /// - @ref mongocxx::v_noabi::options::return_document
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::options::return_document> const& return_document() const {
        return _return_document;
    }

    ///
    /// Sets the order by which to search the collection for a matching document.
    ///
    /// @warning This can influence which document the operation modifies if the provided filter
    /// selects multiple documents.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    find_one_and_replace& sort(bsoncxx::v_noabi::document::view_or_value ordering) {
        _ordering = std::move(ordering);
        return *this;
    }

    ///
    /// Gets the current sort ordering.
    ///
    /// @return The current sort ordering.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& sort() const {
        return _ordering;
    }

    ///
    /// Sets the upsert flag on the operation. When @c true, the operation creates a new document if
    /// no document matches the filter. When @c false, this operation will do nothing if there are
    /// no matching documents. The server-side default is false.
    ///
    /// @param upsert
    ///   Whether or not to perform an upsert.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    find_one_and_replace& upsert(bool upsert) {
        _upsert = upsert;
        return *this;
    }

    ///
    /// Gets the current upsert setting.
    ///
    /// @return The current upsert setting.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& upsert() const {
        return _upsert;
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
    find_one_and_replace& write_concern(v_noabi::write_concern write_concern) {
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
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern() const {
        return _write_concern;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _let;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _comment;
    bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _projection;
    bsoncxx::v_noabi::stdx::optional<v_noabi::options::return_document> _return_document;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _ordering;
    bsoncxx::v_noabi::stdx::optional<bool> _upsert;
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> _write_concern;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::find_one_and_replace from_v1(v1::find_one_and_replace_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::find_one_and_replace_options to_v1(v_noabi::options::find_one_and_replace const& v) {
    return v1::find_one_and_replace_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::find_one_and_replace.
///
/// @par Includes
/// - @ref mongocxx/v1/find_one_and_replace_options.hpp
///
