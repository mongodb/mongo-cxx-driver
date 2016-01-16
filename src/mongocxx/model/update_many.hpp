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

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace model {

///
/// Class representing a MongoDB update operation that modifies multiple documents.
///
class MONGOCXX_API update_many {
   public:
    ///
    /// Constructs an update operation that will modify all documents matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for applying the update.
    /// @param update
    ///   Document representing the modifications to be applied to matching documents.
    ///
    update_many(bsoncxx::document::view_or_value filter, bsoncxx::document::view_or_value update);

    ///
    /// Gets the filter.
    ///
    /// @return The filter to be used for the update operation.
    ///
    const bsoncxx::document::view_or_value& filter() const;

    ///
    /// Gets the update document.
    ///
    /// @return The modifications to be applied as part of the update.
    ///
    const bsoncxx::document::view_or_value& update() const;

    ///
    /// Sets the upsert option.
    ///
    /// When upsert is @c false, update does nothing when no documents match the filter.
    /// However, by specifying upsert as @c true, this operation either updates matching documents
    /// or inserts a new document using the update specification if no matching document exists.
    /// By default, upsert is unset by the driver, and the server-side default, @c false, is used.
    ///
    /// @param upsert
    ///   If set to @c true, creates a new document when no document matches the query criteria.
    ///   The server side default is @c false, which does not insert a new document if a match
    ///   is not found.
    ///
    update_many& upsert(bool upsert);

    ///
    /// Gets the current value of the upsert option.
    ///
    /// @return The optional value of the upsert option.
    ///
    const stdx::optional<bool>& upsert() const;

   private:
    // Required
    bsoncxx::document::view_or_value _filter;
    bsoncxx::document::view_or_value _update;

    // Optional
    stdx::optional<bool> _upsert;
};

}  // namespace model
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
