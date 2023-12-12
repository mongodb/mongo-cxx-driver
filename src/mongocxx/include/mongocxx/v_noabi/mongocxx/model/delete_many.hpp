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

#include <mongocxx/model/delete_many-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
inline namespace wip {
namespace model {

///
/// Class representing a MongoDB delete operation that removes multiple documents.
///
class delete_many {
   public:
    ///
    /// Constructs a delete operation that will delete all documents matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for deletion.
    ///
    explicit delete_many(bsoncxx::document::view_or_value filter);

    ///
    /// Gets the filter for this delete operation.
    ///
    /// @return The filter to be used for the delete operation.
    ///
    const bsoncxx::document::view_or_value& filter() const;

    ///
    /// Sets the collation for this delete operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    ///   https://www.mongodb.com/docs/manual/reference/collation/
    ///
    delete_many& collation(bsoncxx::document::view_or_value collation);

    ///
    /// Gets the collation option for this delete operation.
    ///
    /// @return
    ///   The optional value of the collation option.
    ///
    /// @see
    ///   https://www.mongodb.com/docs/manual/reference/collation/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& collation() const;

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
    delete_many& hint(mongocxx::hint index_hint);

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    const stdx::optional<mongocxx::hint>& hint() const;

   private:
    bsoncxx::document::view_or_value _filter;

    stdx::optional<bsoncxx::document::view_or_value> _collation;
    stdx::optional<mongocxx::hint> _hint;
};

}  // namespace model
}  // namespace wip
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
