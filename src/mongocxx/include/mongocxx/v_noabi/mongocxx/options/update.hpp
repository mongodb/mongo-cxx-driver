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

#include <mongocxx/options/update-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/update_many_options.hpp> // IWYU pragma: export
#include <mongocxx/v1/update_one_options.hpp>  // IWYU pragma: export

#include <utility>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/hint.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::collection.
///
class update {
   public:
    ///
    /// Default initialization.
    ///
    update() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() update(v1::update_many_options opts);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() update(v1::update_one_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @note The "sort" field is ignored.
    ///
    explicit operator v1::update_many_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::update_many_options ret;

        if (_bypass_document_validation) {
            ret.bypass_document_validation(*_bypass_document_validation);
        }

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_upsert) {
            ret.upsert(*_upsert);
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_array_filters) {
            ret.array_filters(bsoncxx::v1::array::value{to_v1(*_array_filters)});
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        return ret;
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::update_one_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::update_one_options ret;

        if (_bypass_document_validation) {
            ret.bypass_document_validation(*_bypass_document_validation);
        }

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_upsert) {
            ret.upsert(*_upsert);
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_array_filters) {
            ret.array_filters(bsoncxx::v1::array::value{to_v1(*_array_filters)});
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_sort) {
            ret.sort(bsoncxx::v1::document::value{to_v1(_sort->view())});
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        return ret;
    }

    ///
    /// Sets the bypass_document_validation option.
    /// If true, allows the write to opt-out of document level validation.
    ///
    /// @note
    ///   On servers >= 3.2, the server applies validation by default. On servers < 3.2, this option
    ///   is ignored.
    ///
    /// @param bypass_document_validation
    ///   Whether or not to bypass document validation
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    update& bypass_document_validation(bool bypass_document_validation) {
        _bypass_document_validation = bypass_document_validation;
        return *this;
    }

    ///
    /// Gets the current value of the bypass_document_validation option.
    ///
    /// @return The optional value of the bypass_document_validation option.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& bypass_document_validation() const {
        return _bypass_document_validation;
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
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    update& collation(bsoncxx::v_noabi::document::view_or_value collation) {
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
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& collation() const {
        return _collation;
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
    update& hint(v_noabi::hint index_hint) {
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
    update& let(bsoncxx::v_noabi::document::view_or_value let) {
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
    /// Set the sort option.
    ///
    update& sort(bsoncxx::v_noabi::document::view_or_value sort) {
        _sort = std::move(sort);
        return *this;
    }

    ///
    /// Get the current value of the sort option.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& sort() const {
        return _sort;
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
    update& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
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
    /// Sets the upsert option.
    ///
    /// By default, if no document matches the filter, the update operation does nothing.
    /// However, by specifying upsert as @c true, this operation either updates matching documents
    /// or inserts a new document using the update specification if no matching document exists.
    ///
    /// @param upsert
    ///   If set to @c true, creates a new document when no document matches the query criteria.
    ///   The server-side default is @c false, which does not insert a new document if a match
    ///   is not found.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    update& upsert(bool upsert) {
        _upsert = upsert;
        return *this;
    }

    ///
    /// Gets the current value of the upsert option.
    ///
    /// @return The optional value of the upsert option.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& upsert() const {
        return _upsert;
    }

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/write-concern/
    ///
    update& write_concern(v_noabi::write_concern wc) {
        _write_concern = std::move(wc);
        return *this;
    }

    ///
    /// The current write_concern for this operation.
    ///
    /// @return
    ///   The current write_concern
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/write-concern/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern() const {
        return _write_concern;
    }

    ///
    /// Set array filters for this operation.
    ///
    /// @param array_filters
    ///   Array representing filters determining which array elements to modify.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    update& array_filters(bsoncxx::v_noabi::array::view_or_value array_filters) {
        _array_filters = std::move(array_filters);
        return *this;
    }

    ///
    /// Get array filters for this operation.
    ///
    /// @return
    ///   The current array filters.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::array::view_or_value> const& array_filters() const {
        return _array_filters;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<bool> _upsert;
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> _write_concern;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::array::view_or_value> _array_filters;
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _let;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _sort;
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
inline v_noabi::options::update from_v1(v1::update_many_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::update from_v1(v1::update_one_options v) {
    return {std::move(v)};
}

// Ambiguous whether `v_noabi::options::update` should be converted to `v1::update_many_options` or
// `v1::update_one_options`. Require users to explicitly cast to the expected type instead.
//
// v1::update_many_options to_v1(v_noabi::options::update const& v);
// v1::update_one_options to_v1(v_noabi::options::update const& v);

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::update.
///
/// @par Includes
/// - @ref mongocxx/v1/update_many_options.hpp
/// - @ref mongocxx/v1/update_one_options.hpp
///
