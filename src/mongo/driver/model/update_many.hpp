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

#include <mongo/driver/config/prelude.hpp>

#include <mongo/bson/document.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace model {

///
/// Class representing a MongoDB update operation that modifies multiple documents.
///
class LIBMONGOCXX_API update_many {

   public:

    ///
    /// Constructs an update operation that will modify all documents matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for applying the update.
    /// @param update
    ///   Document representing the modifications to be applied to matching documents.
    ///
    update_many(bson::document::view filter, bson::document::view update);

    ///
    /// Gets the filter.
    ///
    /// @return The filter to be used for the update operation.
    ///
    const bson::document::view& filter() const;

    ///
    /// Gets the update document.
    ///
    /// @return The modifications to be applied as part of the update.
    ///
    const bson::document::view& update() const;

    ///
    /// Sets the upsert option.
    ///
    /// When upsert is @c false, the update operation does nothing when no documents match the filter.
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
    bson::document::view _filter;
    bson::document::view _update;

    // Optional
    stdx::optional<bool> _upsert;

};

}  // namespace model
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
