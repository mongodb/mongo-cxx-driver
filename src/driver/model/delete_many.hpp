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

#include "driver/config/prelude.hpp"

#include "bson/document.hpp"

namespace mongo {
namespace driver {
namespace model {

///
/// Class representing a MongoDB delete operation that removes multiple documents.
///
class LIBMONGOCXX_EXPORT delete_many {

   public:

    ///
    /// Constructs a delete operation that will delete all documents matching the filter.
    ///
    /// @param filter
    ///   Document representing the criteria for deletion.
    ///
    explicit delete_many(bson::document::view filter);

    ///
    /// Gets the filter for this delete operation.
    ///
    /// @return The filter to be used for the delete operation.
    ///
    const bson::document::view& filter() const;

   private:
    // Required
    bson::document::view _filter;

};

}  // namespace model
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
