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

#include <mongocxx/model/replace_one-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/hint.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

///
/// A MongoDB update operation that replaces a single document.
///
class replace_one {
   public:
    ///
    /// Constructs an update operation that will replace a single document matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for replacement.
    /// @param replacement
    ///   Document that will serve as the replacement.
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    replace_one(
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value replacement);

    ///
    /// Gets the filter for replacement.
    ///
    /// @return The filter to be used for the replacement operation.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view_or_value const&) filter() const;

    ///
    /// Gets the replacement document.
    ///
    /// @return The document that will replace the original selected document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view_or_value const&) replacement() const;

    ///
    /// Sets the collation for this replacement operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&)
    collation(bsoncxx::v_noabi::document::view_or_value collation);

    ///
    /// Gets the collation option for this replacement operation.
    ///
    /// @return
    ///   The optional value of the collation option.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const&)
    collation() const;

    ///
    /// Sets the upsert option.
    ///
    /// When upsert is @c true, this operation will insert the replacement document as a new
    /// document if no existing documents match the filter. When upsert is @c false, the
    /// replacement operation does nothing if there are no matching documents. By default,
    /// upsert is @c false.
    ///
    /// @param upsert
    ///   If set to @c true, creates a new document when no document matches the query criteria.
    ///   The server side default is @c false, which does not insert a new document if a match
    ///   is not found.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) upsert(bool upsert);

    ///
    /// Gets the current value of the upsert option.
    ///
    /// @return The optional value of the upsert option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bool> const&) upsert() const;

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
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) hint(mongocxx::v_noabi::hint index_hint);

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> const&)
    hint() const;

    ///
    /// Set the sort option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) sort(bsoncxx::v_noabi::document::view_or_value sort);

    ///
    /// Get the current value of the sort option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const&)
    sort() const;

   private:
    bsoncxx::v_noabi::document::view_or_value _filter;
    bsoncxx::v_noabi::document::view_or_value _replacement;

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<bool> _upsert;
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _sort;
};

} // namespace model
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::model::replace_one.
///
