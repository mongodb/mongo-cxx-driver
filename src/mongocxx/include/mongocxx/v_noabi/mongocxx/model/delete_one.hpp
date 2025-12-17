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

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/model/delete_one-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

#include <utility>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/hint.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

///
/// A MongoDB delete operation that removes a single document.
///
class delete_one {
   public:
    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() delete_one(v1::bulk_write::delete_one op);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::bulk_write::delete_one() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::bulk_write::delete_one ret{bsoncxx::v1::document::value{to_v1(_filter.view())}};

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        return ret;
    }

    ///
    /// Constructs a delete operation that will delete the first document matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for deletion.
    ///
    delete_one(bsoncxx::v_noabi::document::view_or_value filter) : _filter{std::move(filter)} {}

    ///
    /// Gets the filter on this delete operation.
    ///
    /// @return The filter to be used for the delete operation.
    ///
    bsoncxx::v_noabi::document::view_or_value const& filter() const {
        return _filter;
    }

    ///
    /// Sets the collation for this delete operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    delete_one& collation(bsoncxx::v_noabi::document::view_or_value collation) {
        _collation = std::move(collation);
        return *this;
    }

    ///
    /// Gets the collation option for this delete operation.
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
    ///
    delete_one& hint(mongocxx::v_noabi::hint index_hint) {
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

   private:
    bsoncxx::v_noabi::document::view_or_value _filter;

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> _hint;
};

} // namespace model
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::model::delete_one from_v1(v1::bulk_write::delete_one v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::bulk_write::delete_one to_v1(v_noabi::model::delete_one const& v) {
    return v1::bulk_write::delete_one{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::model::delete_one.
///
/// @par Includes
/// - @ref mongocxx/v1/bulk_write.hpp
///
