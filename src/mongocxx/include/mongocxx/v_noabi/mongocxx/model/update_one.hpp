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

#include <mongocxx/model/update_one-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/array/value.hpp>

#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

#include <initializer_list>
#include <utility>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/hint.hpp>
#include <mongocxx/pipeline.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

///
/// A MongoDB update operation that modifies a single document.
///
class update_one {
    //
    // Utility class supporting the convenience of {} meaning an empty bsoncxx::v_noabi::document.
    //
    // Users may not use this class directly.
    //
    // In places where driver methods take this class as a parameter, passing {} will
    // translate to a default-constructed bsoncxx::v_noabi::document::view_or_value,
    // regardless of other overloads taking other default-constructible types
    // for that parameter. This class avoids compiler ambiguity with such overloads.
    //
    // See update_one() for an example of such overloads.
    //
    class _empty_doc_tag {};

   public:
    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() update_one(v1::bulk_write::update_one op);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::bulk_write::update_one() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::bulk_write::update_one ret{
            bsoncxx::v1::document::value{to_v1(_filter.view())}, bsoncxx::v1::document::value{to_v1(_update.view())}};

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_array_filters) {
            ret.array_filters(bsoncxx::v1::array::value{to_v1(_array_filters->view())});
        }

        if (_upsert) {
            ret.upsert(*_upsert);
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_sort) {
            ret.sort(bsoncxx::v1::document::value{to_v1(_sort->view())});
        }

        return ret;
    }

    ///
    /// Constructs an update operation that will modify a single document matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for applying the update.
    /// @param update
    ///   Document representing the modifications to be applied to the matching document.
    ///
    update_one(bsoncxx::v_noabi::document::view_or_value filter, bsoncxx::v_noabi::document::view_or_value update)
        : _filter{std::move(filter)}, _update{std::move(update)} {}

    ///
    /// Constructs an update operation that will modify a single document matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for applying the update.
    /// @param update
    ///   Pipeline representing the modifications to be applied to the matching document.
    ///
    update_one(bsoncxx::v_noabi::document::view_or_value filter, pipeline const& update)
        : update_one{std::move(filter), bsoncxx::v_noabi::document::value{update.view_array()}} {}

    ///
    /// Constructs an update operation that will modify a single document matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for applying the update.
    /// @param update
    ///   Supports the empty update {}.
    ///
    update_one(bsoncxx::v_noabi::document::view_or_value filter, std::initializer_list<_empty_doc_tag> update)
        : _filter{std::move(filter)} {
        (void)update;
    }

    ///
    /// Gets the filter
    ///
    /// @return The filter to be used for the update operation.
    ///
    bsoncxx::v_noabi::document::view_or_value const& filter() const {
        return _filter;
    }

    ///
    /// Gets the update document.
    ///
    /// @return The modifications to be applied as part of the update.
    ///
    bsoncxx::v_noabi::document::view_or_value const& update() const {
        return _update;
    }

    ///
    /// Sets the collation for this update operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    update_one& collation(bsoncxx::v_noabi::document::view_or_value collation) {
        _collation = std::move(collation);
        return *this;
    }

    ///
    /// Gets the collation option for this update operation.
    ///
    /// @return
    ///   The optional value of the collation option.
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
    update_one& hint(mongocxx::v_noabi::hint index_hint) {
        _hint = std::move(index_hint);
        return *this;
    }

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> const& hint() const {
        return _hint;
    }

    ///
    /// Set the sort option.
    ///
    update_one& sort(bsoncxx::v_noabi::document::view_or_value sort) {
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
    /// Sets the upsert option.
    ///
    /// When upsert is @c false, if no document matches the filter, update does nothing.
    /// However, by specifying upsert as @c true, this operation either updates a matching document
    /// or inserts a new document using the update specification if no matching document exists.
    /// By default upsert is @c false.
    ///
    /// @param upsert
    ///   If set to @c true, creates a new document when no document matches the query criteria.
    ///   The server side default is @c false, which does not insert a new document if a match
    ///   is not found.
    ///
    update_one& upsert(bool upsert) {
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
    /// Set array filters for this update operation.
    ///
    /// @param array_filters
    ///   Array representing filters determining which array elements to modify.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    update_one& array_filters(bsoncxx::v_noabi::array::view_or_value array_filters) {
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
    bsoncxx::v_noabi::document::view_or_value _filter;
    bsoncxx::v_noabi::document::view_or_value _update;

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::array::view_or_value> _array_filters;
    bsoncxx::v_noabi::stdx::optional<bool> _upsert;
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _sort;
};

} // namespace model
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::model::update_one from_v1(v1::bulk_write::update_one v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::bulk_write::update_one to_v1(v_noabi::model::update_one const& v) {
    return v1::bulk_write::update_one{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::model::update_one.
///
/// @par Includes
/// - @ref mongocxx/v1/bulk_write.hpp
///
